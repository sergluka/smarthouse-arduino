#include <Arduino.h>

#include <stdlib.h>
#include <errno.h>

#include <MySensor.h>
#include <EEPROM.h>
#include <Button.h>
#include <TimerOne.h>
#include <Logging.h>

#define PIN_IN_ZERO_CROSS   2
#define PIN_OUT_AC          6                 // Output to Opto Triac
#define PIN_IN_BUTTON       7

#define MS_RF24_CE_PIN          4       //<-- NOTE!!! changed, the default is 9                                                                                                                                     ░
#define MS_RF24_CS_PIN          10
#define MS_RF24_PA_LEVEL        RF24_PA_MAX

#define SERIAL_SPEED            115200U

#define MS_NODE_ID              20
#define MS_SWITCH_ID            0

// This is the delay-per-brightness step in microseconds. It allows for 128 steps
// If using 60 Hz grid frequency set this to 65
#define FREQ_STEP               75

#define FADE_DELAY_MS           50

bool button_direction = false;
bool button_long_press = false;
byte target_light_level = 255;

volatile byte actual_light_level = target_light_level;
volatile int step_counter = 0;                // Variable to use as a counter of dimming steps. It is volatile since it is passed between interrupts
volatile bool zero_cross = false;  // Flag to indicate we have crossed zero

Button button(PIN_IN_BUTTON);

MyTransportNRF24 transport(MS_RF24_CE_PIN, MS_RF24_CS_PIN, MS_RF24_PA_LEVEL);
MySensor gw(transport);
MyMessage msgLedStatus(MS_SWITCH_ID, S_LIGHT);

void on_btn_short_press();
void on_btn_long_press();

void setup()
{
    Serial.begin(SERIAL_SPEED);
    LOG_DEBUG("Setup has been started.")

    LOG_DEBUG("EEPROM restore...");
    eeprom_restore();

    LOG_INFO("Button setup...");
    button.init();
    button.set_minimum_gap(20);
    button.set_maximum_gap(1000);
    button.on_press(on_btn_short_press);
    button.on_long_press(on_btn_long_press);
    button.on_long_release(on_btn_long_release);

    LOG_INFO("Interrupts setup...");
    pinMode(PIN_OUT_AC, OUTPUT);                          // Set the Triac pin as output
    attachInterrupt(digitalPinToInterrupt(PIN_IN_ZERO_CROSS), on_zero_cross_detect, RISING);
    Timer1.initialize(FREQ_STEP);                      // Initialize TimerOne library for the freq we need
    Timer1.attachInterrupt(on_timer_dim_check, FREQ_STEP);      // Go to dim_check procedure every 75 uS (50Hz)  or 65 uS (60Hz)

    LOG_INFO("MySensors setup...");
    gw.begin(on_message, MS_NODE_ID);
    gw.sendSketchInfo("LightSwitchAC", "0.1");
    gw.present(MS_SWITCH_ID, S_RGBW_LIGHT, "LEDs");

    LOG_INFO("Setup done");
}

void loop()
{
    button.init();

    gw.process();

    if (button_long_press) {
        step_target_level(button_direction);
    }
    set_actual_level();
}

void on_message(const MyMessage & message)
{
    LOG_ERROR("Got message from unexpected sensor: (sensor=%d, type=%d)", message.sensor, message.type);
}

void on_btn_short_press()
{
    LOG_DEBUG("Button: short press");
}

void on_btn_long_press()
{
    LOG_DEBUG("Button: long press");

    button_long_press = true;
}

void on_btn_long_release()
{
    LOG_DEBUG("Button: long release");

    button_long_press = false;
    button_direction = !button_direction;
    eeprom_save();
}

void on_zero_cross_detect()
{
    zero_cross = true;               // set flag for dim_check function that a zero cross has occured
    step_counter = 0;                             // stepcounter to 0.... as we start a new cycle
    digitalWrite(PIN_OUT_AC, LOW);
}

void on_timer_dim_check()
{
    if (zero_cross) {
        if (step_counter >= actual_light_level) {
            digitalWrite(PIN_OUT_AC, HIGH);  // turn on light
            step_counter = 0;  // reset time step counter
            zero_cross = false;    // reset zero cross detection flag
        }
        else {
            step_counter++;  // increment time step counter
        }
    }
}

void eeprom_restore()
{
    EEPROM.get(0, target_light_level);
    actual_light_level = target_light_level;
}

void eeprom_save()
{
    EEPROM.put(0, target_light_level);
}

void set_target_level(byte level)
{
    LOG_INFO("Fading %u => %d", actual_light_level, target_light_level);

    target_light_level = level;
}

void step_target_level(bool up)
{
    if ((up && target_light_level == 255) ||
        (!up && target_light_level == 0)) {

        return;
    }

    target_light_level += up ? 1 : -1;
    delay(FADE_DELAY_MS);
}

void set_actual_level()
{
    static unsigned long last_now = millis();

    if (actual_light_level == target_light_level) {
        return;
    }

    unsigned long now = millis();
    if (now - last_now > FADE_DELAY_MS) {
        bool direction = target_light_level < actual_light_level;

        actual_light_level = direction ? +1 : -1;
        last_now = now;

        LOG_DEBUG(">> step: %u/%u", actual_light_level, target_light_level);
    }
}
