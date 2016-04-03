#ifndef ARDUINO_NETWORKING_H
#define ARDUINO_NETWORKING_H

#include <Arduino.h>

#include <MySensor.h>

void network_setup();
void network_process();

void network_send_color_status(bool on);
void network_send_white_status(bool on);

void send(MyMessage & message);

#endif //ARDUINO_NETWORKING_H
