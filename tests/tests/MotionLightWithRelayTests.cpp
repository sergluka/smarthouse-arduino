#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <../mocks/Arduino.h>
#include <../mocks/MySensors.h>

#include "MotionLightWithRelay/MotionLightWithRelay.ino"

using namespace testing;

class MotionLightWithRelayTests : public testing::Test
{
public:
    MotionLightWithRelayTests()
    {
        ON_CALL(ArduinoMock::mock(), digitalRead(3)).WillByDefault(Return(LOW));
        ON_CALL(ArduinoMock::mock(), analogRead(0)).WillByDefault(Return(1023));
        ON_CALL(debouncer, update()).WillByDefault(Return());
        ON_CALL(debouncer, read()).WillByDefault(Return(false));

        reset();
    }
    ~MotionLightWithRelayTests()
    {
        Mock::VerifyAndClearExpectations(&ArduinoMock::mock());
        Mock::VerifyAndClearExpectations(&MySensorsMock::mock());
    }
};

TEST_F(MotionLightWithRelayTests, loop_no_changes_pass)
{
    loop();
}

TEST_F(MotionLightWithRelayTests, loop_on_movement_change_send_messages)
{
    EXPECT_CALL(ArduinoMock::mock(), digitalRead(3)).WillOnce(Return(LOW));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(ArduinoMock::mock(), digitalRead(3)).WillOnce(Return(HIGH));
    EXPECT_CALL(MySensorsMock::mock(), send(msgMot, false)).WillOnce(Return(true));
    loop();
    EXPECT_TRUE(msgMot.getBool());

    EXPECT_CALL(ArduinoMock::mock(), digitalRead(3)).WillOnce(Return(HIGH));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(ArduinoMock::mock(), digitalRead(3)).WillOnce(Return(LOW));
    EXPECT_CALL(MySensorsMock::mock(), send(msgMot, false)).WillOnce(Return(true));
    loop();
    EXPECT_FALSE(msgMot.getBool());

    EXPECT_CALL(ArduinoMock::mock(), digitalRead(3)).WillOnce(Return(LOW));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(ArduinoMock::mock(), digitalRead(3)).WillOnce(Return(HIGH));
    EXPECT_CALL(MySensorsMock::mock(), send(msgMot, false)).WillOnce(Return(true));
    loop();
    EXPECT_TRUE(msgMot.getBool());
}

TEST_F(MotionLightWithRelayTests, loop_on_light_change_send_messages)
{
    EXPECT_CALL(ArduinoMock::mock(), analogRead(0)).WillOnce(Return(1023));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(ArduinoMock::mock(), analogRead(0)).WillOnce(Return(1010));
    EXPECT_CALL(MySensorsMock::mock(), send(msgLight, false)).WillOnce(Return(true));
    loop();
    EXPECT_TRUE(msgLight.getBool());

    EXPECT_CALL(ArduinoMock::mock(), analogRead(0)).WillOnce(Return(1009));
    EXPECT_CALL(MySensorsMock::mock(), send(msgLight, false)).WillOnce(Return(true));
    loop();
    EXPECT_TRUE(msgLight.getBool());

    EXPECT_CALL(ArduinoMock::mock(), analogRead(0)).WillOnce(Return(1021));
    EXPECT_CALL(MySensorsMock::mock(), send(msgMot, false)).Times(0);
    loop();

    EXPECT_CALL(ArduinoMock::mock(), analogRead(0)).WillOnce(Return(1010));
    EXPECT_CALL(MySensorsMock::mock(), send(msgLight, false)).WillOnce(Return(true));
    loop();
    EXPECT_TRUE(msgLight.getBool());
}

TEST_F(MotionLightWithRelayTests, loop_on_button_press_send_messages)
{
    EXPECT_CALL(debouncer, update()).WillOnce(Return());
    EXPECT_CALL(debouncer, read()).WillOnce(Return(false));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(debouncer, update()).WillOnce(Return());
    EXPECT_CALL(debouncer, read()).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(debouncer, update()).WillOnce(Return());
    EXPECT_CALL(debouncer, read()).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();

    EXPECT_CALL(debouncer, update()).WillOnce(Return());
    EXPECT_CALL(debouncer, read()).WillOnce(Return(false));
    EXPECT_CALL(MySensorsMock::mock(), send(msgRelay, true)).WillOnce(Return(true));
    loop();
    EXPECT_TRUE(msgRelay.getBool());

    EXPECT_CALL(debouncer, update()).WillOnce(Return());
    EXPECT_CALL(debouncer, read()).WillOnce(Return(false));
    EXPECT_CALL(MySensorsMock::mock(), send(_, _)).Times(0);
    loop();
}

TEST_F(MotionLightWithRelayTests, receive_ack_pass)
{
    MyMessage message;
    message.setIsAck(true);

    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(_, _)).Times(0);
    EXPECT_CALL(MySensorsMock::mock(), saveState(_, _)).Times(0);;

    receive(message);
}

TEST_F(MotionLightWithRelayTests, receive_message_from_unknown_sensor_fail)
{
    MyMessage message{100, V_VAR5};

    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(_, _)).Times(0);
    EXPECT_CALL(MySensorsMock::mock(), saveState(_, _)).Times(0);

    receive(message);
}

TEST_F(MotionLightWithRelayTests, receive_message_switch_on_relay_pass)
{
    MyMessage message{CHILD_ID_RELAY, V_LIGHT};
    message.set(true);

    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(RELAY_PIN, 0)).Times(1);
    EXPECT_CALL(MySensorsMock::mock(), saveState(CHILD_ID_RELAY, true)).Times(1);

    receive(message);
}

TEST_F(MotionLightWithRelayTests, receive_message_switch_off_relay_pass)
{
    MyMessage message{CHILD_ID_RELAY, V_LIGHT};
    message.set(false);

    EXPECT_CALL(ArduinoMock::mock(), digitalWrite(RELAY_PIN, 1)).Times(1);
    EXPECT_CALL(MySensorsMock::mock(), saveState(CHILD_ID_RELAY, false)).Times(1);

    receive(message);
}
