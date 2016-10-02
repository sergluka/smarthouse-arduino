#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../mocks/Arduino.h"
#include "../../mocks/MySensors.h"

#include "LightSwitchAC/LightSwitchAC.ino"

using namespace ::testing;

class LightSwitchACTests : public testing::Test
{
public:
    LightSwitchACTests()
    {
        reset();
    }
    ~LightSwitchACTests()
    {
        Mock::VerifyAndClearExpectations(&ArduinoMock::mock());
        Mock::VerifyAndClearExpectations(&MySensorsMock::mock());
        Mock::VerifyAndClearExpectations(&EEPROM);
    }
};

TEST_F(LightSwitchACTests, loop_no_changes_pass)
{
    loop();
}

TEST_F(LightSwitchACTests, receive_light_level_overload_pass)
{
    EXPECT_CALL(EEPROM, update(10, 0)).WillOnce(Return());
    EXPECT_CALL(EEPROM, read(10)).WillOnce(Return(0));

    MyMessage message1(MS_LAMP_ID, V_LIGHT_LEVEL);
    MyMessage message2(MS_LAMP_ID, V_LIGHT);
    EXPECT_CALL(MySensorsMock::mock(), send(message1.set(100), false));
    EXPECT_CALL(MySensorsMock::mock(), send(message2.set(1), false));

    receive(MyMessage(MS_LAMP_ID, V_LIGHT_LEVEL).set(1000));

    EXPECT_EQ(0, dimmer.limit);
    EXPECT_EQ(0, dimmer.target);
}

TEST_F(LightSwitchACTests, receive_light_level_zero_pass)
{
    EXPECT_CALL(EEPROM, update(10, DIMMER_MAX)).WillOnce(Return());
    EXPECT_CALL(EEPROM, read(10)).WillOnce(Return(DIMMER_MAX));

    MyMessage message1(MS_LAMP_ID, V_LIGHT_LEVEL);
    MyMessage message2(MS_LAMP_ID, V_LIGHT);
    EXPECT_CALL(MySensorsMock::mock(), send(message1.set(0), false));
    EXPECT_CALL(MySensorsMock::mock(), send(message2.set(1), false));

    receive(MyMessage(MS_LAMP_ID, V_LIGHT_LEVEL).set(0));

    EXPECT_EQ(DIMMER_MAX, dimmer.limit);
    EXPECT_EQ(DIMMER_MAX, dimmer.target);
}

TEST_F(LightSwitchACTests, receive_light_true_pass)
{
    MyMessage message(MS_LAMP_ID, V_LIGHT);

    EXPECT_CALL(MySensorsMock::mock(), send(message.set(0), false));
    receive(MyMessage(MS_LAMP_ID, V_LIGHT).set(0));

    EXPECT_EQ(DIMMER_HIGH, dimmer.target);
}

TEST_F(LightSwitchACTests, receive_light_false_pass)
{
    MyMessage message(MS_LAMP_ID, V_LIGHT);

    EXPECT_CALL(MySensorsMock::mock(), send(message.set(1), false));
    receive(MyMessage(MS_LAMP_ID, V_LIGHT).set(1));

    EXPECT_EQ(0, dimmer.target);
}

TEST_F(LightSwitchACTests, receive_relay1_on_pass)
{
    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(PIN_OUT_RELAY1, HIGH));
    receive(MyMessage(MS_RELAY1_ID, V_LIGHT).set(1));
}

TEST_F(LightSwitchACTests, receive_relay1_off_pass)
{
    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(PIN_OUT_RELAY1, LOW));
    receive(MyMessage(MS_RELAY1_ID, V_LIGHT).set(0));
}

TEST_F(LightSwitchACTests, receive_relay2_on_pass)
{
    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(PIN_OUT_RELAY2, HIGH));
    receive(MyMessage(MS_RELAY2_ID, V_LIGHT).set(1));
}

TEST_F(LightSwitchACTests, receive_relay2_off_pass)
{
    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(PIN_OUT_RELAY2, LOW));
    receive(MyMessage(MS_RELAY2_ID, V_LIGHT).set(0));
}

TEST_F(LightSwitchACTests, dimmer_actual_is_eq_target_pass)
{
    dimmer.actual = 1;
    dimmer.target = 1;

    dimmer_process();
    EXPECT_EQ(1, dimmer.actual);
}

TEST_F(LightSwitchACTests, dimmer_goes_up_pass)
{
    dimmer.actual = 1;
    dimmer.target = 100;
    dimmer.delay = 10;

    EXPECT_CALL(ArduinoMock::mock(), millis()).WillOnce(Return(5));
    dimmer_process();
    EXPECT_EQ(1, dimmer.actual);
    EXPECT_CALL(ArduinoMock::mock(), millis()).WillOnce(Return(10));
    dimmer_process();
    EXPECT_EQ(2, dimmer.actual);
    EXPECT_CALL(ArduinoMock::mock(), millis()).WillOnce(Return(20));
    dimmer_process();
    EXPECT_EQ(3, dimmer.actual);
}

TEST_F(LightSwitchACTests, dimmer_goes_down_pass)
{
    dimmer.actual = 20;
    dimmer.target = 5;
    dimmer.delay = 10;

    EXPECT_CALL(ArduinoMock::mock(), millis()).WillOnce(Return(5));
    dimmer_process();
    EXPECT_EQ(20, dimmer.actual);
    EXPECT_CALL(ArduinoMock::mock(), millis()).WillOnce(Return(10));
    dimmer_process();
    EXPECT_EQ(19, dimmer.actual);
    EXPECT_CALL(ArduinoMock::mock(), millis()).WillOnce(Return(20));
    dimmer_process();
    EXPECT_EQ(18, dimmer.actual);
}
