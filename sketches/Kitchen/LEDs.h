#ifndef ARDUINO_LEDS_H
#define ARDUINO_LEDS_H

#include <Arduino.h>

#include "Types.h"
#include "Parsing.h"

#define FADE_TIME_MS    3000

void leds_reset();
void leds_setup();
void leds_process();

void leds_color_fade(const RGBW & start, const RGBW & stop, unsigned long time = FADE_TIME_MS);
void leds_color_fade(byte R, byte G, byte B, unsigned long time = FADE_TIME_MS);
void leds_white_fade(const RGBW & start, const RGBW & stop, unsigned long time = FADE_TIME_MS);
void leds_white_fade(byte W, unsigned long time = FADE_TIME_MS);

void switch_leds_off();
void switch_color_leds_off();
void switch_white_leds_off();

RGBW led_values();
bool leds_is_transition(LedType led_type);

void leds_set_transition(LedType led_type, const Transition & transition_);
void leds_start_transition(LedType led_type, bool start);

#endif //ARDUINO_LEDS_H
