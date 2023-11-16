#include "libs.h"
#include "userDefs.h"

Adafruit_ST7789 tft = Adafruit_ST7789(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);


void IRAM_ATTR buttonAISR() {
  TickType_t now = xTaskGetTickCountFromISR();
  if (now - lastInterruptTimeButtonA > DEBOUNCE_DELAY) {
    buttonStateA = digitalRead(PIN_BUTTON_A);
  }
  lastInterruptTimeButtonA = now;
}

void buttonATask(void * parameter) {
  for (;;) {
    if (buttonStateA == LOW) {
      TickType_t pressTime = xTaskGetTickCount();
      while (buttonStateA == LOW) {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
      TickType_t releaseTime = xTaskGetTickCount();
      if ((releaseTime - pressTime) > LONG_PRESS_DELAY) {
        // The button was held down for a long time
      } else {
        // The button was pressed and released quickly
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // Check button every 10ms
  }
}

void interfaceTask(void * parameter) {
  // Menu structure
  // Main menu (0): contains option 1, option 2, option 3, option 4
  // Option 1  (1): show CAN bus data (number of messages, speed, erros, etc)
  // Option 2  (2): show CAN messages 
  // Option 3  (3): send CAN messages
  // Option 4  (4): about page/random info
  menuState systemState = MENU_MAIN;
  for (;;) {
    
  }
}



void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void setup() {
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));



  tft.init(TFT_DISPLAY_WIDTH, TFT_DISPLAY_HEIGHT, SPI_MODE3);           // Init ST7789 320x240
  tft.setRotation(3);
  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);

  Serial.println(F("Initialized"));

  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_A), buttonAISR, CHANGE);

  xTaskCreate(buttonATask, "ButtonATask", 1000, NULL, 1, NULL);

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);
}