#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

void displayMainMenu(Adafruit_ST7789 &tftDisplay, int selectedOption) {
  //tftDisplay.fillScreen(ST77XX_BLACK); // Clear screen

  tftDisplay.setTextColor(ST77XX_WHITE); // Set default text color
  tftDisplay.setTextSize(2); // Set text size

  // Display menu title
  tftDisplay.setCursor(80, 20); // Set cursor position
  tftDisplay.println("uCAN32");

  // Display menu options
  tftDisplay.setCursor(30, 60);
  if (selectedOption == 1) tftDisplay.setTextColor(ST77XX_YELLOW);
  tftDisplay.println("1. CAN Data");
  tftDisplay.setTextColor(ST77XX_WHITE); // Reset text color

  tftDisplay.setCursor(30, 100);
  if (selectedOption == 2) tftDisplay.setTextColor(ST77XX_YELLOW);
  tftDisplay.println("2. Show CAN Messages");
  tftDisplay.setTextColor(ST77XX_WHITE); // Reset text color

  tftDisplay.setCursor(30, 140);
  if (selectedOption == 3) tftDisplay.setTextColor(ST77XX_YELLOW);
  tftDisplay.println("3. Send CAN Message");
  tftDisplay.setTextColor(ST77XX_WHITE); // Reset text color

  tftDisplay.setCursor(30, 180);
  if (selectedOption == 4) tftDisplay.setTextColor(ST77XX_YELLOW);
  tftDisplay.println("4. System Settings");
  tftDisplay.setTextColor(ST77XX_WHITE); // Reset text color
}

void displayCANData(Adafruit_ST7789 &tftDisplay) {
  tftDisplay.setTextColor(ST77XX_WHITE); // Set default text color
  tftDisplay.setTextSize(2); // Set text size

  // Display menu title
  tftDisplay.setCursor(80, 20); // Set cursor position
  tftDisplay.println("CAN Data");
}

void testroundrects(Adafruit_ST7789 &tftDisplay) {
  tftDisplay.fillScreen(ST77XX_BLACK);
  uint16_t color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tftDisplay.width()-2;
    int h = tftDisplay.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tftDisplay.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void clearScreen(Adafruit_ST7789 &tftDisplay) {
  tftDisplay.fillScreen(ST77XX_BLACK);
}