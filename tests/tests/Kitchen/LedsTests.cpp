#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <LEDFader.h>

#include "../../mocks/Arduino.h"
#include "../../mocks/MySensors.h"

#include "Kitchen/LEDs.h"

extern LEDFader ledW;
extern LEDFader ledR;
extern LEDFader ledG;
extern LEDFader ledB;

using namespace ::testing;

class LedsTests : public testing::Test
{
public:
    LedsTests() :
        white_one_shot{{0, 0, 0, 0x10}, {0, 0, 0, 0xF0}, 10, false},
        color_one_shot{{0x1, 0x5, 0x20, 0}, {0x10, 0x50, 0xFF, 0}, 100, false},
        white_loop{{0, 0, 0, 0x11}, {0, 0, 0, 0xF1}, 111, true},
        color_loop{{0x2, 0x6, 0x21, 0}, {0x20, 0x60, 0xFF, 0}, 222, true}
    {
        leds_reset();
    }

    ~LedsTests()
    {
        Mock::VerifyAndClearExpectations(&ArduinoMock::mock());
        Mock::VerifyAndClearExpectations(&MySensorsMock::mock());
        Mock::VerifyAndClearExpectations(&ledR);
        Mock::VerifyAndClearExpectations(&ledG);
        Mock::VerifyAndClearExpectations(&ledB);
        Mock::VerifyAndClearExpectations(&ledW);
    }

    Transition white_one_shot;
    Transition color_one_shot;
    Transition white_loop;
    Transition color_loop;
};

TEST_F(LedsTests, transition_white_start_pass)
{
    leds_set_transition(LedType::White, white_one_shot);

    EXPECT_CALL(ledW, set_value(0x10)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0xF0, 10)).WillOnce(Return());

    leds_start_transition(LedType::White, true);

    EXPECT_TRUE(leds_is_transition(LedType::White));
    EXPECT_FALSE(leds_is_transition(LedType::Color));

    leds_process();
    leds_process();
}

TEST_F(LedsTests, transition_color_start_pass)
{
    leds_set_transition(LedType::Color, color_one_shot);

    EXPECT_CALL(ledR, set_value(0x1)).WillOnce(Return());
    EXPECT_CALL(ledG, set_value(0x5)).WillOnce(Return());
    EXPECT_CALL(ledB, set_value(0x20)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0x10, 100)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x50, 100)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0xFF, 100)).WillOnce(Return());

    leds_start_transition(LedType::Color, true);

    EXPECT_FALSE(leds_is_transition(LedType::White));
    EXPECT_TRUE(leds_is_transition(LedType::Color));

    leds_process();
    leds_process();
}

TEST_F(LedsTests, transition_white_loop)
{
    leds_set_transition(LedType::White, white_loop);

    EXPECT_CALL(ledW, set_value(0x11)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0xF1, 111)).WillOnce(Return());

    // start
    leds_start_transition(LedType::White, true);

    EXPECT_CALL(ledW, get_value()).WillOnce(Return(0x50));

    // still going up
    leds_process();

    EXPECT_CALL(ledW, get_value()).WillOnce(Return(0xF1));
    EXPECT_CALL(ledW, set_value(0xF1)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0x11, 111)).WillOnce(Return());

    // is maximum, going down
    leds_process();

    EXPECT_CALL(ledW, get_value()).WillOnce(Return(0x50));

    // down - still going down
    leds_process();

    EXPECT_CALL(ledW, get_value()).WillOnce(Return(0x11));
    EXPECT_CALL(ledW, set_value(0x11)).WillOnce(Return());
    EXPECT_CALL(ledW, fade(0xF1, 111)).WillOnce(Return());

    // repeat
    leds_process();
}

TEST_F(LedsTests, transition_color_loop)
{
    //         color_loop{{0x2, 0x6, 0x21, 0}, {0x20, 0x60, 0xFF, 0}, 222, true}

    leds_set_transition(LedType::Color, color_loop);

    EXPECT_CALL(ledR, set_value(0x2)).WillOnce(Return());
    EXPECT_CALL(ledG, set_value(0x6)).WillOnce(Return());
    EXPECT_CALL(ledB, set_value(0x21)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0x20, 222)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x60, 222)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0xFF, 222)).WillOnce(Return());

    // start
    leds_start_transition(LedType::Color, true);

    EXPECT_CALL(ledR, get_value()).WillOnce(Return(0x2));

    // still going up
    leds_process();

    EXPECT_CALL(ledR, get_value()).WillOnce(Return(0x20));
    EXPECT_CALL(ledG, get_value()).WillOnce(Return(0x60));
    EXPECT_CALL(ledB, get_value()).WillOnce(Return(0xFF));
    EXPECT_CALL(ledR, set_value(0x20)).WillOnce(Return());
    EXPECT_CALL(ledG, set_value(0x60)).WillOnce(Return());
    EXPECT_CALL(ledB, set_value(0xFF)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0x2, 222)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x6, 222)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0x21, 222)).WillOnce(Return());

    // is maximum, going down
    leds_process();

    EXPECT_CALL(ledR, get_value()).WillOnce(Return(0x10));

    // down - still going down
    leds_process();

    EXPECT_CALL(ledR, get_value()).WillOnce(Return(0x2));
    EXPECT_CALL(ledG, get_value()).WillOnce(Return(0x6));
    EXPECT_CALL(ledB, get_value()).WillOnce(Return(0x21));
    EXPECT_CALL(ledR, set_value(0x2)).WillOnce(Return());
    EXPECT_CALL(ledG, set_value(0x6)).WillOnce(Return());
    EXPECT_CALL(ledB, set_value(0x21)).WillOnce(Return());
    EXPECT_CALL(ledR, fade(0x20, 222)).WillOnce(Return());
    EXPECT_CALL(ledG, fade(0x60, 222)).WillOnce(Return());
    EXPECT_CALL(ledB, fade(0xFF, 222)).WillOnce(Return());

    // repeat
    leds_process();
}
