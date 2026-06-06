#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <vector>
#include <BleKeyboard.h> // Required Library

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialize BLE Keyboard (Name, Manufacturer, Battery Level)
BleKeyboard bleKeyboard("Kunal-HID-ESP32", "SBJITMR", 100);

// Pin Definitions
const int chipSelect = 5;
const int BTN_UP = 12;
const int BTN_DOWN = 14;
const int BTN_SELECT = 27;

// Menu Variables
std::vector<String> fileNames;
int selectedIndex = 0;
int scrollOffset = 0; 
bool isTransmitting = false;

// Button State Variables
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

  // Start BLE
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
  
  // Status Bar
  display.setCursor(0, 0);
  if(bleKeyboard.isConnected()) {
    display.print("BLE: CONNECTED");
  } else {
    display.print("BLE: DISCONNECTED");
  }
  display.drawLine(0, 10, 128, 10, WHITE);

  // Scroll Window Logic
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
    showError("Connect BLE first!");
    delay(2000);
    return;
  }

  File myFile = SD.open("/" + fileName);
  if (myFile) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("OPENING FILE...");
    display.println(fileName);
    display.display();

    // STEP 1: Open 'Run' dialog (Windows + R)
    bleKeyboard.press(KEY_LEFT_GUI);
    bleKeyboard.press('r');
    delay(200);
    bleKeyboard.releaseAll();
    delay(500);

    // STEP 2: Type 'notepad' and Enter
    bleKeyboard.print("notepad");
    bleKeyboard.write(KEY_RETURN);
    delay(1000); 

    // STEP 3: Type file contents character by character
    while (myFile.available()) {
      char c = myFile.read();
      bleKeyboard.write(c);
      // Small delay prevents the laptop buffer from overflowing
      delay(5); 
    }
    myFile.close();

    display.println("SUCCESS!");
    display.display();
    delay(2000);
  } else {
    showError("File Open Error!");
  }
}

void showError(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println(msg);
  display.display();
}