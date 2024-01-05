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

void displaySendMessage(Adafruit_ST7789 &tftDisplay, String messageType, String messageId, String dlc, uint8_t messageData[8]) {
  tftDisplay.setTextSize(2); // Set text size

  // Display menu title
  tftDisplay.setTextColor(ST77XX_WHITE); // Set text color
  tftDisplay.setCursor(80, 20); // Set cursor position
  tftDisplay.println("Send CAN Message");

  // Display message type
  tftDisplay.setTextColor(ST77XX_GREEN); // Set text color
  tftDisplay.setCursor(30, 40); // Set cursor position
  tftDisplay.print("Type: ");
  tftDisplay.println(messageType);

  // Display message ID
  tftDisplay.setTextColor(ST77XX_BLUE); // Set text color
  tftDisplay.setCursor(30, 60); // Set cursor position
  tftDisplay.print("ID: ");
  tftDisplay.println(messageId);

  // Display DLC
  tftDisplay.setTextColor(ST77XX_RED); // Set text color
  tftDisplay.setCursor(30, 80); // Set cursor position
  tftDisplay.print("DLC: ");
  tftDisplay.println(dlc);

  // Display message data
  tftDisplay.setTextColor(ST77XX_YELLOW); // Set text color
  for (int i = 0; i < 8; i++) {
    tftDisplay.setCursor(30, 100 + i*20); // Set cursor position
    tftDisplay.print("Data[");
    tftDisplay.print(i);
    tftDisplay.print("]: 0x");
    tftDisplay.println(messageData[i], HEX);
  }
}

void displayCANData(Adafruit_ST7789 &tftDisplay, int canBusSpeed, int messagesSent, int messagesReceived, bool canBusError) {
  tftDisplay.setTextSize(2); // Set text size

  // Display menu title
  tftDisplay.setTextColor(ST77XX_WHITE); // Set text color
  tftDisplay.setCursor(80, 20); // Set cursor position
  tftDisplay.println("CAN Data");
  tftDisplay.println("");

  // Display CAN bus speed
  tftDisplay.setTextColor(ST77XX_GREEN); // Set text color
  tftDisplay.setCursor(30, 40); // Set cursor position
  tftDisplay.print("Speed: ");
  tftDisplay.println(canBusSpeed);

  // Display number of messages sent
  tftDisplay.setTextColor(ST77XX_WHITE); // Set text color
  tftDisplay.setCursor(30, 60); // Set cursor position
  tftDisplay.print("Messages Sent: ");
  tftDisplay.setTextColor(ST77XX_BLUE); // Change text color for data
  tftDisplay.println(messagesSent);

  // Display number of messages received
  tftDisplay.setTextColor(ST77XX_WHITE); // Set text color
  tftDisplay.setCursor(30, 80); // Set cursor position
  tftDisplay.print("Messages Received: ");
  tftDisplay.setTextColor(ST77XX_RED); // Change text color for data
  tftDisplay.println(messagesReceived);

  // Display CAN bus status
  tftDisplay.setTextColor(canBusError ? ST77XX_YELLOW : ST77XX_CYAN); // Set text color based on CAN bus status
  tftDisplay.setCursor(30, 100); // Set cursor position
  tftDisplay.println(canBusError ? "Problema na rede CAN" : "Rede CAN normal");
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

void displaySystemInfo(Adafruit_ST7789 &tftDisplay, int batteryPercentage, bool isBluetoothMode, float batteryVoltage, unsigned long systemTimeMillis) {
  clearScreen(tftDisplay);
  tftDisplay.setTextSize(2); // Set text size

  // Display menu title
  tftDisplay.setTextColor(ST77XX_WHITE); // Set text color
  tftDisplay.setCursor(80, 20); // Set cursor position
  tftDisplay.println("System Settings");
  tftDisplay.println("");

  // Display battery percentage
  tftDisplay.setTextColor(ST77XX_GREEN); // Set text color
  tftDisplay.setCursor(30, 40); // Set cursor position
  tftDisplay.print("Battery: ");
  tftDisplay.print(batteryPercentage);
  tftDisplay.println("%");

  // Display mode
  tftDisplay.setTextColor(ST77XX_BLUE); // Set text color
  tftDisplay.setCursor(30, 60); // Set cursor position
  tftDisplay.print("Mode: ");
  tftDisplay.println(isBluetoothMode ? "Bluetooth" : "Serial");

  // Display battery voltage
  tftDisplay.setTextColor(ST77XX_RED); // Set text color
  tftDisplay.setCursor(30, 80); // Set cursor position
  tftDisplay.print("Voltage: ");
  tftDisplay.print(batteryVoltage, 3); // Limit to 3 decimal places
  tftDisplay.println("V");

  // Display system time
  tftDisplay.setTextColor(ST77XX_YELLOW); // Set text color
  tftDisplay.setCursor(30, 100); // Set cursor position
  tftDisplay.print("Time: ");
  int minutes = systemTimeMillis / 60000;
  int seconds = (systemTimeMillis % 60000) / 1000;
  tftDisplay.print(minutes);
  tftDisplay.print(":");
  if (seconds < 10) tftDisplay.print("0"); // Add leading zero for seconds < 10
  tftDisplay.println(seconds);
}
