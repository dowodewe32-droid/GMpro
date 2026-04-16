#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include <EEPROM.h>
#include "repeater.h"
#include "wifi.h"
#include "settings.h"

// Ambil library NAPT langsung dari path internal
#include <lwip/napt.h>
#include <lwip/dns.h>

#define NAPT 1000
#define NAPT_PORT 10

extern ap_settings_t ap_settings;
static bool state = false;

namespace repeater {
    void connect(String ssid, String pass){
        File f1 = LittleFS.open("/ssid.txt", "w");
        if(f1) { f1.print(ssid); f1.close(); }
        File f2 = LittleFS.open("/pass.txt", "w");
        if(f2) { f2.print(pass); f2.close(); }
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid.c_str(), pass.c_str());
    }

    void run() {
        wifi::stopAP();
        WiFi.mode(WIFI_STA);
        EEPROM.write(400, 0);
        EEPROM.commit();
        
        delay(1000); 
        
        WiFi.softAPConfig(IPAddress(172, 217, 28, 254), IPAddress(172, 217, 28, 254), IPAddress(255, 255, 255, 0));
        WiFi.softAP(ap_settings.ssid, ap_settings.password);
        
        if (ip_napt_init(NAPT, NAPT_PORT) == ERR_OK) {
            ip_napt_enable_no(SOFTAP_IF, 1);
            state = true;
        }
    }

    bool status() { return state; }
    void update_status(bool s) { state = s; }
}
