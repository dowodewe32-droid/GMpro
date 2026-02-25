/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#ifndef CONFIG_H
#define CONFIG_H

// 1. SYSTEM CONSTANTS
#define DEAUTHER_VERSION "2.6.1"
#define DEAUTHER_VERSION_MAJOR 2
#define DEAUTHER_VERSION_MINOR 6
#define DEAUTHER_VERSION_REVISION 1
#define EEPROM_SIZE 4095
#define BOOT_COUNTER_ADDR 1
#define SETTINGS_ADDR 100

// 2. HARDWARE (NODEMCU)
#define NODEMCU
#define NYALA 1
#define MATI 0
#define STROBE 2

// LED & DISPLAY (USE NUMBERS ONLY)
#define LED_NEOPIXEL_GRB
#define LED_NUM 1
#define LED_NEOPIXEL_PIN 15
#define LED_MODE_BRIGHTNESS 10
#define USE_LED 1
#define USE_DISPLAY 1
#define FLIP_DIPLAY 1
#define SH1106_I2C       
#define I2C_ADDR 0x3C
#define I2C_SDA 4
#define I2C_SCL 5
#define DISPLAY_TEXT "WifiX_v1.5"
#define INTRO_STR "WifiX_v1.5"

// BUTTONS (GPIO)
#define BUTTON_UP 14
#define BUTTON_DOWN 12
#define BUTTON_A 2
#define BUTTON_B 0
#define RESET_BUTTON 0   

// 3. NETWORK
#define AP_SSID "WifiX_v1.5"
#define AP_PASSWD "deauther"
#define AP_HIDDEN 0
#define AP_IP_ADDR {192, 168, 4, 1}
#define WEB_IP_ADDR {192, 168, 4, 1} 
#define WEB_URL "deauth.me"
#define WEB_ENABLED 1
#define WEB_CAPTIVE_PORTAL 0
#define WEB_USE_SPIFFS 0
#define DEFAULT_LANG "en"

// 4. ATTACK & SNIFFER
#define ATTACK_ALL_CH 0
#define RANDOM_TX 0
#define ATTACK_TIMEOUT 0
#define DEAUTHS_PER_TARGET 25
#define DEAUTH_REASON 1
#define PROBE_FRAMES_PER_SSID 1
#define CH_TIME 200
#define MIN_DEAUTH_FRAMES 3

// 5. FEATURES
#define CLI_ENABLED 1
#define CLI_ECHO 1
#define ENABLE_REPEATER 1
#define DISPLAY_TIMEOUT 600

// LED COLORS (RGB)
#define LED_MODE_OFF 0,0,0
#define LED_MODE_SCAN 0,0,255
#define LED_MODE_ATTACK 255,0,0
#define LED_MODE_IDLE 0,255,0

#ifndef LED_NEOPIXEL
#define LED_NEOPIXEL
#endif

#endif
