#ifndef ARDUINO_TYPES_H
#define ARDUINO_TYPES_H

#include <stdint.h>

enum Command : char
{
    NONE                    = '\0',
    LIGHT_SET_EXT_LIMIT     = '0',
    LIGHT_SET_BUTTON_LIMIT  = '1',
    TRANSITION_ONE_SHOT     = '2',
    TRANSITION_LOOP         = '3'
};

struct RGBW
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W;
} __attribute__((packed));

enum LedType
{
    Color,
    White,
    TYPES_MAX_SIZE
};

enum SwitchingSource
{
    External,
    Button,
    SOURCES_MAX_SIZE
};

struct  Storage
{
    uint8_t version;
    RGBW colors[SwitchingSource::SOURCES_MAX_SIZE];
}  __attribute__((packed));

struct Transition
{
    RGBW start;
    RGBW stop;
    unsigned long time;
    bool is_loop;
};

#endif //ARDUINO_TYPES_H
