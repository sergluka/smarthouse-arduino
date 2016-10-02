#ifndef ARDUINO_LEDFADER_H
#define ARDUINO_LEDFADER_H

#include "gmock/gmock.h"
#include "base/NiceMock.h"
#include "Arduino.h"

using namespace testing;

class LEDFader : public NiceMock<void>
{
public:
    static const byte MIN_INTERVAL = 20;

    LEDFader(uint8_t pwm_pin)
    {
        ON_CALL(*this, update()).WillByDefault(Return(true));
        allow(this);
    }

    MOCK_METHOD1(set_pin, void(uint8_t pwm_pin));
    MOCK_METHOD0(get_pin, uint8_t());

    MOCK_METHOD1(set_value, void(int pwm));
    MOCK_METHOD0(get_value, uint8_t());
    MOCK_METHOD0(get_target_value, uint8_t());

    MOCK_METHOD2(fade, void(uint8_t pwm, unsigned long time));
    MOCK_METHOD0(is_fading, bool());

    MOCK_METHOD0(stop_fade, bool());
    MOCK_METHOD0(update, bool());
    MOCK_METHOD1(slower, void(int by_seconds));
    MOCK_METHOD1(faster, void(int by_seconds));
    MOCK_METHOD0(get_progress, uint8_t());
};

#endif //ARDUINO_LEDFADER_H
