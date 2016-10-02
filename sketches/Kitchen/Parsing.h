#ifndef ARDUINO_PARSING_H
#define ARDUINO_PARSING_H

#include "Types.h"

struct CommandData
{
    CommandData(Command command_ = Command::NONE, Transition transition_ = {{0, 0, 0, 0}, {0, 0, 0, 0}, 0, false});

    Command command;
    union {
        RGBW       limit;
        Transition transition;
    };
};

CommandData parse_command(const char * payload, LedType led_type);

#endif // #ifndef ARDUINO_PARSING_H

