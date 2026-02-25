#pragma once

// ===== CORE SETTINGS =====
#define DEAUTHER_VERSION "2.6.1"
#define BOOT_COUNTER_ADDR 1
#define SETTINGS_ADDR 100

// ===== HARDWARE SELECTION =====
#define NODEMCU
#define USE_LED true
#define LED_NEOPIXEL_GRB
#define LED_NUM 1
#define LED_NEOPIXEL_PIN 15 // Pin D8
#define LED_MODE_BRIGHTNESS 10

// ===== DISPLAY SETTINGS (FIX ERROR 1000197478) =====
#define USE_DISPLAY true
#define SH1106_I2C
#define I2C_ADDR 0x3C
#define I2C_SDA 4 // Pin D2
#define I2C_SCL 5 // Pin D1
#define FLIP_DIPLAY true
#define DISPLAY_TEXT "WifiX v1.5"
#define INTRO_STR "WifiX v1.5"

// ===== BUTTONS =====
#define BUTTON_UP 14   // D5
#define BUTTON_DOWN 12 // D6
#define BUTTON_A 2     // D4
#define BUTTON_B 0     // D3
#define RESET_BUTTON 0 // Flash Button

// ===== SYSTEM FIXES (FIX ERROR 1000197463) =====
#define PROBE_FRAMES_PER_SSID 1
#define MIN_DEAUTH_FRAMES 3
#define CLI_ECHO true
#define CLI_ENABLED true
#define WEB_ENABLED true

// ===== LED MODES =====
#define LED_MODE_OFF 0, 0, 0
#define LED_MODE_SCAN 0, 0, 255
#define LED_MODE_ATTACK 255, 0, 0
#define LED_MODE_IDLE 0, 255, 0
