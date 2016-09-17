#ifndef ARDUINO_TIMERONE_H
#define ARDUINO_TIMERONE_H

#include "gmock/gmock.h"
//#include "base/StaticMock.h"

class TimerOneMock// : public StaticMock<TimerOneMock>
{
public:
    MOCK_METHOD1(initialize, void(long microseconds));
    MOCK_METHOD0(start, void());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD0(restart, void());
    MOCK_METHOD0(resume, void());
    MOCK_METHOD0(read, unsigned long());
    MOCK_METHOD3(pwm, void(char pin, int duty, long microseconds));
    MOCK_METHOD1(disablePwm, void(char pin));
    MOCK_METHOD2(attachInterrupt, void(void (*isr)(), long microseconds));
    MOCK_METHOD0(detachInterrupt, void());
    MOCK_METHOD1(setPeriod, void(long microseconds));
    MOCK_METHOD2(setPwmDuty, void(char pin, int duty));
};

static TimerOneMock Timer1;

#endif //ARDUINO_TIMERONE_H
