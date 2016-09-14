#ifndef MOCK_BOUNCE_H_
#define MOCK_BOUNCE_H_

#include <stdint.h>

#include <gmock/gmock.h>
#include <base/StaticMock.h>
#include <base/NiceMock.h>

using namespace testing;

class Bounce : public NiceMock<void>
{
public:
    Bounce();

    MOCK_METHOD1(attach, void(int pin));
    MOCK_METHOD2(attach, void(int pin, int mode));
    MOCK_METHOD1(interval, void(uint16_t interval_millis));
    MOCK_METHOD0(update, void());
    MOCK_METHOD0(read, bool());
};

#endif //MOCK_BOUNCE_H_
