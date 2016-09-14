#include "Networking.h"

#include <Logging.h>

#define MY_NODE_ID          4
#define MY_RF24_CE_PIN      4       //<-- NOTE!!! changed, the default is 9                                                                                                                                     ░
#define MY_RF24_CS_PIN      10
#define MY_RF24_PA_LEVEL    RF24_PA_MAX
#define MY_RADIO_NRF24
#include <MySensors.h>

#include "Types.h"
#include "Parsing.h"
#include "Storage.h"
#include "LEDs.h"

#define VERSION "0.4"

#define MS_SENSOR_COLOR_LEDS_ID  0
#define MS_SENSOR_WHITE_LED_ID   1

MyMessage msgColorLedStatus(MS_SENSOR_COLOR_LEDS_ID, V_LIGHT);
MyMessage msgWhiteLedStatus(MS_SENSOR_WHITE_LED_ID, V_LIGHT);

void presentation()
{
    LOG_INFO("MySensors setup...");
    sendSketchInfo("KitchenLEDs", VERSION);
    present(MS_SENSOR_COLOR_LEDS_ID, S_RGB_LIGHT, "Color LEDs");
    present(MS_SENSOR_WHITE_LED_ID, S_LIGHT, "White LED");
}

void on_message_set_limit(uint8_t sensor, RGBW rgbw, SwitchingSource source)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_start_transition(LedType::Color, false);

        save_color_leds(source, rgbw.R, rgbw.G, rgbw.B);
        leds_color_fade(rgbw.R, rgbw.G, rgbw.B);
        network_send_color_status(true);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_start_transition(LedType::White, false);

        save_white_leds(source, rgbw.W);
        leds_white_fade(rgbw.W);
        network_send_white_status(true);
    }
}

void on_message_set_transition(uint8_t sensor, const Transition & transition)
{
    if (sensor == MS_SENSOR_COLOR_LEDS_ID) {
        leds_set_transition(LedType::Color, transition);
        leds_start_transition(LedType::Color, true);
        network_send_color_status(true);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_set_transition(LedType::White, transition);
        leds_start_transition(LedType::White, true);
        network_send_white_status(true);
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
        restore_color_leds(SwitchingSource::External);
        network_send_color_status(true);
    }
    else if (sensor == MS_SENSOR_WHITE_LED_ID) {
        leds_start_transition(LedType::White, false);
        restore_white_leds(SwitchingSource::External);
        network_send_white_status(true);
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
    case Command::LIGHT_SET_EXT_LIMIT:
        on_message_set_limit(message.sensor, data.limit, SwitchingSource::External);
        break;
    case Command::LIGHT_SET_BUTTON_LIMIT:
        on_message_set_limit(message.sensor, data.limit, SwitchingSource::Button);
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

void receive(const MyMessage & message)
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

void send_message(MyMessage & message)
{
    if (!send(message)) {
        LOG_ERROR("Message (sensor=%d, type=%d) doesn't reach a next node", message.sensor, message.type);
    }
}

void network_send_color_status(bool on)
{
    send_message(msgColorLedStatus.set(on ? 1 : 0));
}

void network_send_white_status(bool on)
{
    send_message(msgWhiteLedStatus.set(on ? 1 : 0));
}
