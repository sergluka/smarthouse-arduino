#include <Arduino.h>

#include <MySensor.h>
#include <EEPROM.h>
#include <NewButton.h>
#include <LEDFader.h>
#include <Logging.h>

#include "Types.h"
#include "Parsing.h"

#define VERSION "0.2"
#define STORAGE_VERSION 1

#define PIN_IN_RED  3
#define PIN_IN_GREEN  5
#define PIN_IN_BLUE  6
#define PIN_IN_BUTTON  7
#define PIN_IN_WHITE  9

#define MS_RF24_CE_PIN      4       //<-- NOTE!!! changed, the default is 9                                                                                                                                     ░
#define MS_RF24_CS_PIN      10
#define MS_RF24_PA_LEVEL    RF24_PA_MAX

#define FADE_TIME_MS    3000

#define SERIAL_SPEED    115200U

#define MS_NODE_ID         3
#define MS_SENSOR_COLOR_LEDS_ID  0
#define MS_SENSOR_WHITE_LED_ID   1

struct Storage
{
    byte version;
    RGBW color;
}  __attribute__((packed));

Storage storage = {STORAGE_VERSION, {0, 0, 0, 0}};

bool leds_on = false;

NewButton button(PIN_IN_BUTTON);
LEDFader ledW;
LEDFader ledR;
LEDFader ledG;
LEDFader ledB;

MyTransportNRF24 transport(MS_RF24_CE_PIN, MS_RF24_CS_PIN, MS_RF24_PA_LEVEL);
MySensor gw(transport);
MyMessage msgColorLedStatus(MS_SENSOR_COLOR_LEDS_ID, S_LIGHT);
MyMessage msgWhiteLedStatus(MS_SENSOR_WHITE_LED_ID, S_LIGHT);

void setup()
{
    LOGGING_SETUP(SERIAL_SPEED);
    LOG_DEBUG("Setup has been started.")

    LOG_DEBUG("EEPROM restore...");
    load_led_values();

    LOG_INFO("Leds setup...");
    ledR = LEDFader(PIN_IN_RED);
    ledG = LEDFader(PIN_IN_GREEN);
    ledB = LEDFader(PIN_IN_BLUE);
    ledW = LEDFader(PIN_IN_WHITE);

    LOG_INFO("Button setup...");
    button.setup();
    button.on_short_release(on_btn_short_release);

    LOG_INFO("MySensors setup...");
    gw.begin(on_message, MS_NODE_ID);
    gw.sendSketchInfo("KitchenLEDs", VERSION);
    gw.present(MS_SENSOR_COLOR_LEDS_ID, S_RGB_LIGHT, "Color LEDs");
    gw.present(MS_SENSOR_WHITE_LED_ID, S_LIGHT, "White LED");

    switch_leds_off();

    LOG_INFO("Setup done");
}

void loop()
{
    button.process();

    gw.process();

    ledR.update();
    ledG.update();
    ledB.update();
    ledW.update();
}

void on_btn_short_release()
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

void on_message(const MyMessage & message)
{
    if (message.sensor != MS_SENSOR_COLOR_LEDS_ID && message.sensor != MS_SENSOR_WHITE_LED_ID) {
        LOG_ERROR("Got message from unexpected sensor: (sensor=%d, type=%d)", message.sensor, message.type);
        return;
    }

    if (message.type == V_VAR1) {
        on_message_var1(message);
    }
//    else if (message.type == V_STATUS) {
//        on_message_set_status(message);
//    }
//    else if (message.type == V_VAR1) {
//        on_message_set_status(message);
//    }
    else {
        LOG_ERROR("Got message with unexpected type: (sensor=%d, type=%d)", message.sensor, message.type);
    }
}

void on_message_var1(const MyMessage & message)
{
    const char * payload = message.getString();
    LOG_DEBUG("=> Message: sensor=%d, type=%d, data=%s", message.sensor, message.type, payload);

    LedType led_type;
    if (message.sensor == MS_SENSOR_COLOR_LEDS_ID) {
        led_type = LedType::Color;
    }
    else if (message.sensor == MS_SENSOR_WHITE_LED_ID) {
        led_type = LedType::White;
    }
    else {
        LOG_ERROR("Unknown sensor");
        return;
    }

    CommandData data = parse_command(payload, led_type);
}

//void on_message_set_rgbw(const MyMessage & message)
//{
//    const char * rgb_string = message.getString();
//    LOG_DEBUG("=> Message: sensor=%d, type=%d, value=%s", message.sensor, message.type,
//              rgb_string);
//    RGBW rgbw = string_to_rgbw(rgb_string);
//
//    fade_leds(rgbw.R, rgbw.G, rgbw.B, rgbw.W);
//    save_led_values(rgbw);
//}

//void on_message_set_status(const MyMessage & message)
//{
//    bool status = message.getBool();
//    LOG_DEBUG("=> Message: sensor=%d, type=%d, value=%d", message.sensor, message.type, status);
//
//    if (status) {
//        restore_leds();
//    }
//    else {
//        switch_leds_off();
//    }
//}

void save_led_values(const RGBW & rgbw)
{
    storage.color = rgbw;
    EEPROM.put(0, storage);
}

void load_led_values()
{
    EEPROM.get(0, storage);
    if (storage.version != STORAGE_VERSION) {
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
    send(msgColorLedStatus.set(1));
    send(msgWhiteLedStatus.set(1));
}

void switch_leds_on(byte R, byte G, byte B, byte W)
{
    fade_leds(R, G, B, W);
    leds_on = true;
    send(msgColorLedStatus.set(1));
    send(msgWhiteLedStatus.set(1));
}

void switch_leds_off()
{
    fade_leds(0, 0, 0, 0);
    leds_on = false;
    send(msgColorLedStatus.set(0));
    send(msgWhiteLedStatus.set(0));
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
