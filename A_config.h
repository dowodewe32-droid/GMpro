/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#pragma once

// ========== USER OVERRIDE ========== //
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

#ifndef ENABLE_REPEATER
  #define ENABLE_REPEATER true
#endif

// PAKSA PILIH NODEMCU
#define NODEMCU

// ========== CONFIGS ========== //

#if defined(NODEMCU)
  #define LED_NEOPIXEL_GRB
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN D8
  #define LED_MODE_BRIGHTNESS 5
  
  #define USE_DISPLAY true
  #define FLIP_DIPLAY true
  #define SH1106_I2C       
  #define I2C_ADDR 0x3C
  #define I2C_SDA 4        // D2
  #define I2C_SCL 5        // D1
  #define DISPLAY_TEXT "WifiX v1.5"
  #define INTRO_STR "WifiX v1.5"

  #define BUTTON_UP D5     
  #define BUTTON_DOWN D6   
  #define BUTTON_A D4      
  #define BUTTON_B D3      
  
  #define RESET_BUTTON 0   
#endif

// ========= FALLBACKS (WAJIB BUAT SETTINGS & WIFI.CPP) ========= //

#define DEAUTHER_VERSION "2.6.1"
#define DEAUTHER_VERSION_MAJOR 2
#define DEAUTHER_VERSION_MINOR 6
#define DEAUTHER_VERSION_REVISION 1

#define EEPROM_SIZE 4095
#define BOOT_COUNTER_ADDR 1
#define SETTINGS_ADDR 100

#define ATTACK_ALL_CH false
#define RANDOM_TX false
#define ATTACK_TIMEOUT 0
#define DEAUTHS_PER_TARGET 25
#define DEAUTH_REASON 1
#define PROBE_FRAMES_PER_SSID 1

#define CH_TIME 200
#define MIN_DEAUTH_FRAMES 3

#define AP_SSID "#WifiX.1.5#"
#define AP_PASSWD "deauther"
#define AP_HIDDEN false
#define AP_IP_ADDR { 192, 168, 4, 1 }

#define WEB_ENABLED true
#define WEB_CAPTIVE_PORTAL false
#define WEB_USE_SPIFFS false
#define DEFAULT_LANG "en"

#define CLI_ENABLED true
#define CLI_ECHO true

#ifndef DISPLAY_TIMEOUT
  #define DISPLAY_TIMEOUT 600
#endif

// FIX BUAT WIFI.CPP (Log 1000197450.jpg)
// Pakai kurung kurawal {}, bukan kurung biasa ()
#define WEB_IP_ADDR {192, 168, 4, 1}
#define WEB_URL "deauth.me"

// =============== LED MODE DEFINITIONS =============== //
#define LED_MODE_OFF 0, 0, 0
#define LED_MODE_SCAN 0, 0, 255
#define LED_MODE_ATTACK 255, 0, 0
#define LED_MODE_IDLE 0, 255, 0

#if defined(LED_NEOPIXEL_RGB) || defined(LED_NEOPIXEL_GRB)
  #define LED_NEOPIXEL
#endif 

#if !defined(LED_DIGITAL) && !defined(LED_RGB) && !defined(LED_NEOPIXEL) && !defined(LED_MY92) && !defined(LED_DOTSTAR)
  #define LED_DIGITAL
  #define USE_LED false
#else 
  #define USE_LED true
#endif 

#if LED_MODE_BRIGHTNESS == 0
#error LED_MODE_BRIGHTNESS must not be zero!
#endif 
