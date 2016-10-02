#ifndef ARDUINO_STORAGE_H
#define ARDUINO_STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>

#include "Types.h"

#define STORAGE_VERSION 2

void storage_reset();
void storage_setup();

void save_color_leds(SwitchingSource source, byte R, byte G, byte B);
void save_white_leds(SwitchingSource source, byte W);
void restore_color_leds(SwitchingSource source);
void restore_white_leds(SwitchingSource source);

bool is_stored_equal(SwitchingSource source, const RGBW & leds);

#endif //ARDUINO_STORAGE_H
