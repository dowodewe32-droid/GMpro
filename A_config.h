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
  // ===== LED ===== //
  #define LED_NEOPIXEL_GRB
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN D8
  #define LED_MODE_BRIGHTNESS 5
  
  // ===== DISPLAY (WAJIB ADA) ===== //
  #define USE_DISPLAY true
  #define FLIP_DIPLAY true
  #define SH1106_I2C       
  #define I2C_ADDR 0x3C
  #define I2C_SDA 4        // D2
  #define I2C_SCL 5        // D1
  #define DISPLAY_TEXT "WifiX v1.5" // INI YANG BIKIN ERROR TADI
  #define INTRO_STR "WifiX v1.5"

  // ===== BUTTONS ===== //
  #define BUTTON_UP D5     
  #define BUTTON_DOWN D6   
  #define BUTTON_A D4      
  #define BUTTON_B D3      
#endif

// ========= FALLBACKS (Agar Compiler Tenang) ========= //

#ifndef AUTOSAVE_ENABLED
  #define AUTOSAVE_ENABLED true
#endif 

#ifndef AP_SSID
  #define AP_SSID "#WifiX.1.5#"
#endif 

#ifndef AP_PASSWD
  #define AP_PASSWD "deauther"
#endif 

#ifndef WEB_URL
  #define WEB_URL "deauth.me"
#endif 

#ifndef DISPLAY_TIMEOUT
  #define DISPLAY_TIMEOUT 600
#endif

#ifndef DEAUTHER_VERSION
  #define DEAUTHER_VERSION "2.6.1"
#endif

// =============== LED & DISPLAY LOGIC =============== //
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

// Penutup Error Check
#if LED_MODE_BRIGHTNESS == 0
#error LED_MODE_BRIGHTNESS must not be zero!
#endif 
