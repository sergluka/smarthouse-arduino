#ifndef ARDUINO_STATICMOCK_H
#define ARDUINO_STATICMOCK_H

#include "gmock/gmock.h"

template <typename Mock>
class StaticMock
{
public:
    static Mock & mock()
    {
        return m_mock;
    }

private:
    static Mock m_mock;
};

template <typename Mock>
Mock StaticMock<Mock>::m_mock;

#endif //ARDUINO_STATICMOCK_H
