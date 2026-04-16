#pragma once

// ===== CORE SETTINGS =====
#define DEAUTHER_VERSION "2.6.1"
#define BOOT_COUNTER_ADDR 1
#define SETTINGS_ADDR 100

// ===== VERSION CONSTANTS =====
#define DEAUTHER_VERSION_MAJOR 2
#define DEAUTHER_VERSION_MINOR 6
#define DEAUTHER_VERSION_REVISION 1

// ===== HARDWARE SELECTION =====
#define NODEMCU
#define USE_LED true
#define LED_NEOPIXEL_GRB
#define LED_NEOPIXEL
#define LED_NUM 1
#define LED_NEOPIXEL_PIN 15 // Pin D8
#define LED_MODE_BRIGHTNESS 10

// LED PINS
#define STROBE 2 // Pin D4
#define INDIKATOR_NYALA HIGH
#define NYALA INDIKATOR_NYALA

// ===== DISPLAY SETTINGS (FIX ERROR 1000197478) =====
#define USE_DISPLAY true
#define SH1106_I2C
#define I2C_ADDR 0x3C
#define I2C_SDA 4 // Pin D2
#define I2C_SCL 5 // Pin D1
#define FLIP_DIPLAY true
#define DISPLAY_TEXT "GMpro87dev"
#define INTRO_STR "GMpro87dev"
#define DISPLAY_TIMEOUT 5000

// ===== BUTTONS =====
#define BUTTON_UP 14   // D5
#define BUTTON_DOWN 12 // D6
#define BUTTON_A 2     // D4
#define BUTTON_B 0     // D3
#define RESET_BUTTON 0 // Flash Button

// ===== SYSTEM SETTINGS =====
#define PROBE_FRAMES_PER_SSID 1
#define MIN_DEAUTH_FRAMES 3
#define CLI_ECHO true
#define CLI_ENABLED true
#define WEB_ENABLED true
#define EEPROM_SIZE 512

// ===== DEBUG MACROS =====
#define debugF(x) (x)
#define debuglnF(x) Serial.println(x)

// ===== ATTACK SETTINGS =====
#define ATTACK_ALL_CH true
#define RANDOM_TX false
#define ATTACK_TIMEOUT 30
#define DEAUTHS_PER_TARGET 5
#define DEAUTH_REASON 1

// ===== SNIFFER SETTINGS =====
#define CH_TIME 200

// ===== ACCESS POINT SETTINGS =====
#define AP_SSID "GMpro87"
#define AP_PASSWD "Sangkur87"
#define AP_HIDDEN false
#define AP_IP_ADDR {192,168,4,1}

// ===== WEB SETTINGS =====
#define WEB_CAPTIVE_PORTAL true
#define WEB_USE_SPIFFS true
#define DEFAULT_LANG "en"

// ===== WEB SERVER SETTINGS =====
#define WEB_IP_ADDR {192,168,4,1}
#define WEB_URL "esp8266-webupdate"
#define WEB_NETMASK {255,255,255,0}
#define WEB_SERVER_PORT 80

// ===== LED MODES =====
#define LED_MODE_OFF 0, 0, 0
#define LED_MODE_SCAN 0, 0, 255
#define LED_MODE_ATTACK 255, 0, 0
#define LED_MODE_IDLE 0, 255, 0
