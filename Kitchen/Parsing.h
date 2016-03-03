#ifndef ARDUINO_PARSING_H
#define ARDUINO_PARSING_H

#include "Types.h"

struct Transition
{
    RGBW start;
    RGBW stop;
    unsigned long time;
    bool is_loop;
};

struct CommandData
{
    Command command;
    union {
        RGBW       limit;
        Transition transition;
    };

    CommandData(Command command_ = Command::NONE,
                Transition transition_ = {{0, 0, 0, 0}, {0, 0, 0, 0}, 0, false}) :
        command(command_),
        transition(transition_)
    {
    };
};

CommandData parse_command(const char * payload, LedType led_type);;

//#include "Parsing.cpp"

#endif // #ifndef ARDUINO_PARSING_H

