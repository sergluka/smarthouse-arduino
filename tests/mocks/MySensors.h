#ifndef ARDUINO_MYSENSORS_H
#define ARDUINO_MYSENSORS_H

#include <stdint.h>
#include <base/StaticMock.h>

#include "gmock/gmock.h"

using namespace testing;

#define PROTOCOL_VERSION 2    //!< The version of the protocol
#define MAX_MESSAGE_LENGTH 32 //!< The maximum size of a message (including header)
#define HEADER_SIZE 7         //!< The size of the header
#define MAX_PAYLOAD (MAX_MESSAGE_LENGTH - HEADER_SIZE) //!< The maximum size of a payload depends on #MAX_MESSAGE_LENGTH and #HEADER_SIZE

typedef enum
{
    S_DOOR = 0,    //!< Door sensor, V_TRIPPED, V_ARMED
    S_MOTION = 1,    //!< Motion sensor, V_TRIPPED, V_ARMED
    S_SMOKE = 2,    //!< Smoke sensor, V_TRIPPED, V_ARMED
    S_BINARY = 3,    //!< Binary light or relay, V_STATUS, V_WATT
    S_LIGHT = 3,    //!< \deprecated Same as S_BINARY, **** DEPRECATED, DO NOT USE ****
    S_DIMMER = 4,    //!< Dimmable light or fan device, V_STATUS (on/off), V_PERCENTAGE (dimmer level 0-100), V_WATT
    S_COVER = 5,    //!< Blinds or window cover, V_UP, V_DOWN, V_STOP, V_PERCENTAGE (open/close to a percentage)
    S_TEMP = 6,    //!< Temperature sensor, V_TEMP
    S_HUM = 7,    //!< Humidity sensor, V_HUM
    S_BARO = 8,    //!< Barometer sensor, V_PRESSURE, V_FORECAST
    S_WIND = 9,    //!< Wind sensor, V_WIND, V_GUST
    S_RAIN = 10,    //!< Rain sensor, V_RAIN, V_RAINRATE
    S_UV = 11,    //!< Uv sensor, V_UV
    S_WEIGHT = 12,    //!< Personal scale sensor, V_WEIGHT, V_IMPEDANCE
    S_POWER = 13,    //!< Power meter, V_WATT, V_KWH, V_VAR, V_VA, V_POWER_FACTOR
    S_HEATER = 14,    //!< Header device, V_HVAC_SETPOINT_HEAT, V_HVAC_FLOW_STATE, V_TEMP
    S_DISTANCE = 15,    //!< Distance sensor, V_DISTANCE
    S_LIGHT_LEVEL = 16,    //!< Light level sensor, V_LIGHT_LEVEL (uncalibrated in percentage),  V_LEVEL (light level in lux)
    S_ARDUINO_NODE = 17,    //!< Used (internally) for presenting a non-repeating Arduino node
    S_ARDUINO_REPEATER_NODE = 18,    //!< Used (internally) for presenting a repeating Arduino node
    S_LOCK = 19,    //!< Lock device, V_LOCK_STATUS
    S_IR = 20,    //!< IR device, V_IR_SEND, V_IR_RECEIVE
    S_WATER = 21,    //!< Water meter, V_FLOW, V_VOLUME
    S_AIR_QUALITY = 22,    //!< Air quality sensor, V_LEVEL
    S_CUSTOM = 23,    //!< Custom sensor
    S_DUST = 24,    //!< Dust sensor, V_LEVEL
    S_SCENE_CONTROLLER = 25,    //!< Scene controller device, V_SCENE_ON, V_SCENE_OFF.
    S_RGB_LIGHT = 26,    //!< RGB light. Send color component data using V_RGB. Also supports V_WATT
    S_RGBW_LIGHT = 27,    //!< RGB light with an additional White component. Send data using V_RGBW. Also supports V_WATT
    S_COLOR_SENSOR = 28,    //!< Color sensor, send color information using V_RGB
    S_HVAC = 29,    //!< Thermostat/HVAC device. V_HVAC_SETPOINT_HEAT, V_HVAC_SETPOINT_COLD, V_HVAC_FLOW_STATE, V_HVAC_FLOW_MODE, V_TEMP
    S_MULTIMETER = 30,    //!< Multimeter device, V_VOLTAGE, V_CURRENT, V_IMPEDANCE
    S_SPRINKLER = 31,    //!< Sprinkler, V_STATUS (turn on/off), V_TRIPPED (if fire detecting device)
    S_WATER_LEAK = 32,    //!< Water leak sensor, V_TRIPPED, V_ARMED
    S_SOUND = 33,    //!< Sound sensor, V_TRIPPED, V_ARMED, V_LEVEL (sound level in dB)
    S_VIBRATION = 34,    //!< Vibration sensor, V_TRIPPED, V_ARMED, V_LEVEL (vibration in Hz)
    S_MOISTURE = 35,    //!< Moisture sensor, V_TRIPPED, V_ARMED, V_LEVEL (water content or moisture in percentage?)
    S_INFO = 36,    //!< LCD text device / Simple information device on controller, V_TEXT
    S_GAS = 37,    //!< Gas meter, V_FLOW, V_VOLUME
    S_GPS = 38,    //!< GPS Sensor, V_POSITION
    S_WATER_QUALITY = 39    //!< V_TEMP, V_PH, V_ORP, V_EC, V_STATUS
} mysensor_sensor;

typedef enum
{
    V_TEMP = 0,
    V_HUM = 1,
    V_STATUS = 2,
    V_LIGHT = 2,
    V_PERCENTAGE = 3,
    V_DIMMER = 3,
    V_PRESSURE = 4,
    V_FORECAST = 5,
    V_RAIN = 6,
    V_RAINRATE = 7,
    V_WIND = 8,
    V_GUST = 9,
    V_DIRECTION = 10,
    V_UV = 11,
    V_WEIGHT = 12,
    V_DISTANCE = 13,
    V_IMPEDANCE = 14,
    V_ARMED = 15,
    V_TRIPPED = 16,
    V_WATT = 17,
    V_KWH = 18,
    V_SCENE_ON = 19,
    V_SCENE_OFF = 20,
    V_HVAC_FLOW_STATE = 21,
    V_HEATER = 21,
    V_HVAC_SPEED = 22,
    V_LIGHT_LEVEL = 23,
    V_VAR1 = 24,
    V_VAR2 = 25,
    V_VAR3 = 26,
    V_VAR4 = 27,
    V_VAR5 = 28,
    V_UP = 29,
    V_DOWN = 30,
    V_STOP = 31,
    V_IR_SEND = 32,
    V_IR_RECEIVE = 33,
    V_FLOW = 34,
    V_VOLUME = 35,
    V_LOCK_STATUS = 36,
    V_LEVEL = 37,
    V_VOLTAGE = 38,
    V_CURRENT = 39,
    V_RGB = 40,
    V_RGBW = 41,
    V_ID = 42,
    V_UNIT_PREFIX = 43,
    V_HVAC_SETPOINT_COOL = 44,
    V_HVAC_SETPOINT_HEAT = 45,
    V_HVAC_FLOW_MODE = 46,
    V_TEXT = 47,
    V_CUSTOM = 48,
    V_POSITION = 49,
    V_IR_RECORD = 50,
    V_PH = 51,
    V_ORP = 52,
    V_EC = 53,
    V_VAR = 54,
    V_VA = 55,
    V_POWER_FACTOR = 56,
} mysensor_data;

class MyMessage
{
public:
    MyMessage();
    MyMessage(uint8_t sensor, uint8_t type);

    bool operator==(const MyMessage & rhs) const;
    bool operator!=(const MyMessage & rhs) const;

    char* getStream(char *buffer) const;
    char* getString(char *buffer) const;
    const char* getString() const;
    void* getCustom() const;
    bool getBool() const;
    uint8_t getByte() const;
    float getFloat() const;
    int16_t getInt() const;
    uint16_t getUInt() const;
    int32_t getLong() const;
    uint32_t getULong() const;

    void setIsAck(bool is_ack);
    bool isAck() const;

    MyMessage& setType(uint8_t type);
    MyMessage& setSensor(uint8_t sensor);
    MyMessage& setDestination(uint8_t destination);

    MyMessage& set(void* payload, uint8_t length);
    MyMessage& set(const char* value);
    MyMessage& set(float value, uint8_t decimals);
    MyMessage& set(bool value);
    MyMessage& set(uint8_t value);
    MyMessage& set(uint32_t value);
    MyMessage& set(int32_t value);
    MyMessage& set(uint16_t value);
    MyMessage& set(int16_t value);

    uint8_t last;
    uint8_t sender;
    uint8_t destination;

    uint8_t sensor;
    uint8_t type;

private:
    union {
        uint8_t bValue;
        uint16_t uiValue;
        int16_t iValue;
        uint32_t ulValue;
        int32_t lValue;
        struct { // Float messages
            float fValue;
            uint8_t fPrecision;   // Number of decimals when serializing
        };
        struct {  // Presentation messages
            uint8_t version; 	  // Library version
            uint8_t sensorType;   // Sensor type hint for controller, see table above
        };
        char data[MAX_PAYLOAD + 1];
    } m_data;
    bool m_is_ack;
};

::std::ostream & operator<<(::std::ostream & os, const MyMessage & mock);

class MySensorsMock : public NiceMock<StaticMock<MySensorsMock>>
{
public:
    MySensorsMock();

    MOCK_METHOD1(loadState, uint8_t(uint8_t pos));
    MOCK_METHOD2(saveState, void(uint8_t pos, uint8_t value));
    MOCK_METHOD4(present, void(uint8_t sensorId, uint8_t sensorType, const char * description, bool ack));
    MOCK_METHOD3(sendSketchInfo, void(const char * name, const char * version, bool ack));
    MOCK_METHOD2(send, bool(MyMessage & msg, bool ack));
};

uint8_t loadState(uint8_t pos);
void saveState(uint8_t pos, uint8_t value);
void present(uint8_t sensorId, uint8_t sensorType, const char * description = "", bool ack = false);
void sendSketchInfo(const char * name, const char * version, bool ack = false);
bool send(MyMessage & msg, bool ack = false);

#endif //ARDUINO_MYSENSORS_H
