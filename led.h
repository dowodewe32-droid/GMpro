/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#pragma once

#include <cstdint>
#include "A_config.h"

// WAJIB: Panggil library NeoPixel di sini
#ifdef LED_NEOPIXEL
    #include <Adafruit_NeoPixel.h>
#endif

enum LED_MODE {
    OFF,
    SCAN,
    ATTACK,
    IDLE
};

namespace led {
    void setup();
    void update();
    void setMode(LED_MODE new_mode, bool force = false);
    void setColor(uint8_t r, uint8_t g, uint8_t b);
}
