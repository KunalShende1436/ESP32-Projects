#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int chipSelect = 5; 
String marqueeText = ""; // Variable to store text from SD
int xPos = SCREEN_WIDTH;  // Start position at the right edge

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;); 
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2); // Larger text for the marquee
  display.setTextWrap(false); // CRITICAL: Prevents text from jumping to next line

  // Initialize SD Card
  if (!SD.begin(chipSelect)) {
    display.setTextSize(1);
    display.println("SD Init Failed!");
    display.display();
    return;
  }

  // Open the file as seen in your photo
  File myFile = SD.open("/msg.txt.txt"); 

  if (myFile) {
    while (myFile.available()) {
      marqueeText += (char)myFile.read(); // Read file into the string
    }
    myFile.close();
  } else {
    marqueeText = "File Not Found!";
  }
}

void loop() {
  display.clearDisplay();
  
  // Set the cursor at the moving X position
  display.setCursor(xPos, 25);
  display.print(marqueeText);
  display.display();

  // Move text to the left
  xPos = xPos - 2; 

  // Reset if the text has completely scrolled off to the left
  // (Approx. 12 pixels per character at Size 2)
  int textWidth = marqueeText.length() * 12; 
  if (xPos < -textWidth) {
    xPos = SCREEN_WIDTH;
  }

  delay(30); // Speed control: Lower is faster
}