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

template <typename Mock>
class DynamicMock
{
public:
    DynamicMock() //:
//        m_mock{new testing::NiceMock<Mock>()}
    {
//        ExposedMockClass* mockObject = new testing::NiceMock<ExposedMockClass>();
    }

    testing::NiceMock<Mock> & mock()
    {
        Mock * child = static_cast<Mock*>(this);
        auto mock = static_cast<testing::NiceMock<Mock>*>(child);

//        return *m_mock;
        return *mock;
//        return m_mock;
    }
//    testing::NiceMock<Mock> & nice_mock()
//    {
////        return *m_mock;
//        return dynamic_cast<testing::NiceMock<Mock>>(this);
////        return m_mock;
//    }

private:
    testing::NiceMock<Mock> * m_mock;
};

#endif //ARDUINO_STATICMOCK_H
