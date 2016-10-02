#ifndef ARDUINO_ARDUINO_H
#define ARDUINO_ARDUINO_H

#include "gmock/gmock.h"
#include "base/StaticMock.h"

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define CHANGE 1                                                                                                                                                                                                    ▓
#define FALLING 2                                                                                                                                                                                                   ▓
#define RISING 3

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

typedef uint8_t byte;

using namespace testing;

class ArduinoMock : public NiceMock<StaticMock<ArduinoMock>>
{
public:
    ArduinoMock();

    MOCK_METHOD2(pinMode, void(uint8_t pin, uint8_t mode));
    MOCK_METHOD2(digitalWrite, void(uint8_t pin, uint8_t value));
    MOCK_METHOD1(digitalRead, int(uint8_t pin));
    MOCK_METHOD1(analogRead, int(uint8_t pin));
    MOCK_METHOD1(analogReference, void(uint8_t mode));
    MOCK_METHOD0(millis, unsigned long());
    MOCK_METHOD0(micros, unsigned long());
    MOCK_METHOD2(analogWrite, void(uint8_t pin, int value));
    MOCK_METHOD1(delay, void(unsigned long delay));
    MOCK_METHOD1(delayMicroseconds, void(unsigned int delay));
    MOCK_METHOD3(attachInterrupt, void(uint8_t itr, void (*callback)(void), int mode));
    MOCK_METHOD1(detachInterrupt, void(uint8_t itr));
    MOCK_METHOD1(digitalPinToInterrupt, uint8_t(uint8_t pin));
};

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogReference(uint8_t mode);
void analogWrite(uint8_t, int);
unsigned long millis();
unsigned long micros();
void delay(unsigned long delay);
void delayMicroseconds(unsigned int delay);
void attachInterrupt(uint8_t, void (*)(void), int mode);
void detachInterrupt(uint8_t);
uint8_t digitalPinToInterrupt(uint8_t pin);

#endif //ARDUINO_ARDUINO_H
