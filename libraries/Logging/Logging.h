#ifndef _LOGGING_
#define _LOGGING_

#include <Arduino.h>

void log(const __FlashStringHelper *fmt, ... ){
    static char buf[128];
    va_list args;
    va_start (args, fmt);
    vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
    va_end(args);
    Serial.println(buf);
    Serial.flush();
}

#define LOG_DEBUG(_msg, ...) log(F("[DEBUG] " _msg) , ##__VA_ARGS__);
#define LOG_INFO(_msg, ...)  log(F("[INFO] "  _msg) , ##__VA_ARGS__);
#define LOG_ERROR(_msg, ...) log(F("[ERROR] " _msg) , ##__VA_ARGS__);

#endif // _LOGGING_
