/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// 1. CORE SYSTEM & VERSIONING
// ==========================================
#define DEAUTHER_VERSION "2.6.1"
#define DEAUTHER_VERSION_MAJOR 2
#define DEAUTHER_VERSION_MINOR 6
#define DEAUTHER_VERSION_REVISION 1

#define EEPROM_SIZE 4095
#define BOOT_COUNTER_ADDR 1
#define SETTINGS_ADDR 100

// ==========================================
// 2. HARDWARE PROFILE: NODEMCU
// ==========================================
#define NODEMCU

#ifdef NODEMCU
  // LED Configuration
  #define LED_NEOPIXEL_GRB
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN 15 // GPIO15 = D8
  #define LED_MODE_BRIGHTNESS 10
  
  // Display Configuration (SH1106 I2C)
  #define USE_DISPLAY true
  #define FLIP_DIPLAY true
  #define SH1106_I2C       
  #define I2C_ADDR 0x3C
  #define I2C_SDA 4        // GPIO4 = D2
  #define I2C_SCL 5        // GPIO5 = D1
  #define DISPLAY_TEXT "WifiX v1.5"
  #define INTRO_STR "WifiX v1.5"

  // Button Configuration
  #define BUTTON_UP 14     // GPIO14 = D5
  #define BUTTON_DOWN 12   // GPIO12 = D6
  #define BUTTON_A 2       // GPIO2  = D4
  #define BUTTON_B 0       // GPIO0  = D3
  
  #define RESET_BUTTON 0   // Flash Button
#endif

// ==========================================
// 3. NETWORK & WEB SETTINGS (FIXED FORMAT)
// ==========================================
// Format array {} wajib untuk IPAddress di wifi.cpp
#define AP_SSID "#WifiX.1.5#"
#define AP_PASSWD "deauther"
#define AP_HIDDEN false
#define AP_IP_ADDR {192, 168, 4, 1}

#define WEB_IP_ADDR {192, 168, 4, 1} 
#define WEB_URL "deauth.me"
#define WEB_ENABLED true
#define WEB_CAPTIVE_PORTAL false
#define WEB_USE_SPIFFS false
#define DEFAULT_LANG "en"

// ==========================================
// 4. ATTACK & SNIFFER DEFAULTS
// ==========================================
#define ATTACK_ALL_CH false
#define RANDOM_TX false
#define ATTACK_TIMEOUT 0
#define DEAUTHS_PER_TARGET 25
#define DEAUTH_REASON 1
#define PROBE_FRAMES_PER_SSID 1
#define CH_TIME 200
#define MIN_DEAUTH_FRAMES 3

// ==========================================
// 5. SYSTEM FEATURES
// ==========================================
#define CLI_ENABLED true
#define CLI_ECHO true
#define ENABLE_REPEATER true
#define DISPLAY_TIMEOUT 600

#ifndef STROBE
  #define STROBE 2
  #define NYALA HIGH
  #define MATI LOW
#endif

#define ENABLE_DEBUG
#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200

// ==========================================
// 6. LED COLOR MODES
// ==========================================
#define LED_MODE_OFF 0, 0, 0
#define LED_MODE_SCAN 0, 0, 255
#define LED_MODE_ATTACK 255, 0, 0
#define LED_MODE_IDLE 0, 255, 0

// Logic Protectors
#if defined(LED_NEOPIXEL_RGB) || defined(LED_NEOPIXEL_GRB)
  #define LED_NEOPIXEL
#endif 

#ifndef USE_LED
  #define USE_LED true
#endif

#endif // CONFIG_H
