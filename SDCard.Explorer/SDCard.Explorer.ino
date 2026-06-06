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

// Variables
std::vector<String> fileNames;
int selectedIndex = 0;
bool isDisplayingFile = false;
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

// --- Logic Functions ---

void updateFileList() {
  fileNames.clear();
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
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

  // Display files (showing 3 at a time for clarity)
  for (int i = 0; i < fileNames.size(); i++) {
    if (i == selectedIndex) display.print("> ");
    else display.print("  ");
    display.println(fileNames[i]);
  }
  display.display();

  // Button Handling
  if (digitalRead(BTN_UP) == LOW) {
    selectedIndex--;
    if (selectedIndex < 0) selectedIndex = fileNames.size() - 1; // Wrap to bottom
    delay(200);
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    selectedIndex++;
    if (selectedIndex >= fileNames.size()) selectedIndex = 0; // Wrap to top
    delay(200);
  }
  if (digitalRead(BTN_SELECT) == LOW) {
    loadFile(fileNames[selectedIndex]);
    delay(200);
  }
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
    xPos = SCREEN_WIDTH; // Reset marquee position
  }
}

void handleMarquee() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(xPos, 25);
  display.print(marqueeText);
  display.display();

  xPos -= 3; // Scroll speed

  int textWidth = marqueeText.length() * 12;
  if (xPos < -textWidth) {
    xPos = SCREEN_WIDTH;
  }

  // Press Select to go back to Menu
  if (digitalRead(BTN_SELECT) == LOW) {
    isDisplayingFile = false;
    delay(200);
  }
}

void showError(String msg) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(msg);
  display.display();
}