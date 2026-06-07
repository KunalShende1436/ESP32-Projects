#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Using your current working I2C setup
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int xPos = SCREEN_WIDTH; // Start text off-screen to the right

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }
  
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setTextWrap(false); // Crucial for marquee effect
}

void loop() {
  display.clearDisplay();
  
  // Set position (x, y)
  display.setCursor(xPos, 25);
  display.print("Hey! DCEKKA");
  
  display.display();
  
  // Move text to the left
  xPos = xPos - 2; 

  // Reset position when text is fully off-screen to the left
  // (Adjust -150 based on your message length)
  if(xPos < -150) { 
    xPos = SCREEN_WIDTH; 
  }

  delay(30); // Adjust delay to change speed (lower = faster)
}