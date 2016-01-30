#include <Arduino.h>

#include <stdlib.h>
#include <errno.h>

#include <MySensor.h>
#include <Button.h>
#include <SimpleTimer.h>
#include <LEDFader.h>
#include <EEPROM.h>

#define PIN_IN_RED  3
#define PIN_IN_GREEN  5
#define PIN_IN_BLUE  6
#define PIN_IN_BUTTON  7
#define PIN_IN_WHITE  9

#define MS_RF24_CE_PIN      4       //<-- NOTE!!! changed, the default is 9                                                                                                                                     ░
#define MS_RF24_CS_PIN      10      // default is 10                                                                                                                                                            ░
#define MS_RF24_PA_LEVEL    RF24_PA_MAX

#define FADE_TIME_MS    3000
#define HEARTBEAT_PERIOD_MS  60 * 60 * 1000U

#define SERIAL_SPEED    115200U

#define MS_NODE_ID         3
#define MS_SENSOR_LEDS_ID  0

void on_heartbeat_timer();

struct RGBW {
    byte R;
    byte G;
    byte B;
    byte W;
} __attribute__((packed));

struct Storage
{
    bool has_values;
    RGBW color;
}  __attribute__((packed));

Storage storage = {false, {0, 0, 0, 0}};

bool leds_on = false;

Button button(PIN_IN_BUTTON);
LEDFader ledW;
LEDFader ledR;
LEDFader ledG;
LEDFader ledB;

MyTransportNRF24 transport(MS_RF24_CE_PIN, MS_RF24_CS_PIN, MS_RF24_PA_LEVEL);
MySensor gw(transport);
MyMessage msgLedStatus(MS_SENSOR_LEDS_ID, S_LIGHT);

SimpleTimer hearbeat_timer;

void log(const __FlashStringHelper *fmt, ... ){
    static char buf[128];
    va_list args;
    va_start (args, fmt);
    vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
    va_end(args);
    Serial.println(buf);
    Serial.flush();
}

#define LOG_DEBUG(_msg, ...) log(F("[DEBUG] " _msg) , ##__VA_ARGS__);
#define LOG_INFO(_msg, ...) log(F("[INFO] " _msg) , ##__VA_ARGS__);
#define LOG_ERROR(_msg, ...) log(F("[ERROR] " _msg) , ##__VA_ARGS__);

void setup()
{
    Serial.begin(SERIAL_SPEED);
    LOG_DEBUG("Setup has been started.")

    LOG_DEBUG("EEPROM restore...");
    load_led_values();

    LOG_INFO("Leds setup...");
    ledR = LEDFader(PIN_IN_RED);
    ledG = LEDFader(PIN_IN_GREEN);
    ledB = LEDFader(PIN_IN_BLUE);
    ledW = LEDFader(PIN_IN_WHITE);

    LOG_INFO("Button setup...");
    button.init();
    button.set_minimum_gap(20);
    button.set_maximum_gap(1000);
    button.on_press(on_btn_press);

    LOG_INFO("MySensors setup...");
    gw.begin(on_message, MS_NODE_ID);
    gw.sendSketchInfo("KitchenLEDs", "0.1");
    gw.present(MS_SENSOR_LEDS_ID, S_RGBW_LIGHT, "LEDs");

    switch_leds_off();

    hearbeat_timer.setInterval(HEARTBEAT_PERIOD_MS, on_heartbeat_timer);
    on_heartbeat_timer();

    LOG_INFO("Setup done");
}

void loop()
{
    button.init();

    gw.process();

    ledR.update();
    ledG.update();
    ledB.update();
    ledW.update();

    hearbeat_timer.run();
}

void on_btn_press()
{
    leds_on = !leds_on;

    LOG_DEBUG("Button press");

    if (leds_on) {
        restore_leds();
    }
    else {
        switch_leds_off();
    }
}

void on_heartbeat_timer()
{
    gw.sendBatteryLevel(100);
}

void on_message(const MyMessage & message)
{
    if (message.sensor != MS_SENSOR_LEDS_ID) {
        LOG_ERROR("Got message from unexpected sensor: (sensor=%d, type=%d)", message.sensor, message.type);
        return;
    }

    if (message.type == V_RGBW) {
        on_message_set_rgbw(message);
    }
    else if (message.type == V_STATUS) {
        on_message_set_status(message);
    }
    else {
        LOG_ERROR("Got message with unexpected type: (sensor=%d, type=%d)", message.sensor, message.type);
    }
}

void on_message_set_rgbw(const MyMessage & message)
{
    const char * rgb_string = message.getString();
    LOG_DEBUG("=> Message: sensor=%d, type=%d, value=%s", message.sensor, message.type,
              rgb_string);
    RGBW rgbw = string_to_rgbw(rgb_string);

    fade_leds(rgbw.R, rgbw.G, rgbw.B, rgbw.W);
    save_led_values(rgbw);
}

void on_message_set_status(const MyMessage & message)
{
    bool status = message.getBool();
    LOG_DEBUG("=> Message: sensor=%d, type=%d, value=%d", message.sensor, message.type, status);

    if (status) {
        restore_leds();
    }
    else {
        switch_leds_off();
    }
}

void save_led_values(const RGBW & rgbw)
{
    storage.has_values = true;
    storage.color = rgbw;
    EEPROM.put(0, storage);
}

void load_led_values()
{
    EEPROM.get(0, storage);
    if (!storage.has_values) {
        storage.color.R = 255;
        storage.color.G = 255;
        storage.color.B = 255;
        storage.color.W = 255;
    }
}

void restore_leds()
{
    fade_leds(storage.color.R, storage.color.G, storage.color.B, 255);
    leds_on = true;
    send(msgLedStatus.set(1));
}

void switch_leds_on(byte R, byte G, byte B, byte W)
{
    fade_leds(R, G, B, W);
    leds_on = true;
    send(msgLedStatus.set(1));
}

void switch_leds_off()
{
    fade_leds(0, 0, 0, 0);
    leds_on = false;
    send(msgLedStatus.set(0));
}

void fade_leds(byte R, byte G, byte B, byte W)
{
    ledR.fade(R, FADE_TIME_MS);
    ledG.fade(G, FADE_TIME_MS);
    ledB.fade(B, FADE_TIME_MS);
    ledW.fade(W, FADE_TIME_MS);
    LOG_INFO("LEDs are fading to R=0x%x, G=0x%x, B=0x%x, W=0x%x, time=%dms", R, G, B, W, FADE_TIME_MS);
}

void send(MyMessage & message)
{
    if (!gw.send(message)) {
        LOG_ERROR("Message (sensor=%d, type=%d) doesn't reach a next node", message.sensor, message.type);
    }
}

RGBW string_to_rgbw(const char * str)
{
    RGBW result = {0, 0, 0, 0};

    if (!str) {
        LOG_ERROR("RGBW string is NULL");
        return {0, 0, 0, 0};
    }

    size_t len = strlen(str);
    if (len != 4*2) {
        LOG_ERROR("Unexpected length of RGBW string: %d", len);
        return {0, 0, 0, 0};
    }

    char buffer[9];
    strncpy(buffer, str, sizeof(buffer));

    result.W = strtol(buffer+6, nullptr, 16);
    if (errno != 0) {
        LOG_ERROR("Fail to parse W in string: %s", str);
        return {0, 0, 0, 0};
    }
    buffer[6] = '\0';
    result.B = strtol(buffer+4, nullptr, 16);
    if (errno != 0) {
        LOG_ERROR("Fail to parse B in string: %s", str);
        return {0, 0, 0, 0};
    }
    buffer[4] = '\0';
    result.G = strtol(buffer+2, nullptr, 16);
    if (errno != 0) {
        LOG_ERROR("Fail to parse G in string: %s", str);
        return {0, 0, 0, 0};
    }
    buffer[2] = '\0';
    result.R = strtol(buffer, nullptr, 16);
    if (errno != 0) {
        LOG_ERROR("Fail to parse R in string: %s", str);
        return {0, 0, 0, 0};
    }

    return result;
}
