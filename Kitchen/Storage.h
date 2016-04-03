#ifndef ARDUINO_STORAGE_H
#define ARDUINO_STORAGE_H

#include <Arduino.h>

#include "Types.h"

void storage_setup();

void save_color_leds(SwitchingSource source, byte R, byte G, byte B);
void save_white_leds(SwitchingSource source, byte W);
void restore_color_leds(SwitchingSource source);
void restore_white_leds(SwitchingSource source);

#endif //ARDUINO_STORAGE_H
