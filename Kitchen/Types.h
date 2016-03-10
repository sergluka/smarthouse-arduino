#ifndef ARDUINO_TYPES_H
#define ARDUINO_TYPES_H

#include <stdint.h>

enum Command : char {
    NONE                    = '\0',
    LIGHT_SET_LIMIT         = '0',
    TRANSITION_ONE_SHOT     = '1',
    TRANSITION_LOOP         = '2'
};

struct RGBW {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W;
} __attribute__((packed));

enum LedType {
    Color,
    White,
    MAX_SIZE
};

#endif //ARDUINO_TYPES_H
