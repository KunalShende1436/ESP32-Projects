#include <BleKeyboard.h>

// The 'BOOT' button on the DevKit V1 is on GPIO 0
const int triggerButton = 0; 
BleKeyboard bleKeyboard("Honeypot", "Logitech", 100);

void setup() {
  Serial.begin(115200);
  // Initialize the button with an internal pull-up resistor
  pinMode(triggerButton, INPUT_PULLUP); 
  bleKeyboard.begin();
  Serial.println("Waiting for Bluetooth connection...");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    // Check if the BOOT button is pressed (it goes LOW when pressed)
    if (digitalRead(triggerButton) == LOW) {
      Serial.println("Button Pressed! Launching Payload...");
      
      delay(500); // Short delay to prevent accidental double-triggers

      // Step 1: Open 'Run' (Win + R)
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.press('r');
      delay(100);
      bleKeyboard.releaseAll();
      delay(600);

      // Step 2: Open Notepad
      bleKeyboard.print("notepad");
      bleKeyboard.write(KEY_RETURN);
      delay(1000); 

      // Step 3: Write your sentence
      bleKeyboard.print("I can see you!!");
      
      // Wait for button release to avoid repeating
      while(digitalRead(triggerButton) == LOW) { delay(10); }
    }
  }
}