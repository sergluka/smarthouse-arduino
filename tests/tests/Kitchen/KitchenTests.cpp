#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <LEDFader.h>

#include "../../mocks/Arduino.h"
#include "../../mocks/MySensors.h"

#include "Kitchen/Kitchen.ino"

extern LEDFader ledW;
extern LEDFader ledR;
extern LEDFader ledG;
extern LEDFader ledB;
extern EEPROMClass<Storage> eeprom;

using namespace ::testing;

class KitchenTests : public testing::Test
{
public:
    KitchenTests() :
        msgColorStatus{0, V_LIGHT},
        msgWhiteStatus{1, V_LIGHT}
    {
        reset();

        save_color_leds(SwitchingSource::Button, 0x10, 0x20, 0x30);
        save_color_leds(SwitchingSource::External, 0x11, 0x21, 0x31);
        save_white_leds(SwitchingSource::Button, 0x40);
        save_white_leds(SwitchingSource::External, 0x41);
    }

    ~KitchenTests()
    {
        Mock::VerifyAndClearExpectations(&ArduinoMock::mock());
        Mock::VerifyAndClearExpectations(&MySensorsMock::mock());
        Mock::VerifyAndClearExpectations(&eeprom);
        Mock::VerifyAndClearExpectations(&ledR);
        Mock::VerifyAndClearExpectations(&ledG);
        Mock::VerifyAndClearExpectations(&ledB);
        Mock::VerifyAndClearExpectations(&ledW);
    }

    MyMessage msgColorStatus;
    MyMessage msgWhiteStatus;
};

TEST_F(KitchenTests, loop_no_changes_pass)
{
    loop();
}

TEST_F(KitchenTests, on_btn_short_release_leds_on)
{
    save_white_leds(SwitchingSource::Button, 0x40);
    save_color_leds(SwitchingSource::Button, 0x10, 0x20, 0x30);

    EXPECT_CALL(MySensorsMock::mock(), send(msgColorStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(ledW, fade(0x40, _)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0x10, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x20, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x30, _)).WillOnce(Return());

    on_btn_short_release(nullptr);

    EXPECT_FALSE(leds_is_transition(LedType::White));
    EXPECT_FALSE(leds_is_transition(LedType::Color));
}

TEST_F(KitchenTests, on_btn_short_release_leds_off)
{
    save_white_leds(SwitchingSource::Button, 0x40);
    save_color_leds(SwitchingSource::Button, 0x10, 0x20, 0x30);

    EXPECT_CALL(MySensorsMock::mock(), send(msgColorStatus.set(false), false)).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteStatus.set(false), false)).WillOnce(Return(true));
    EXPECT_CALL(ledW, get_value()).WillOnce(Return(0x40));
    EXPECT_CALL(ledR, get_value()).WillOnce(Return(0x10));
    EXPECT_CALL(ledG, get_value()).WillOnce(Return(0x20));
    EXPECT_CALL(ledB, get_value()).WillOnce(Return(0x30));
    EXPECT_CALL(ledW, fade(0, _)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0, _)).WillOnce(Return());

    on_btn_short_release(nullptr);
}

TEST_F(KitchenTests, on_btn_long_press_switch_colors_by_loop)
{
    EXPECT_CALL(MySensorsMock::mock(), send(msgColorStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(ledR, fade(0x10, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x20, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x30, _)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0x40, _)).WillOnce(Return());

    on_btn_long_press(nullptr);

    EXPECT_CALL(MySensorsMock::mock(), send(msgColorStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteStatus.set(false), false)).WillOnce(Return(true));
    EXPECT_CALL(ledR, fade(0x10, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x20, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x30, _)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0x0, _)).WillOnce(Return());

    on_btn_long_press(nullptr);

    EXPECT_CALL(MySensorsMock::mock(), send(msgColorStatus.set(false), false)).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(ledR, fade(0x0, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x0, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x0, _)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0x40, _)).WillOnce(Return());

    on_btn_long_press(nullptr);

    EXPECT_CALL(MySensorsMock::mock(), send(msgColorStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(MySensorsMock::mock(), send(msgWhiteStatus.set(true), false)).WillOnce(Return(true));
    EXPECT_CALL(ledR, fade(0x10, _)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x20, _)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x30, _)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0x40, _)).WillOnce(Return());

    on_btn_long_press(nullptr);
}
