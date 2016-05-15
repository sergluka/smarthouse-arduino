#include <Arduino.h>

#include <Logging.h>
#include <NewButton.h>

#include "Types.h"
#include "LEDs.h"
#include "Parsing.h"
#include "Storage.h"
#include "Networking.h"

#define PIN_IN_BUTTON  7

#define SERIAL_SPEED    115200U

enum class ButtonMode {
    Both,
    Color,
    White
};

NewButton button(PIN_IN_BUTTON);
ButtonMode button_mode = ButtonMode::Both;

void setup()
{
    LOGGING_SETUP(SERIAL_SPEED);
    LOG_DEBUG("Setup has been started.")

    LOG_DEBUG("EEPROM restore...");
    storage_setup();

    LOG_INFO("Button setup...");
    button.setup();
    button.on_short_release(on_btn_short_release);
    button.on_long_press(on_btn_long_press);

    LOG_INFO("MySensors setup...");
    network_setup();

    LOG_INFO("Leds setup");
    leds_setup();

    LOG_INFO("Setup done");
}

void loop()
{
    button.process();
    network_process();
    leds_process();
}

void on_btn_short_release(void *)
{
    LOG_DEBUG("Button short release");

    bool status = is_stored_equal(SwitchingSource::Button, led_values());
    switch_leds_by_button(!status);
}

void on_btn_long_press(void *)
{
    LOG_DEBUG("Button long press");

    switch (button_mode) {
    case ButtonMode::Both:
        button_mode = ButtonMode::Color;
        break;
    case ButtonMode::Color:
        button_mode = ButtonMode::White;
        break;
    case ButtonMode::White:
        button_mode = ButtonMode::Both;
        break;
    }

    switch_leds_by_button(true);

    LOG_DEBUG("New button mode: %d", static_cast<int>(button_mode));
}

void switch_leds_by_button(bool on)
{
    leds_start_transition(LedType::White, false);
    leds_start_transition(LedType::Color, false);

    if (on) {
        restore_color_leds(SwitchingSource::Button);
        restore_white_leds(SwitchingSource::Button);
    }
    else {
        switch_leds_off();
    }
}
