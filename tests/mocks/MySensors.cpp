#include "MySensors.h"

MyMessage::MyMessage() :
    sensor{0},
    type{0},
    m_data{},
    m_is_ack{false}
{
}

MyMessage::MyMessage(uint8_t sensor_, uint8_t type_) :
    sensor{sensor_},
    type{type_},
    m_data{},
    m_is_ack{false}
{
}

bool MyMessage::operator==(const MyMessage & rhs) const
{
    return type == rhs.type && sensor == rhs.sensor &&
        memcmp(m_data.data, rhs.m_data.data, sizeof(m_data.data)) == 0 && m_is_ack == rhs.m_is_ack;
}

bool MyMessage::operator!=(const MyMessage & rhs) const
{
    return !(rhs == *this);
}

char * MyMessage::getStream(char * buffer) const
{
    throw std::runtime_error("Not implemented");
    return nullptr;
}

char * MyMessage::getString(char * buffer) const
{
    return buffer;
}

const char * MyMessage::getString() const
{
    return m_data.data;
}

void * MyMessage::getCustom() const
{
    return (void *)m_data.data;
}

bool MyMessage::getBool() const
{
    return m_data.bValue != 0;
}

uint8_t MyMessage::getByte() const
{
    return m_data.bValue;
}

float MyMessage::getFloat() const
{
    return m_data.fValue;
}

int16_t MyMessage::getInt() const
{
    return m_data.iValue;
}

uint16_t MyMessage::getUInt() const
{
    return m_data.uiValue;
}

int32_t MyMessage::getLong() const
{
    return m_data.lValue;
}

uint32_t MyMessage::getULong() const
{
    return m_data.ulValue;
}

void MyMessage::setIsAck(bool is_ack)
{
    m_is_ack = is_ack;
}

bool MyMessage::isAck() const
{
    return m_is_ack;
}

MyMessage & MyMessage::set(void * payload, uint8_t length)
{
    if (length > sizeof(m_data)) {
        throw std::runtime_error("Too big payload");
    }

    memcpy(m_data.data, payload, length);
    return *this;
}

MyMessage & MyMessage::set(const char * value)
{
    if (strlen(value) > sizeof(m_data)) {
        throw std::runtime_error("Too big payload");
    }

    strncpy(m_data.data, value, sizeof(m_data));
    return *this;
}

MyMessage & MyMessage::set(float value, uint8_t decimals)
{
    m_data.fValue = value;
    m_data.fPrecision = decimals;
    return *this;
}

MyMessage & MyMessage::set(bool value)
{
    m_data.bValue = value;
    return *this;
}

MyMessage & MyMessage::set(uint8_t value)
{
    m_data.bValue = value;
    return *this;
}

MyMessage & MyMessage::set(uint32_t value)
{
    m_data.ulValue = value;
    return *this;
}

MyMessage & MyMessage::set(int32_t value)
{
    m_data.lValue = value;
    return *this;
}

MyMessage & MyMessage::set(uint16_t value)
{
    m_data.uiValue = value;
    return *this;
}

MyMessage & MyMessage::set(int16_t value)
{
    m_data.iValue = value;
    return *this;
}

MyMessage & MyMessage::setType(uint8_t type)
{
    this->type = type;
    return *this;
}

MyMessage & MyMessage::setSensor(uint8_t sensor)
{
    this->sensor = sensor;
    return *this;
}

::std::ostream & operator<<(::std::ostream & os, const MyMessage & mock)
{
    return os << "MyMessage: [sensor=" << (int)mock.sensor <<
            ", type=" << (int)mock.type <<
            ", is_ack=" << mock.isAck() <<
            ", bool=" << mock.getBool() <<
            ", int=" << mock.getInt() <<
            ", str=" << mock.getString() << "]";
}

uint8_t loadState(uint8_t pos)
{
    return MySensorsMock::mock().loadState(pos);
}

void saveState(uint8_t pos, uint8_t value)
{
    MySensorsMock::mock().saveState(pos, value);
}

void present(uint8_t sensorId, uint8_t sensorType, const char * description, bool ack)
{
    MySensorsMock::mock().present(sensorId, sensorType, description, ack);
}

void sendSketchInfo(const char * name, const char * version, bool ack)
{
    MySensorsMock::mock().sendSketchInfo(name, version, ack);
}

bool send(MyMessage & msg, bool ack)
{
    return MySensorsMock::mock().send(msg, ack);
}

MySensorsMock::MySensorsMock()
{
    ON_CALL(mock(), loadState(_)).WillByDefault(Return(0));
    ON_CALL(mock(), saveState(_, _)).WillByDefault(Return());
    ON_CALL(mock(), present(_, _, _, _)).WillByDefault(Return());
    ON_CALL(mock(), sendSketchInfo(_, _, _)).WillByDefault(Return());
}
