#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Kitchen/Parsing.h"

using namespace ::testing;

class ParsingTests : public testing::Test
{
public:
    ParsingTests()
    {
    }

    ~ParsingTests()
    {
    }

    void parsing_should_be_failed(const CommandData & result)
    {
        EXPECT_EQ(NONE, result.command);
    }
};

TEST_F(ParsingTests, parse_command_white_set_ext_limit_pass)
{
    CommandData result = parse_command("0,54", LedType::White);
    EXPECT_EQ(LIGHT_SET_EXT_LIMIT, result.command);
    EXPECT_EQ(0, result.limit.R);
    EXPECT_EQ(0, result.limit.G);
    EXPECT_EQ(0, result.limit.B);
    EXPECT_EQ(0x54, result.limit.W);
}

TEST_F(ParsingTests, parse_command_white_set_ext_limit_fail)
{
    parsing_should_be_failed(parse_command("0,544", LedType::White));
}

TEST_F(ParsingTests, parse_command_color_set_ext_limit_pass)
{
    CommandData result = parse_command("0,54FF77", LedType::Color);
    EXPECT_EQ(LIGHT_SET_EXT_LIMIT, result.command);
    EXPECT_EQ(0x54, result.limit.R);
    EXPECT_EQ(0xFF, result.limit.G);
    EXPECT_EQ(0x77, result.limit.B);
    EXPECT_EQ(0, result.limit.W);
}

TEST_F(ParsingTests, parse_command_color_set_ext_limit_fail)
{
    parsing_should_be_failed(parse_command("0,54FF77,", LedType::Color));
    parsing_should_be_failed(parse_command("0,54FF777", LedType::Color));
}

TEST_F(ParsingTests, parse_command_white_set_button_limit_pass)
{
    CommandData result = parse_command("1,54", LedType::White);
    EXPECT_EQ(LIGHT_SET_BUTTON_LIMIT, result.command);
    EXPECT_EQ(0, result.limit.R);
    EXPECT_EQ(0, result.limit.G);
    EXPECT_EQ(0, result.limit.B);
    EXPECT_EQ(0x54, result.limit.W);
}

TEST_F(ParsingTests, parse_command_color_set_button_limit_pass)
{
    CommandData result = parse_command("1,54FF77", LedType::Color);
    EXPECT_EQ(LIGHT_SET_BUTTON_LIMIT, result.command);
    EXPECT_EQ(0x54, result.limit.R);
    EXPECT_EQ(0xFF, result.limit.G);
    EXPECT_EQ(0x77, result.limit.B);
    EXPECT_EQ(0, result.limit.W);
}

TEST_F(ParsingTests, parse_command_white_fade_pass)
{
    CommandData result = parse_command("2,77,55,1000000", LedType::White);
    EXPECT_EQ(TRANSITION_ONE_SHOT, result.command);
    EXPECT_EQ(0, result.transition.start.R);
    EXPECT_EQ(0, result.transition.start.G);
    EXPECT_EQ(0, result.transition.start.B);
    EXPECT_EQ(0x77, result.transition.start.W);
    EXPECT_EQ(0, result.transition.stop.R);
    EXPECT_EQ(0, result.transition.stop.G);
    EXPECT_EQ(0, result.transition.stop.B);
    EXPECT_EQ(0x55, result.transition.stop.W);
    EXPECT_EQ(1000000U, result.transition.time);
    EXPECT_FALSE(result.transition.is_loop);
}

TEST_F(ParsingTests, parse_command_color_fade_pass)
{
    CommandData result = parse_command("2,54FF77,010203,123", LedType::Color);
    EXPECT_EQ(TRANSITION_ONE_SHOT, result.command);
    EXPECT_EQ(0x54, result.transition.start.R);
    EXPECT_EQ(0xFF, result.transition.start.G);
    EXPECT_EQ(0x77, result.transition.start.B);
    EXPECT_EQ(0, result.transition.start.W);
    EXPECT_EQ(0x01, result.transition.stop.R);
    EXPECT_EQ(0x02, result.transition.stop.G);
    EXPECT_EQ(0x03, result.transition.stop.B);
    EXPECT_EQ(0, result.transition.stop.W);
    EXPECT_EQ(123U, result.transition.time);
    EXPECT_FALSE(result.transition.is_loop);
}

TEST_F(ParsingTests, parse_command_white_loop_pass)
{
    CommandData result = parse_command("3,77,55,1000000", LedType::White);
    EXPECT_EQ(TRANSITION_LOOP, result.command);
    EXPECT_EQ(0, result.transition.start.R);
    EXPECT_EQ(0, result.transition.start.G);
    EXPECT_EQ(0, result.transition.start.B);
    EXPECT_EQ(0x77, result.transition.start.W);
    EXPECT_EQ(0, result.transition.stop.R);
    EXPECT_EQ(0, result.transition.stop.G);
    EXPECT_EQ(0, result.transition.stop.B);
    EXPECT_EQ(0x55, result.transition.stop.W);
    EXPECT_EQ(1000000U, result.transition.time);
    EXPECT_TRUE(result.transition.is_loop);
}

TEST_F(ParsingTests, parse_command_color_loop_pass)
{
    CommandData result = parse_command("3,54FF77,010203,123", LedType::Color);
    EXPECT_EQ(TRANSITION_LOOP, result.command);
    EXPECT_EQ(0x54, result.transition.start.R);
    EXPECT_EQ(0xFF, result.transition.start.G);
    EXPECT_EQ(0x77, result.transition.start.B);
    EXPECT_EQ(0, result.transition.start.W);
    EXPECT_EQ(0x01, result.transition.stop.R);
    EXPECT_EQ(0x02, result.transition.stop.G);
    EXPECT_EQ(0x03, result.transition.stop.B);
    EXPECT_EQ(0, result.transition.stop.W);
    EXPECT_EQ(123U, result.transition.time);
    EXPECT_TRUE(result.transition.is_loop);
}

TEST_F(ParsingTests, parse_command_too_long_time_fail)
{
    parsing_should_be_failed(parse_command("3,54FF77,010203,10000000000", LedType::Color));
}

TEST_F(ParsingTests, parse_command_wrong_cmd_fail)
{
    parsing_should_be_failed(parse_command("5,54FF77,010203,10000000000", LedType::Color));
}

TEST_F(ParsingTests, parse_command_wrong_separator_fail)
{
    parsing_should_be_failed(parse_command("3;54FF77,010203,10000000000", LedType::Color));
}

