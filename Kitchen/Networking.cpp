#include "Networking.h"

#include <Logging.h>

#include "Types.h"
#include "Parsing.h"
#include "Storage.h"
#include "LEDs.h"

#define VERSION "0.2"

#define MS_RF24_CE_PIN      4       //<-- NOTE!!! changed, the default is 9                                                                                                                                     ░
#define MS_RF24_CS_PIN      10
#define MS_RF24_PA_LEVEL    RF24_PA_MAX

#define MS_NODE_ID         3
#define MS_SENSOR_COLOR_LEDS_ID  0
#define MS_SENSOR_COLOR_LEDS_ID  0
#define MS_SENSOR_WHITE_LED_ID   1

MyTransportNRF24 transport(MS_RF24_CE_PIN, MS_RF24_CS_PIN, MS_RF24_PA_LEVEL);
MySensor gw(transport);
MyMessage msgColorLedStatus(MS_SENSOR_COLOR_LEDS_ID, S_LIGHT);
MyMessage msgWhiteLedStatus(MS_SENSOR_WHITE_LED_ID, S_LIGHT);

void on_message(const MyMessage & message);

void network_setup()
{
    gw.begin(on_message, MS_NODE_ID);
    gw.sendSketchInfo("KitchenLEDs", VERSION);
    gw.present(MS_SENSOR_COLOR_LEDS_ID, S_RGB_LIGHT, "Color LEDs");
    gw.present(MS_SENSOR_WHITE_LED_ID, S_LIGHT, "White LED");
}

void network_process()
{
    gw.process();
}

void on_message_set_limit(uint8_t sensor, RGBW rgbw)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_start_transition(LedType::Color, false);

        save_color_leds(rgbw.R, rgbw.G, rgbw.B);
        leds_color_fade(rgbw.R, rgbw.G, rgbw.B);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_start_transition(LedType::White, false);

        save_white_leds(rgbw.W);
        leds_white_fade(rgbw.W);
    }
}

void on_message_set_transition(uint8_t sensor, const Transition & transition)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_set_transition(LedType::Color, transition);
        leds_start_transition(LedType::Color, true);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_set_transition(LedType::White, transition);
        leds_start_transition(LedType::White, true);
    }
}

void on_message_light_off(uint8_t sensor)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_start_transition(LedType::Color, false);
        leds_color_fade(0, 0, 0);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_start_transition(LedType::White, false);
        leds_white_fade(0);
    }
}

void on_message_light_on(uint8_t sensor)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_start_transition(LedType::Color, false);
        restore_color_leds();
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_start_transition(LedType::White, false);
        restore_white_leds();
    }
}

void on_message_light(const MyMessage & message)
{
    bool status = message.getBool();
    LOG_DEBUG("=> Message: sensor=%d, type=%d, status=%d", message.sensor, message.type, status);

    if (status) {
        on_message_light_on(message.sensor);
    }
    else {
        on_message_light_off(message.sensor);
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
    case Command::LIGHT_SET_LIMIT:
        on_message_set_limit(message.sensor, data.limit);
        break;
    case Command::TRANSITION_ONE_SHOT:
    case Command::TRANSITION_LOOP:
        on_message_set_transition(message.sensor, data.transition);
        break;
    default:
        break;
    case NONE:break;
    }
}

void on_message(const MyMessage & message)
{
    if (message.sensor != MS_SENSOR_COLOR_LEDS_ID && message.sensor != MS_SENSOR_WHITE_LED_ID) {
        LOG_ERROR("Got message from unexpected sensor: (sensor=%d, type=%d)", message.sensor, message.type);
        return;
    }

    if (message.type == V_LIGHT) {
        on_message_light(message);
    }
    else if (message.type == V_VAR1) {
        on_message_var1(message);
    }
    else {
        LOG_ERROR("Got message with unexpected type: (sensor=%d, type=%d)", message.sensor, message.type);
    }
}

void send(MyMessage & message)
{
    if (!gw.send(message)) {
        LOG_ERROR("Message (sensor=%d, type=%d) doesn't reach a next node", message.sensor, message.type);
    }
}

void network_send_color_status(bool on)
{
    send(msgColorLedStatus.set(on ? 1 : 0));
}

void network_send_white_status(bool on)
{
    send(msgWhiteLedStatus.set(on ? 1 : 0));
}
