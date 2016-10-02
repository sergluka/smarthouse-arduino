#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <LEDFader.h>
#include <Kitchen/Types.h>
#include <Kitchen/Parsing.h>
#include <Kitchen/LEDs.h>

#include "../../mocks/Arduino.h"
#include "../../mocks/MySensors.h"

#include "Kitchen/Networking.h"
#include "Kitchen/Storage.h"

using namespace ::testing;

void on_message_set_limit(uint8_t sensor, RGBW rgbw, SwitchingSource source);
void on_message_set_transition(uint8_t sensor, const Transition & transition);
void on_message_light_off(uint8_t sensor);
void on_message_light_on(uint8_t sensor);

extern LEDFader ledW;
extern LEDFader ledR;
extern LEDFader ledG;
extern LEDFader ledB;
extern EEPROMClass<Storage> eeprom;

class NetworkingTests : public testing::Test
{
public:
    NetworkingTests() :
        msgColorLedStatus{0, V_LIGHT},
        msgWhiteLedStatus{1, V_LIGHT}
    {
        leds_reset();
        storage_reset();
    }

    ~NetworkingTests()
    {
        Mock::VerifyAndClearExpectations(&ArduinoMock::mock());
        Mock::VerifyAndClearExpectations(&MySensorsMock::mock());
        Mock::VerifyAndClearExpectations(&eeprom);
        Mock::VerifyAndClearExpectations(&ledR);
        Mock::VerifyAndClearExpectations(&ledG);
        Mock::VerifyAndClearExpectations(&ledB);
        Mock::VerifyAndClearExpectations(&ledW);
    }

    MyMessage msgColorLedStatus;
    MyMessage msgWhiteLedStatus;
};

TEST_F(NetworkingTests, on_message_set_limit_for_external_and_color)
{
    EXPECT_CALL(eeprom, put(0, _)).WillOnce(Invoke([](int idx, const Storage & storage) -> const Storage& {
        EXPECT_EQ(0x10, storage.colors[SwitchingSource::External].R);
        EXPECT_EQ(0x20, storage.colors[SwitchingSource::External].G);
        EXPECT_EQ(0x30, storage.colors[SwitchingSource::External].B);
        EXPECT_EQ(0x0,  storage.colors[SwitchingSource::External].W);
        return storage;
    }));

    EXPECT_CALL(ledR, fade(0x10, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x20, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x30, _)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(_, _)).Times(0);
    EXPECT_CALL(MySensorsMock::mock(), send(msgColorLedStatus.set(1), false)).WillOnce(Return(true));

    on_message_set_limit(0, RGBW{0x10, 0x20, 0x30, 0x40}, SwitchingSource::External);
}

TEST_F(NetworkingTests, on_message_set_limit_for_button_and_white)
{
    EXPECT_CALL(eeprom, put(0, _)).WillOnce(Invoke([](int idx, const Storage & storage) -> const Storage& {
        EXPECT_EQ(0x0,  storage.colors[SwitchingSource::Button].R);
        EXPECT_EQ(0x0,  storage.colors[SwitchingSource::Button].G);
        EXPECT_EQ(0x0,  storage.colors[SwitchingSource::Button].B);
        EXPECT_EQ(0x40,  storage.colors[SwitchingSource::Button].W);
        return storage;
    }));

    EXPECT_CALL(ledR, fade(_, _)).Times(0);
    EXPECT_CALL(ledG, fade(_, _)).Times(0);
    EXPECT_CALL(ledB, fade(_, _)).Times(0);
    EXPECT_CALL(ledW, fade(0x40, _)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteLedStatus.set(1), false)).WillOnce(Return(true));

    on_message_set_limit(1, RGBW{0x10, 0x20, 0x30, 0x40}, SwitchingSource::Button);
}

TEST_F(NetworkingTests, on_message_set_transition_color)
{
    EXPECT_CALL(ledR, set_value(0x10)).WillOnce(Return());
    EXPECT_CALL(ledG, set_value(0x20)).WillOnce(Return());
    EXPECT_CALL(ledB, set_value(0x30)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0x40, 101)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x50, 101)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x60, 101)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgColorLedStatus.set(1), false)).WillOnce(Return(true));

    on_message_set_transition(0, Transition{{0x10, 0x20, 0x30}, {0x40, 0x50, 0x60}, 101, false});

    EXPECT_TRUE(leds_is_transition(LedType::Color));
    EXPECT_FALSE(leds_is_transition(LedType::White));
}

TEST_F(NetworkingTests, on_message_set_transition_white)
{
    EXPECT_CALL(ledW, set_value(0x10)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0x60, 101)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteLedStatus.set(1), false)).WillOnce(Return(true));

    on_message_set_transition(1, Transition{{0, 0, 0, 0x10}, {0, 0, 0, 0x60}, 101, false});

    EXPECT_FALSE(leds_is_transition(LedType::Color));
    EXPECT_TRUE(leds_is_transition(LedType::White));
}

TEST_F(NetworkingTests, on_message_light_off_color)
{
    EXPECT_CALL(ledR, fade(0, 3000)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0, 3000)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0, 3000)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgColorLedStatus.set(0), false)).WillOnce(Return(true));

    on_message_light_off(0);
}

TEST_F(NetworkingTests, on_message_light_off_white)
{
    EXPECT_CALL(ledW, fade(0, 3000)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteLedStatus.set(0), false)).WillOnce(Return(true));

    on_message_light_off(1);
}

TEST_F(NetworkingTests, on_message_light_on_color)
{
    save_color_leds(SwitchingSource::External, 0x10, 0x20, 0x30);

    EXPECT_CALL(ledR, fade(0x10, 3000)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x20, 3000)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x30, 3000)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgColorLedStatus.set(1), false)).WillOnce(Return(true));

    on_message_light_on(0);
}

TEST_F(NetworkingTests, on_message_light_on_white)
{
    save_white_leds(SwitchingSource::External, 0x40);

    EXPECT_CALL(ledW, fade(0x40, 3000)).WillOnce(Return());
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteLedStatus.set(1), false)).WillOnce(Return(true));

    on_message_light_on(1);
}
