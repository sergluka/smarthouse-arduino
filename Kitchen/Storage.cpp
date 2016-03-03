#include "Storage.h"

#include <Logging.h>
#include <EEPROM.h>

#include "LEDs.h"
#include "Networking.h"

#define STORAGE_VERSION 1

struct Storage
{
    byte version;
    RGBW color;
}  __attribute__((packed));

static Storage storage = {STORAGE_VERSION, {0, 0, 0, 0}};

void save_color_leds(byte R, byte G, byte B)
{
    storage.color.R = R;
    storage.color.G = G;
    storage.color.B = B;
    EEPROM.put(0, storage);
}
void save_white_leds(byte W)
{
    storage.color.W = W;
    EEPROM.put(0, storage);
}

void load_led_values()
{
    EEPROM.get(0, storage);
    if (storage.version != STORAGE_VERSION) {
        storage.color.R = 255;
        storage.color.G = 255;
        storage.color.B = 255;
        storage.color.W = 255;
    }
    LOG_INFO("Restored values: R=%d, G=%d, B=%d, W=%d",
             storage.color.R, storage.color.G, storage.color.B, storage.color.W);
}

void storage_setup()
{
    load_led_values();
}

void restore_color_leds()
{
    leds_color_fade(storage.color.R, storage.color.G, storage.color.B);
    network_send_color_status(true);
}

void restore_white_leds()
{
    leds_white_fade(storage.color.W, FADE_TIME_MS);
    network_send_white_status(true);
}
