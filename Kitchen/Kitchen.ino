#include <Arduino.h>

#include <MySensor.h>
#include <EEPROM.h>
#include <NewButton.h>
#include <Logging.h>

#include "Types.h"
#include "LEDs.h"
#include "Parsing.h"

#define VERSION "0.2"
#define STORAGE_VERSION 1

#define PIN_IN_BUTTON  7

#define MS_RF24_CE_PIN      4       //<-- NOTE!!! changed, the default is 9                                                                                                                                     ░
#define MS_RF24_CS_PIN      10
#define MS_RF24_PA_LEVEL    RF24_PA_MAX

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

NewButton button(PIN_IN_BUTTON);

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

    leds_process();
}

void on_btn_short_release()
{
    LOG_DEBUG("Button press");

    if (is_leds_on()) {
        switch_leds_off();
    }
    else {
        restore_leds();
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
    switch (data.command) {
    case Command::LIGHT_OFF:
        on_message_light_off(message.sensor);
        break;
    case Command::LIGHT_ON:
        on_message_light_on(message.sensor);
        break;
    case Command::LIGHT_SET_LIMIT:
        on_message_set_limit(message.sensor, data.limit);
        break;
    default:
        break;
    }
}

void on_message_light_off(uint8_t sensor)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_color_fade(0, 0, 0);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_white_fade(0);
    }
}

void on_message_light_on(uint8_t sensor)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_color_fade(storage.color.R, storage.color.G, storage.color.B);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_white_fade(storage.color.W);
    }

}

void on_message_set_limit(uint8_t sensor, RGBW rgbw)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        storage.color.R = rgbw.R;
        storage.color.G = rgbw.G;
        storage.color.B = rgbw.B;
        leds_color_fade(storage.color.R, storage.color.G, storage.color.B);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        storage.color.W = rgbw.W;
        leds_white_fade(storage.color.W);
    }

    save_led_values(storage.color);
}

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
    leds_fade(storage.color.R, storage.color.G, storage.color.B, storage.color.W);
    send(msgColorLedStatus.set(1));
    send(msgWhiteLedStatus.set(1));
}

void switch_leds_off()
{
    leds_fade(0, 0, 0, 0);
    send(msgColorLedStatus.set(0));
    send(msgWhiteLedStatus.set(0));
}

void send(MyMessage & message)
{
    if (!gw.send(message)) {
        LOG_ERROR("Message (sensor=%d, type=%d) doesn't reach a next node", message.sensor, message.type);
    }
}
