#include "led.h"
#include "A_config.h"
#include <Arduino.h>
#include "language.h"
#include "settings.h"
#include "Attack.h"
#include "Scan.h"

// FIX: Panggil library sistem Arduinodroid
#if defined(LED_NEOPIXEL)
  #include <Adafruit_NeoPixel.h>
#elif defined(LED_MY92)
  #include <my92xx.h>
#elif defined(LED_DOTSTAR)
  #include <Adafruit_DotStar.h>
#endif

extern Attack attack;
extern Scan   scan;

namespace led {
    LED_MODE mode = OFF;

#if defined(LED_NEOPIXEL_GRB)
    Adafruit_NeoPixel strip { LED_NUM, LED_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 };
#elif defined(LED_NEOPIXEL_RGB)
    Adafruit_NeoPixel strip { LED_NUM, LED_NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800 };
#endif

    void setColor(uint8_t r, uint8_t g, uint8_t b) {
        #if defined(LED_NEOPIXEL)
        for (uint16_t i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, r, g, b);
        strip.show();
        #endif
    }

    void setup() {
        #if defined(LED_NEOPIXEL)
        strip.begin();
        strip.setBrightness(LED_MODE_BRIGHTNESS);
        strip.show();
        #endif
    }

    void update() {
        if (!settings::getLEDSettings().enabled) setMode(OFF);
        else if (attack.isRunning()) setMode(ATTACK);
        else if (scan.isScanning()) setMode(SCAN);
        else setMode(IDLE);
    }

    void setMode(LED_MODE new_mode, bool force) {
        if ((new_mode != mode) || force) {
            mode = new_mode;
            switch (mode) {
                case OFF: setColor(0,0,0); break;
                case SCAN: setColor(0,0,255); break;
                case ATTACK: setColor(255,0,0); break;
                case IDLE: setColor(0,255,0); break;
            }
        }
    }
}
