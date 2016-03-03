#ifndef ARDUINO_STORAGE_H
#define ARDUINO_STORAGE_H

#include <Arduino.h>

#include "Types.h"

void storage_setup();

void save_color_leds(byte R, byte G, byte B);
void save_white_leds(byte W);
void restore_color_leds();
void restore_white_leds();

//#include "Storage.cpp"

#endif //ARDUINO_STORAGE_H
