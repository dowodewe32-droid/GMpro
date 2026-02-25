/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#include "led.h"
#include "A_config.h" 
#include <Arduino.h>  
#include "language.h" 
#include "settings.h" 
#include "Attack.h"   
#include "Scan.h"     

// FIX: Pakai kurung sudut agar mencari di library sistem Arduinodroid
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

#if defined(LED_NEOPIXEL_RGB)
    Adafruit_NeoPixel strip { LED_NUM, LED_NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800 };
#elif defined(LED_NEOPIXEL_GRB)
    Adafruit_NeoPixel strip { LED_NUM, LED_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 };
#elif defined(LED_MY92)
    my92xx myled { LED_MY92_MODEL, LED_NUM, LED_MY92_DATA, LED_MY92_CLK, MY92XX_COMMAND_DEFAULT };
#elif defined(LED_DOTSTAR)
    Adafruit_DotStar strip { LED_NUM, LED_DOTSTAR_DATA, LED_DOTSTAR_CLK, DOTSTAR_BGR };
#endif

    void setColor(uint8_t r, uint8_t g, uint8_t b) {
#if defined(LED_DIGITAL)
        if (LED_ANODE) {
            if (LED_PIN_R < 255) digitalWrite(LED_PIN_R, r > 0);
            if (LED_PIN_G < 255) digitalWrite(LED_PIN_G, g > 0);
            if (LED_PIN_B < 255) digitalWrite(LED_PIN_B, b > 0);
        } else {
            if (LED_PIN_R < 255) digitalWrite(LED_PIN_R, r == 0);
            if (LED_PIN_G < 255) digitalWrite(LED_PIN_G, g == 0);
            if (LED_PIN_B < 255) digitalWrite(LED_PIN_B, b == 0);
        }
#elif defined(LED_RGB)
        analogWrite(LED_PIN_R, LED_ANODE ? 255-r : r);
        analogWrite(LED_PIN_G, LED_ANODE ? 255-g : g);
        analogWrite(LED_PIN_B, LED_ANODE ? 255-b : b);
#elif defined(LED_NEOPIXEL) || defined(LED_DOTSTAR)
        for (uint16_t i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, r, g, b);
        strip.show();
#endif
    }

    void setup() {
        analogWriteRange(0xff);
#if defined(LED_NEOPIXEL) || defined(LED_DOTSTAR)
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
