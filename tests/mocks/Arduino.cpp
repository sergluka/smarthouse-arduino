#include "Arduino.h"

ArduinoMock::ArduinoMock()
{
    ON_CALL(mock(), pinMode(_, _)).WillByDefault(Return());
    ON_CALL(mock(), digitalWrite(_, _)).WillByDefault(Return());
    ON_CALL(mock(), digitalRead(_)).WillByDefault(Return(0));
    ON_CALL(mock(), analogRead(_)).WillByDefault(Return(0));
    ON_CALL(mock(), analogReference(_)).WillByDefault(Return());
    ON_CALL(mock(), millis()).WillByDefault(Return(0));
    ON_CALL(mock(), micros()).WillByDefault(Return(0));
    ON_CALL(mock(), analogWrite(_, _)).WillByDefault(Return());
    ON_CALL(mock(), delay(_)).WillByDefault(Return());
    ON_CALL(mock(), delayMicroseconds(_)).WillByDefault(Return());
    ON_CALL(mock(), attachInterrupt(_, _, _)).WillByDefault(Return());
    ON_CALL(mock(), detachInterrupt(_)).WillByDefault(Return());
    ON_CALL(mock(), digitalPinToInterrupt(_)).WillByDefault(Return(0));
}

void pinMode(uint8_t pin, uint8_t mode)
{
    ArduinoMock::mock().pinMode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    ArduinoMock::mock().digitalWrite(pin, value);
}

int digitalRead(uint8_t pin)
{
    return ArduinoMock::mock().digitalRead(pin);
}

int analogRead(uint8_t pin)
{
    return ArduinoMock::mock().analogRead(pin);
}

void analogReference(uint8_t mode)
{
    ArduinoMock::mock().analogReference(mode);
}

void analogWrite(uint8_t pin, int value)
{
    ArduinoMock::mock().analogWrite(pin, value);
}

unsigned long millis()
{
    return ArduinoMock::mock().millis();
}

unsigned long micros()
{
    return ArduinoMock::mock().micros();
}

void delay(unsigned long delay)
{
    return ArduinoMock::mock().delay(delay);
}

void delayMicroseconds(unsigned int delay)
{
    ArduinoMock::mock().delayMicroseconds(delay);
}

void attachInterrupt(uint8_t itr, void (*callback)(void), int mode)
{
    ArduinoMock::mock().attachInterrupt(itr, callback, mode);
}

void detachInterrupt(uint8_t itr)
{
    ArduinoMock::mock().detachInterrupt(itr);
}

uint8_t digitalPinToInterrupt(uint8_t pin)
{
    return ArduinoMock::mock().digitalPinToInterrupt(pin);
}
