#ifndef ARDUINO_NETWORKING_H
#define ARDUINO_NETWORKING_H

#include <Arduino.h>

class MyMessage;

void network_send_color_status(bool on);
void network_send_white_status(bool on);

void send_message(MyMessage & message);

#endif //ARDUINO_NETWORKING_H
