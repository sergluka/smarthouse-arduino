#ifndef _LOGGING_
#define _LOGGING_

#include <Arduino.h>
#include <utility/RF24_config.h>

#include <string.h>

void log(const __FlashStringHelper *fmt, ...)
{
    static char buf[128];

    va_list args;
    va_start (args, fmt);
    vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
    va_end(args);
    Serial.println(buf);
    Serial.flush();
}

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOGGING_SETUP(_speed) Serial.begin(_speed)
#define LOG_DEBUG(_msg, ...) log(F("[DEBUG] " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);
#define LOG_INFO(_msg, ...)  log(F("[INFO]  " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);
#define LOG_ERROR(_msg, ...) log(F("[ERROR] " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);

#endif // _LOGGING_

