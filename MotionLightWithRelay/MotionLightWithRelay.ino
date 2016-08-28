#include <Bounce2.h>
#include <MsTimer2.h>
#include <Logging.h>

#define MY_DEBUG
#define MY_RADIO_NRF24
#include <MySensors.h>

#define BAUD_RATE    115200U

#define CHILD_ID_LIGHT  0
#define CHILD_ID_RELAY  1
#define CHILD_ID_MOT    2

#define LIGHT_SENSOR_ANALOG_PIN     0
#define DIGITAL_INPUT_SENSOR        3  // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define INTERRUPT DIGITAL_INPUT_SENSOR-2 // Usually the interrupt = pin -2 (on uno/nano anyway)

#define BUTTON_PIN      4
#define RELAY_PIN       5

#define RELAY_ON 0
#define RELAY_OFF 1

#define TIMER_DELAY_MS  2 * 60 * 1000U

Bounce debouncer = Bounce();
int oldValue = 0;
bool state;
MyMessage msgMot(CHILD_ID_MOT, V_TRIPPED);
MyMessage msgLight(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
MyMessage msgRelay(CHILD_ID_RELAY, V_LIGHT);

// Sensors actual values
bool movement_detected = false;
int light_level;

void on_send_timer();
void on_light_level_timer();

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    LOGGING_SETUP(BAUD_RATE);
    LOG_INFO("Setup start");

    pinMode(BUTTON_PIN, INPUT);
    digitalWrite(BUTTON_PIN, HIGH);

    debouncer.attach(BUTTON_PIN);
    debouncer.interval(5);

    digitalWrite(RELAY_PIN, RELAY_OFF);
    pinMode(RELAY_PIN, OUTPUT);

    state = loadState(CHILD_ID_RELAY);
    digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);

    pinMode(DIGITAL_INPUT_SENSOR, INPUT);  // sets the motion sensor digital pin as input

    MsTimer2::set(TIMER_DELAY_MS, on_send_timer);
    MsTimer2::start();
}

void before()
{
    LOGGING_SETUP(BAUD_RATE);
    LOG_INFO("Start");
}

void presentation()
{
    sendSketchInfo("Balcony Light", "1.2");
    present(CHILD_ID_RELAY, S_LIGHT, "Relay");
    present(CHILD_ID_MOT, S_MOTION, "Motion");
    present(CHILD_ID_LIGHT, S_LIGHT_LEVEL, "LUX");
}

void loop()
{
    movement_sensor_process();
    light_level_process();

    debouncer.update();
    int value = debouncer.read();
    if (value != oldValue && value == 0) {
        send(msgRelay.set(state ? false : true), true); // Send new state and request ack back
    }
    oldValue = value;
}

void receive(const MyMessage & message)
{
    LOG_INFO("=>: sensor=%d, type=%d, is_ack=%d", message.sensor, message.type, message.isAck());

    if (message.isAck()) {
        LOG_DEBUG("This is an ack from gateway");
        return;
    }

    if (message.sensor != CHILD_ID_RELAY) {
        LOG_ERROR("Got command %d for non-relay sensor", message.type);
        return;
    }

    if (message.type == V_LIGHT) {
        state = message.getBool();
        LOG_INFO("Incoming change for the relay: %d", state);

        digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
        saveState(CHILD_ID_RELAY, state);
    }
}

void movement_sensor_process()
{
    bool new_movement_detected = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH;

    if (new_movement_detected && !movement_detected) {
        movement_detected = true;
        send(msgMot.set(movement_detected));
    }
    else if (!new_movement_detected && movement_detected) {
        movement_detected = false;
        send(msgMot.set(movement_detected));
    }
}

void light_level_process()
{
    int new_light_level = (1023 - analogRead(LIGHT_SENSOR_ANALOG_PIN)) / 10.23;
    if (new_light_level != light_level) {
        light_level = new_light_level;
        send(msgLight.set(light_level));
    }
}

void on_send_timer()
{
    LOG_INFO("Sending movement status: %d", movement_detected);
    send(msgMot.set(movement_detected));

    LOG_INFO("Sending light level: %d", light_level);

    send(msgLight.set(light_level));
}
