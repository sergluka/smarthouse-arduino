#ifndef ARDUINO_EEPROM_H
#define ARDUINO_EEPROM_H

#include "gmock/gmock.h"
#include <base/NiceMock.h>

using namespace testing;

template <typename T>
class EEPROMClass : public NiceMock<void>
{
public:
    EEPROMClass() :
        m_default()
    {
        ON_CALL(*this, write(_, _)).WillByDefault(Return());
        ON_CALL(*this, update(_, _)).WillByDefault(Return());
        ON_CALL(*this, put(_, _)).WillByDefault(ReturnRef(m_default));
        allow(this);
    }

    MOCK_METHOD1(read, uint8_t(int idx));
    MOCK_METHOD2(write, void(int idx, uint8_t val));
    MOCK_METHOD2(update, void(int idx, uint8_t val));

    MOCK_METHOD2_T(get, T&(int idx, T & t));
    MOCK_METHOD2_T(put, const T&(int idx, const T & t));

private:
    T m_default;
};

static EEPROMClass<int> EEPROM;

#endif //ARDUINO_EEPROM_H
