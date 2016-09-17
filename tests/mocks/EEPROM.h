#ifndef ARDUINO_EEPROM_H
#define ARDUINO_EEPROM_H

#include "gmock/gmock.h"
//#include "base/StaticMock.h"

class EEPROMClassMock// : public StaticMock<EEPROMClassMock>
{
public:
    MOCK_METHOD1(read, uint8_t(int idx));
    MOCK_METHOD2(write, void(int idx, uint8_t val));
    MOCK_METHOD2(update, void(int idx, uint8_t val));
};

static EEPROMClassMock EEPROM;

////Functionality to 'get' and 'put' objects to and from EEPROM.
//template< typename T > T &get( int idx, T &t ){
//    EEPtr e = idx;
//    uint8_t *ptr = (uint8_t*) &t;
//    for( int count = sizeof(T) ; count ; --count, ++e )  *ptr++ = *e;
//    return t;
//}
//
//template< typename T > const T &put( int idx, const T &t ){
//    EEPtr e = idx;
//    const uint8_t *ptr = (const uint8_t*) &t;
//    for( int count = sizeof(T) ; count ; --count, ++e )  (*e).update( *ptr++ );
//    return t;
//}
//
//EEPROM.read

#endif //ARDUINO_EEPROM_H
