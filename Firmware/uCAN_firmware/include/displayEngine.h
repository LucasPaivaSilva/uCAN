// menu.h
#ifndef MENU_H
#define MENU_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

void displayMainMenu(Adafruit_ST7789 &tftDisplay, int selectedOption);
void testroundrects(Adafruit_ST7789 &tftDisplay);
void clearScreen(Adafruit_ST7789 &tftDisplay);
void displayCANData(Adafruit_ST7789 &tftDisplay);

#endif // MENU_H