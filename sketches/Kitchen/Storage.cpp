#include "Storage.h"

#include <Logging.h>

#include "LEDs.h"
#include "Networking.h"

// Use templates mock class since it's not possible to mock template functions `get` and `put` of EEPROMClass
#ifdef UNITTESTS
EEPROMClass<Storage> eeprom;
#else
EEPROMClass eeprom;
#endif

void load_storage();

Storage storage = {STORAGE_VERSION, {0, 0, 0, 0}};

void storage_reset()
{
    storage = {STORAGE_VERSION, {0, 0, 0, 0}};
}

void storage_setup()
{
    storage_reset();
    load_storage();
}

void save_color_leds(SwitchingSource source, byte R, byte G, byte B)
{
    storage.colors[source].R = R;
    storage.colors[source].G = G;
    storage.colors[source].B = B;
    eeprom.put(0, storage);
}

void save_white_leds(SwitchingSource source, byte W)
{
    storage.colors[source].W = W;
    eeprom.put(0, storage);
}

void load_storage()
{
    eeprom.get(0, storage);
    if (storage.version != STORAGE_VERSION) {
        LOG_WARN("Old data format or uninitialized flash memory. Reset to default values");
        storage.version = STORAGE_VERSION;
        storage.colors[SwitchingSource::Button] = {255, 255, 255, 255};
        storage.colors[SwitchingSource::External] = {255, 255, 255, 255};
    }
    LOG_INFO("Restored values: external={R=%d, G=%d, B=%d, W=%d}, button={R=%d, G=%d, B=%d, W=%d}",
             storage.colors[SwitchingSource::Button].R,
             storage.colors[SwitchingSource::Button].G,
             storage.colors[SwitchingSource::Button].B,
             storage.colors[SwitchingSource::Button].W,
             storage.colors[SwitchingSource::External].R,
             storage.colors[SwitchingSource::External].G,
             storage.colors[SwitchingSource::External].B,
             storage.colors[SwitchingSource::External].W);
}

void restore_color_leds(SwitchingSource source)
{
    leds_color_fade(storage.colors[source].R, storage.colors[source].G, storage.colors[source].B);
    network_send_color_status(true);
}

void restore_white_leds(SwitchingSource source)
{
    leds_white_fade(storage.colors[source].W, FADE_TIME_MS);
    network_send_white_status(true);
}

bool is_stored_equal(SwitchingSource source, const RGBW & leds)
{
    return storage.colors[source].R == leds.R &&
           storage.colors[source].G == leds.G &&
           storage.colors[source].B == leds.B &&
           storage.colors[source].W == leds.W;

}

