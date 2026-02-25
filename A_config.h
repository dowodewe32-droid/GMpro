/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#pragma once

// ========== USER OVERRIDE ========== //
// Kita bungkus pakai #ifndef supaya nggak bentrok sama file debug.h atau library lain
#ifndef STROBE
  #define STROBE 2
  #define NYALA HIGH
  #define MATI LOW
#endif

#ifndef ENABLE_DEBUG
  #define ENABLE_DEBUG
  #define DEBUG_PORT Serial
  #define DEBUG_BAUD 115200
#endif

// AKTIFKAN FITUR REPEATER (Wajib ada di sini!)
#ifndef ENABLE_REPEATER
  #define ENABLE_REPEATER true
#endif

// PILIHAN BOARD
#define NODEMCU

// ========== CONFIGS ========== //

#if defined(NODEMCU) || defined(WEMOS_D1_MINI) || defined(DSTIKE_USB_DEAUTHER) || defined(DSTIKE_NODEMCU_07) || defined(DSTIKE_DEAUTHER_V1) || defined(DSTIKE_DEAUTHER_V2) || defined(DSTIKE_DEAUTHER_V3)
  // ===== LED ===== //
  #define LED_NEOPIXEL_GRB
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN D8
  #define LED_MODE_BRIGHTNESS 5
  #define INTRO_STR "WifiX v1.5"

  // ===== DISPLAY ===== //
  #define USE_DISPLAY true
  #define FLIP_DIPLAY true
  #define SH1106_I2C       // Menggunakan SH1106 untuk NodeMCU umum
  #define I2C_ADDR 0x3C
  #define I2C_SDA 4        // D2
  #define I2C_SCL 5        // D1

  // ===== BUTTONS ===== //
  #define BUTTON_UP D5     // D5
  #define BUTTON_DOWN D6   // D6
  #define BUTTON_A D4      // D4
  #define BUTTON_B D3      // D3
#endif

// https://github.com/spacehuhntech/hackheld
#if defined(HACKHELD_VEGA)
  #define USE_LED true
  #define LED_NEOPIXEL
  #define LED_NEOPIXEL_GRB
  #define LED_MODE_BRIGHTNESS 10
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN 15 // D8
  #define USE_DISPLAY true
  #define FLIP_DIPLAY true
  #define SH1106_I2C
  #define I2C_ADDR 0x3C
  #define I2C_SDA 5     // D2
  #define I2C_SCL 4      // D1
  #define BUTTON_UP 13   // D5
  #define BUTTON_DOWN 14 // D6
  #define BUTTON_A 12     // D4
  #define BUTTON_B 0     // D3

#elif defined(HACKHELD_VEGA_LITE)
  #define ONE_HIT D7
  #define LED_NEOPIXEL_GRB
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN D8
  #define INTRO_STR "Deauther 2.6"
  #define USE_DISPLAY true
  #define FLIP_DIPLAY true
  #define SSD1306_I2C
  #define I2C_ADDR 0x3c
  #define I2C_SDA 4
  #define I2C_SCL 5
  #define BUTTON_UP D5   
  #define BUTTON_DOWN D6 
  #define BUTTON_A D3     
  #define BUTTON_B D4     
#endif

// ========= FALLBACK & DEFAULT SETTINGS ========= //

#ifndef AUTOSAVE_ENABLED
  #define AUTOSAVE_ENABLED true
#endif 

#ifndef AUTOSAVE_TIME
  #define AUTOSAVE_TIME 60
#endif 

#ifndef ATTACK_ALL_CH
  #define ATTACK_ALL_CH false
#endif 

#ifndef AP_SSID
  #define AP_SSID "#WifiX.1.5#"
#endif 

#ifndef AP_PASSWD
  #define AP_PASSWD "deauther"
#endif 

#ifndef WEB_IP_ADDR
  #define WEB_IP_ADDR (192, 168, 4, 1)
#endif 

#ifndef WEB_URL
  #define WEB_URL "deauth.me"
#endif 

// =============== LED FALLBACK =============== //
#if defined(LED_NEOPIXEL_RGB) || defined(LED_NEOPIXEL_GRB)
  #define LED_NEOPIXEL
#endif 

#if !defined(LED_DIGITAL) && !defined(LED_RGB) && !defined(LED_NEOPIXEL) && !defined(LED_MY92) && !defined(LED_DOTSTAR)
  #define LED_DIGITAL
  #define USE_LED false
#else 
  #define USE_LED true
#endif 

#ifndef LED_MODE_BRIGHTNESS
  #define LED_MODE_BRIGHTNESS 10
#endif 

// =============== DISPLAY FALLBACK =============== //
#if !defined(SSD1306_I2C) && !defined(SSD1306_SPI) && !defined(SH1106_I2C) && !defined(SH1106_SPI)
  #define SSD1306_I2C
  #define USE_DISPLAY false
#else 
  #define USE_DISPLAY true
#endif 

// =============== VERSION =============== //
#define DEAUTHER_VERSION "2.6.1"
#define DEAUTHER_VERSION_MAJOR 2
#define DEAUTHER_VERSION_MINOR 6
#define DEAUTHER_VERSION_REVISION 1
#define EEPROM_SIZE 4095
#define SETTINGS_ADDR 100

// ========== ERROR CHECKS ========== //
#if LED_MODE_BRIGHTNESS == 0
#error LED_MODE_BRIGHTNESS must not be zero!
#endif 
