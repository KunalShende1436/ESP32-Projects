#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <vector>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pin Definitions
const int chipSelect = 5;
const int BTN_UP = 12;
const int BTN_DOWN = 14;
const int BTN_SELECT = 27;

// Menu Variables
std::vector<String> fileNames;
int selectedIndex = 0;
int scrollOffset = 0; // For handling long lists
bool isDisplayingFile = false;

// Button State Variables (Debouncing)
bool lastUpState = HIGH;
bool lastDownState = HIGH;
bool lastSelectState = HIGH;

// Marquee Variables
String marqueeText = "";
int xPos = SCREEN_WIDTH;

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

  if (!SD.begin(chipSelect)) {
    showError("SD Init Failed!");
    return;
  }

  updateFileList();
}

void loop() {
  if (!isDisplayingFile) {
    handleMenu();
  } else {
    handleMarquee();
  }
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
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("--- SD Explorer ---");
  display.drawLine(0, 10, 128, 10, WHITE);

  // Scrolling Window Logic (Displays 5 files at a time)
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

  // --- Improved Button Logic (State Change Detection) ---
  bool currentUp = digitalRead(BTN_UP);
  bool currentDown = digitalRead(BTN_DOWN);
  bool currentSelect = digitalRead(BTN_SELECT);

  // UP Button
  if (currentUp == LOW && lastUpState == HIGH) {
    selectedIndex--;
    if (selectedIndex < 0) selectedIndex = fileNames.size() - 1;
    delay(50); 
  }
  lastUpState = currentUp;

  // DOWN Button
  if (currentDown == LOW && lastDownState == HIGH) {
    selectedIndex++;
    if (selectedIndex >= fileNames.size()) selectedIndex = 0;
    delay(50);
  }
  lastDownState = currentDown;

  // SELECT Button
  if (currentSelect == LOW && lastSelectState == HIGH) {
    if (fileNames.size() > 0) {
      loadFile(fileNames[selectedIndex]);
    }
    delay(50);
  }
  lastSelectState = currentSelect;
}

void loadFile(String fileName) {
  marqueeText = "";
  File myFile = SD.open("/" + fileName);
  if (myFile) {
    while (myFile.available()) {
      marqueeText += (char)myFile.read();
    }
    myFile.close();
    isDisplayingFile = true;
    xPos = SCREEN_WIDTH; 
  }
}

void handleMarquee() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(xPos, 25);
  display.print(marqueeText);
  display.display();

  xPos -= 4; // Scroll speed

  // Approximate width calculation (size 2 text is ~12px wide per char)
  int textWidth = marqueeText.length() * 12;
  if (xPos < -textWidth) xPos = SCREEN_WIDTH;

  // Back to Menu on Select
  bool currentSelect = digitalRead(BTN_SELECT);
  if (currentSelect == LOW && lastSelectState == HIGH) {
    isDisplayingFile = false;
    delay(50);
  }
  lastSelectState = currentSelect;
}

void showError(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(msg);
  display.display();
}