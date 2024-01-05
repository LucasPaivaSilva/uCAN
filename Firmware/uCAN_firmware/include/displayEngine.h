// menu.h
#ifndef MENU_H
#define MENU_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

void displayMainMenu(Adafruit_ST7789 &tftDisplay, int selectedOption);
void displaySystemInfo(Adafruit_ST7789 &tftDisplay, int batteryPercentage, bool isBluetoothMode, float batteryVoltage, unsigned long systemTimeMillis);
void displaySendMessage(Adafruit_ST7789 &tftDisplay, String messageType, String messageId, String dlc, uint8_t messageData[8]);
void displayCANData(Adafruit_ST7789 &tftDisplay, int canBusSpeed, int messagesSent, int messagesReceived, bool canBusError);
void testroundrects(Adafruit_ST7789 &tftDisplay);
void clearScreen(Adafruit_ST7789 &tftDisplay);
void displayCANData(Adafruit_ST7789 &tftDisplay);

#endif // MENU_H