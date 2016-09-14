#ifndef ARDUINO_MSTIMER2_H
#define ARDUINO_MSTIMER2_H

#include "gmock/gmock.h"
#include "base/StaticMock.h"

using namespace testing;

class MsTimerMock;

class MsTimerMock : public StaticMock<MsTimerMock>
{
public:
    MOCK_METHOD2(set, void(unsigned long ms, void (*f)()));
    MOCK_METHOD0(start, void());
    MOCK_METHOD0(stop, void());
};

namespace MsTimer2 {

void set(unsigned long ms, void (*f)());
void start();
void stop();

}

#endif //ARDUINO_MSTIMER2_H
