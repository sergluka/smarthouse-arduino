#include <Arduino.h>

#include <MySensor.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include <Logging.h>

#define PIN_IN_ZERO_CROSS   2
#define PIN_OUT_AC          6                 // Output to Opto Triac
#define PIN_IN_BUTTON       5

#define SERIAL_SPEED            115200U

#define MS_NODE_ID              20
#define MS_LAMP_ID              0

// This is the delay-per-brightness step in microseconds. It allows for 128 steps
// If using 60 Hz grid frequency set this to 65
#define FREQ_STEP               75

// Delay for transition at button long press
#define FADE_DELAY_SLOW_MS           100
// Delay for transition between on and off
#define FADE_DELAY_FAST_MS           20

#define BUTTON_SHORT_PRESS_TIME 100
#define BUTTON_LONG_PRESS_TIME 2000

bool button_direction = false;
bool button_long_press = false;

struct {
    volatile byte actual;
    byte max;
    byte target;
    unsigned int delay;
} dimmer = {255, 255, 255, FADE_DELAY_SLOW_MS};

volatile int step_counter = 0;                // Variable to use as a counter of dimming steps. It is volatile since it is passed between interrupts
volatile bool zero_cross = false;  // Flag to indicate we have crossed zero

MyTransportNRF24 transport;
MySensor gw(transport);
MyMessage msgLedStatus(MS_LAMP_ID, S_LIGHT);

enum class ButtonPressStatus
{
    NONE,
    SHORT,
    LONG,
};

void setup()
{
    Serial.begin(SERIAL_SPEED);
    LOG_DEBUG("Setup has been started.")

    LOG_DEBUG("EEPROM restore...");
    eeprom_restore();

    LOG_INFO("Pins setup...");

    pinMode(PIN_IN_BUTTON, INPUT);
    pinMode(PIN_OUT_AC, OUTPUT);                          // Set the Triac pin as output

    LOG_INFO("Interrupts setup...");
    attachInterrupt(digitalPinToInterrupt(PIN_IN_ZERO_CROSS), on_zero_cross_detect, RISING);
    Timer1.initialize(FREQ_STEP);                      // Initialize TimerOne library for the freq we need
    Timer1.attachInterrupt(on_timer_dim_check, FREQ_STEP);      // Go to dim_check procedure every 75 uS (50Hz)  or 65 uS (60Hz)

    LOG_INFO("MySensors setup...");
    gw.begin(on_message, MS_NODE_ID);
    gw.sendSketchInfo("LightSwitchAC", "0.1");
    gw.present(MS_LAMP_ID, S_DIMMER, "Lamp");

    LOG_INFO("Setup done");
}

void loop()
{
    gw.process();

    if (button_long_press) {
        dimmer_step(button_direction);
    }
    dimmer_process();
    button_process();
}

void button_process()
{
    static bool prev_status = false;
    static unsigned long pressed_time = 0;
    static ButtonPressStatus prev_press_status = ButtonPressStatus::NONE;
    static ButtonPressStatus press_status = ButtonPressStatus::NONE;

    bool status = digitalRead(PIN_IN_BUTTON) == HIGH;
    // on press
    if (status && !prev_status) {
        pressed_time = millis();
    }
    // on release
    else if (!status && prev_status) {
        on_btn_release(press_status);
        pressed_time = 0;
        press_status = ButtonPressStatus::NONE;
    }

    if (pressed_time > 0) {
        unsigned long hold_time = millis() - pressed_time;
        if (hold_time >= BUTTON_LONG_PRESS_TIME) {
            press_status = ButtonPressStatus::LONG;
        }
        else if (hold_time >= BUTTON_SHORT_PRESS_TIME) {
            press_status = ButtonPressStatus::SHORT;
        }
    }

    if (prev_press_status != press_status) {
        on_btn_press(press_status);
    }

    prev_status = status;
    prev_press_status = press_status;

}

bool calc_button_direction()
{
    if (dimmer.actual == 0) {
        return true;
    }
    else if (dimmer.actual == 255) {
        return false;
    }
    else {
        return !button_direction;
    }
}

void on_btn_press(ButtonPressStatus status)
{
    if (status != ButtonPressStatus::LONG) {
        return;
    }

    button_direction = calc_button_direction();

    if (button_direction) {
        dimmer.target = 255;
    }
    else {
        dimmer.target = 0;
    }
    dimmer.delay = FADE_DELAY_SLOW_MS;
}

void on_btn_release(ButtonPressStatus status)
{
    if (status == ButtonPressStatus::SHORT) {
        LOG_DEBUG("Button: short release");
        dimmer_switch(dimmer.actual == 0, FADE_DELAY_FAST_MS);
    }
    else if (status == ButtonPressStatus::LONG) {
        LOG_DEBUG("Button: long release. Stopped at %d", dimmer.actual);

        dimmer.target = dimmer.actual;
        dimmer.max = dimmer.actual;

        eeprom_save();
    }
}

void on_zero_cross_detect()
{
    zero_cross = true;               // set flag for dim_check function that a zero cross has occured
    step_counter = 0;                             // stepcounter to 0.... as we start a new cycle
    digitalWrite(PIN_OUT_AC, LOW);
}

void on_timer_dim_check()
{
    if (zero_cross && dimmer.actual > 0) {
        if (step_counter >= dimmer.actual) {
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
    EEPROM.get(0, dimmer.max);
    dimmer.actual = 0;
    dimmer.target = 0;
}

void eeprom_save()
{
    EEPROM.put(0, dimmer.max);
}

void dimmer_switch(bool on_off, unsigned int delay)
{
    if (on_off) {
        LOG_DEBUG("Switch light to %d", dimmer.max);
        dimmer.target = dimmer.max;
        send(msgLedStatus.set(1));
    }
    else {
        LOG_DEBUG("Switch light off");
        dimmer.target = 0;
        send(msgLedStatus.set(0));
    }
    dimmer.delay = delay;
}

void dimmer_step(bool up)
{
    if ((up && dimmer.target == 255) ||
        (!up && dimmer.target == 0)) {

        return;
    }

    dimmer.target += up ? 1 : -1;
    dimmer.max = dimmer.target;
    delay(dimmer.delay);
}

// Going from actual to target level with delay
void dimmer_process()
{
    static unsigned long last_now = millis();

    bool direction = dimmer.target > dimmer.actual;
    if (direction && (dimmer.actual >= dimmer.target)) {
        return;
    }
    else if (!direction && (dimmer.actual == 0)) {
        return;
    }
    else if (dimmer.actual == dimmer.target) {
        return;
    }

    unsigned long now = millis();
    if (now - last_now > dimmer.delay) {
        dimmer.actual += (direction ? +1 : -1);
        last_now = now;

        LOG_DEBUG("step: %u/%u", dimmer.actual, dimmer.target);
    }
}

void send(MyMessage & message)
{
    if (!gw.send(message)) {
        LOG_ERROR("Message (sensor=%d, type=%d) doesn't reach a next node", message.sensor, message.type);
    }
}

void on_message(const MyMessage & message)
{
    if (message.sensor != MS_LAMP_ID) {
        LOG_ERROR("Got message from unexpected sensor: (sensor=%d, type=%d)", message.sensor, message.type);
        return;
    }

    if (message.type == V_DIMMER) {
        on_message_set_dimmer(message);
    }
    else if (message.type == V_STATUS) {
        on_message_set_status(message);
    }
    else if (message.type == V_VAR1) {
        on_message_dump_data(message);
    }
    else {
        LOG_ERROR("Got message with unexpected type: (sensor=%d, type=%d)", message.sensor, message.type);
    }
}

void on_message_set_dimmer(const MyMessage & message)
{
    int dimmer_level = constrain(message.getInt(), 0, 255);
    LOG_DEBUG("=> Message: sensor=%d, type=DIMMER, value=%d", message.sensor, dimmer_level);

    dimmer.max = dimmer_level;
    dimmer.target = dimmer_level;
    eeprom_save();
}

void on_message_set_status(const MyMessage & message)
{
    bool status = message.getBool();
    LOG_DEBUG("=> Message: sensor=%d, type=STATUS, value=%d", message.sensor, status);

    dimmer_switch(status, FADE_DELAY_FAST_MS);
}

void on_message_dump_data(const MyMessage & message)
{
    LOG_DEBUG("=> Message: sensor=%d, type=VAR1", message.sensor);

    LOG_DEBUG("Actual: %d", dimmer.actual);
    LOG_DEBUG("Target: %d", dimmer.target);
    LOG_DEBUG("Max: %d", dimmer.max);
    LOG_DEBUG("Delay: %ums", dimmer.delay);
    LOG_DEBUG("Button direction: %s", button_direction ? "up" : "down");
    LOG_DEBUG("Button is long press: %d", button_long_press);
}
