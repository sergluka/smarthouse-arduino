#ifndef _LOGGING_
#define _LOGGING_

#include <Arduino.h>

void log(const __FlashStringHelper *fmt, ...);

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOGGING_SETUP(_speed) Serial.begin(_speed)
#define LOG_DEBUG(_msg, ...) log(F("[DEBUG] " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);
#define LOG_INFO(_msg, ...)  log(F("[INFO]  " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);
#define LOG_WARN(_msg, ...)  log(F("[WARN] " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);
#define LOG_ERROR(_msg, ...) log(F("[ERROR] " _msg " [%s:%d]") , ##__VA_ARGS__, __FILENAME__, __LINE__);

#endif // _LOGGING_

