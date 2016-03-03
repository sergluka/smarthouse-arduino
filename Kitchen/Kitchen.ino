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

void on_btn_short_release()
{
    LOG_DEBUG("Button short release");

    switch_leds_by_button(!is_leds_on());
}

void on_btn_long_press()
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
    bool use_color = false;
    bool use_white = false;

    switch (button_mode) {
    case ButtonMode::Both:
        use_color = true;
        use_white = true;
        break;
    case ButtonMode::Color:
        use_color = true;
        break;
    case ButtonMode::White:
        use_white = true;
        break;
    }

    leds_start_transition(LedType::White, false);
    leds_start_transition(LedType::Color, false);

    if (on) {
        leds_fade(0, 0, 0, 0);
        if (use_color) {
            restore_color_leds();
        }
        if (use_white) {
            restore_white_leds();
        }
    }
    else {
        switch_leds_off();
    }
}
