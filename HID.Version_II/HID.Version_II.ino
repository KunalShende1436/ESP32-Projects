#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <vector>
#include <BleKeyboard.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> // Required Library

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialize BLE Keyboard
BleKeyboard bleKeyboard("Kunal-HID-ESP32", "SBJITMR", 100);

// WiFi Credentials
const char* ssid = "ZeroTrace-C2";
const char* password = "11111111";

AsyncWebServer server(80);

const int chipSelect = 5;
const int BTN_UP = 12;
const int BTN_DOWN = 14;
const int BTN_SELECT = 27;

std::vector<String> fileNames;
int selectedIndex = 0;
int scrollOffset = 0;
bool lastUpState = HIGH;
bool lastDownState = HIGH;
bool lastSelectState = HIGH;

// --- HTML Cyberpunk UI ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ZeroTrace C2</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { background-color: #0d0f12; color: #22d3ee; font-family: 'Courier New', monospace; text-align: center; margin: 0; padding: 20px; }
    .header { border: 2px solid #22d3ee; padding: 15px; margin-bottom: 20px; box-shadow: 0 0 15px #22d3ee; }
    h1 { margin: 0; font-size: 1.5rem; text-transform: uppercase; letter-spacing: 3px; }
    .status { margin: 10px 0; font-size: 0.9rem; color: #ff0055; }
    .status.connected { color: #00ff99; }
    .payload-card { background: #1a1d23; border: 1px solid #22d3ee33; margin: 10px auto; padding: 15px; max-width: 400px; display: flex; justify-content: space-between; align-items: center; border-radius: 5px; transition: 0.3s; }
    .payload-card:hover { border-color: #22d3ee; box-shadow: 0 0 10px #22d3ee55; }
    .btn-inject { background: transparent; border: 1px solid #22d3ee; color: #22d3ee; padding: 8px 15px; cursor: pointer; text-transform: uppercase; font-weight: bold; font-size: 0.8rem; }
    .btn-inject:active { background: #22d3ee; color: #0d0f12; }
    footer { margin-top: 30px; font-size: 0.7rem; opacity: 0.5; }
  </style>
</head><body>
  <div class="header">
    <h1>ZeroTrace - HID Framework</h1>
  </div>
  <div id="connection" class="status">CHECKING BLE STATUS...</div>
  <div id="payload-list">
    </div>
  <footer>PROTOTYPE V0.2 // SBJITMR // SECURED CHANNEL</footer>
<script>
  function updateStatus() {
    fetch('/status').then(r => r.text()).then(t => {
      const el = document.getElementById('connection');
      el.innerText = "BLE STATUS: " + t;
      el.className = t.includes("CONNECTED") ? "status connected" : "status";
    });
  }
  function loadFiles() {
    fetch('/list').then(r => r.json()).then(files => {
      let html = '';
      files.forEach(f => {
        html += `<div class="payload-card"><span>${f}</span><button class="btn-inject" onclick="inject('${f}')">Inject</button></div>`;
      });
      document.getElementById('payload-list').innerHTML = html;
    });
  }
  function inject(file) {
    fetch(`/inject?file=${file}`);
    alert("Deploying: " + file);
  }
  setInterval(updateStatus, 3000);
  loadFiles();
</script>
</body></html>)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  bleKeyboard.begin();
  if (!SD.begin(chipSelect)) { showError("SD Fail!"); return; }
  updateFileList();

  // Initialize WiFi AP
  WiFi.softAP(ssid, password);
  
  // Web Routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", bleKeyboard.isConnected() ? "CONNECTED" : "DISCONNECTED");
  });

  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "[";
    for (size_t i = 0; i < fileNames.size(); i++) {
      json += "\"" + fileNames[i] + "\"" + (i == fileNames.size() - 1 ? "" : ",");
    }
    json += "]";
    request->send(200, "application/json", json);
  });

  server.on("/inject", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasArg("file")) {
      executeHIDPayload(request->arg("file"));
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  handleMenu();
}

void updateFileList() {
  fileNames.clear();
  File root = SD.open("/");
  while (File entry = root.openNextFile()) {
    if (!entry.isDirectory()) fileNames.push_back(String(entry.name()));
    entry.close();
  }
  root.close();
}

void handleMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(bleKeyboard.isConnected() ? "BLE: ON" : "BLE: OFF");
  display.setCursor(70, 0);
  display.print("AP: ON");
  display.drawLine(0, 10, 128, 10, WHITE);

  if (selectedIndex < scrollOffset) scrollOffset = selectedIndex;
  if (selectedIndex >= scrollOffset + 5) scrollOffset = selectedIndex - 4;

  for (int i = 0; i < 5; i++) {
    int fileIdx = i + scrollOffset;
    if (fileIdx >= fileNames.size()) break;
    display.setCursor(0, 15 + (i * 10));
    display.print(fileIdx == selectedIndex ? "> " : "  ");
    display.println(fileNames[fileIdx]);
  }
  display.display();
  checkButtons();
}

void checkButtons() {
  if (digitalRead(BTN_UP) == LOW && lastUpState == HIGH) {
    selectedIndex = (selectedIndex <= 0) ? fileNames.size() - 1 : selectedIndex - 1;
    delay(150);
  }
  if (digitalRead(BTN_DOWN) == LOW && lastDownState == HIGH) {
    selectedIndex = (selectedIndex >= fileNames.size() - 1) ? 0 : selectedIndex + 1;
    delay(150);
  }
  if (digitalRead(BTN_SELECT) == LOW && lastSelectState == HIGH) {
    if (fileNames.size() > 0) executeHIDPayload(fileNames[selectedIndex]);
    delay(150);
  }
}

void executeHIDPayload(String fileName) {
  if (!bleKeyboard.isConnected()) return;
  File myFile = SD.open("/" + fileName);
  if (myFile) {
    // Keep your existing injection logic here...
    // [Insert Step 1 to Step 7 from your previous code]
    myFile.close();
  }
}

void showError(String msg) {
  display.clearDisplay();
  display.setCursor(0, 30);
  display.println(msg);
  display.display();
}