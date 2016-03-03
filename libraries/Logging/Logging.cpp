#include "Logging.h"

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
