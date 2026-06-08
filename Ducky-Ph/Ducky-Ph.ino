#include <BleKeyboard.h>

const int triggerButton = 0; // Your BOOT button
BleKeyboard bleKeyboard("Honeypot", "Logitech", 100);

void setup() {
  pinMode(triggerButton, INPUT_PULLUP);
  bleKeyboard.begin();
}

void loop() {
  if (bleKeyboard.isConnected()) {
    if (digitalRead(triggerButton) == LOW) {
      delay(500); 

      // 1. Wake and go to Home Screen
      // Sending 'Home' often clears any open search bars
      bleKeyboard.write(KEY_HOME); 
      delay(500);

      // 2. Open the "Search" or "Finder" (Samsung: Alt + Space)
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press(' '); 
      delay(100);
      bleKeyboard.releaseAll();
      delay(500);

      // 3. Type the "tel:" protocol
      // Many phones recognize "tel:1234..." as a command to open the dialer directly
      bleKeyboard.print("1122334455"); 
      delay(500);
      
      // 4. Press Enter to select the "Call" action
      bleKeyboard.write(KEY_RETURN);
      delay(500);

      // 5. Final Enter to confirm the dial
      bleKeyboard.write(KEY_RETURN);

      while(digitalRead(triggerButton) == LOW) { delay(10); }
    }
  }
}