#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include <EEPROM.h>

// Include header project sendiri
#include "repeater.h"
#include "wifi.h"
#include "settings.h"

// Library internal LWIP untuk fitur NAPT (Repeater)
#include <lwip/napt.h>
#include <lwip/dns.h>

// Perbaikan: Mencari dhcpserver.h tanpa folder 'apps' untuk kompatibilitas core 2.7.x
#include <dhcpserver.h>

#define NAPT 1000
#define NAPT_PORT 10

// Struktur data AP
typedef struct ap_settings_t {
    char    path[33];
    char    ssid[33];
    char    password[65];
    uint8_t channel;
    bool    hidden;
    bool    captive_portal;
} ap_settings_t;

// Mengambil variabel global ap_settings dari file lain (settings.cpp)
extern ap_settings_t ap_settings;

// Variabel lokal untuk status repeater
static bool state = false;
static String wifi_ssid, wifi_pass;

// Fungsi pembantu tulis file (Helper)
static void w(const char *path, String message){
    Serial.printf("Writing file: %s\r\n", path);
    File file = LittleFS.open(path, "w");
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    file.print(message);
    file.close();
}

// Fungsi pembantu baca file (Helper)
static String r(const char * path){
    Serial.printf("Reading file: %s\r\n", path);
    File file = LittleFS.open(path, "r");
    if(!file || file.isDirectory()){
        return String();
    }
    String fileContent = file.readStringUntil('\n');
    file.close();
    return fileContent;
}

// Fungsi hapus file
static void de(const char *path){
    LittleFS.remove(path);
}

namespace repeater {
    
    void connect(String ssid, String pass){
        w("/ssid.txt", ssid);
        w("/pass.txt", pass);
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.println("Saving repeater config..");
    }

    void run() {
        wifi::stopAP();
        WiFi.mode(WIFI_STA);
        
        wifi_ssid = r("/ssid.txt");
        wifi_pass = r("/pass.txt");
        
        de("/ssid.txt");
        de("/pass.txt");
        
        EEPROM.write(400, 0);
        EEPROM.commit();
        
        if(wifi_pass.length() < 8) {
            WiFi.begin(wifi_ssid.c_str());
        } else {
            WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
        }
        
        Serial.print("Connecting to WiFi");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print('.');
        }
        Serial.println("\nConnected!");

        // Berikan IP DNS ke sisi DHCP server
        dhcps_set_dns(0, WiFi.dnsIP(0));
        dhcps_set_dns(1, WiFi.dnsIP(1));
      
        // Konfigurasi SoftAP (IP Gateway Repeater)
        WiFi.softAPConfig(
            IPAddress(172, 217, 28, 254),
            IPAddress(172, 217, 28, 254),
            IPAddress(255, 255, 255, 0)
        );
        
        // Aktifkan WiFi AP untuk client
        WiFi.softAP(ap_settings.ssid, ap_settings.password);
        Serial.printf("AP IP: %s\n", WiFi.softAPIP().toString().c_str());
      
        // Inisialisasi NAPT
        err_t ret = ip_napt_init(NAPT, NAPT_PORT);
        if (ret == ERR_OK) {
            ret = ip_napt_enable_no(SOFTAP_IF, 1);
            if (ret == ERR_OK) {
                Serial.println("Repeater (NAPT) Started Successfully!");
                state = true;
            }
        }
        
        if (ret != ERR_OK) {
            Serial.printf("NAPT initialization failed with error: %d\n", (int)ret);
        }
    }

    bool status(){
        return state;
    }
    
    void update_status(bool s){
        state = s;
    }
}
