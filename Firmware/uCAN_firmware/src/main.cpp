#include "libs.h"
#include "userDefs.h"

Adafruit_ST7789 tft = Adafruit_ST7789(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
QueueHandle_t encoderQueue = xQueueCreate(1, sizeof(encoderStateRotationEnum));
QueueHandle_t buttonQueue = xQueueCreate(1, sizeof(buttonStateEnum));


void IRAM_ATTR updateEncoder() {
  int MSB = digitalRead(pinA); //MSB = most significant bit
  int LSB = digitalRead(pinB); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) callCounter ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) callCounter --;

  if(callCounter >= 4) {
    encoderPos++;
    callCounter = 0;
  } else if(callCounter <= -4) {
    encoderPos--;
    callCounter = 0;
  }

  lastEncoded = encoded; //store this value for next time
}

void IRAM_ATTR buttonAISR() {
  TickType_t now = xTaskGetTickCountFromISR();
  if (now - lastInterruptTimeButtonA > DEBOUNCE_DELAY) {
    buttonStateA = digitalRead(PIN_BUTTON_A);
  }
  lastInterruptTimeButtonA = now;
}

void encoderTask(void * parameter) {
  encoderStateRotationEnum encoderState = ENCODER_IDLE;
  for (;;) {
    static int lastReportedPos = 0; // change management
    if (lastReportedPos != encoderPos) {
      Serial.println(encoderPos);
      if (lastReportedPos > encoderPos) {
        // Encoder rotated left
        encoderState = ENCODER_LEFT;
      } else {
        // Encoder rotated right
        encoderState = ENCODER_RIGHT;
      }
      lastReportedPos = encoderPos;
      // Send encoder state to queue
      xQueueSend(encoderQueue, &encoderState, portMAX_DELAY);
    }
    vTaskDelay(pdMS_TO_TICKS(5)); // Check encoder every 10ms
  }
}

void buttonATask(void * parameter) {
  buttonStateEnum buttonState = BUTTON_IDLE;
  for (;;) {
    if (buttonStateA == LOW) {
      TickType_t pressTime = xTaskGetTickCount();
      while (buttonStateA == LOW) {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
      TickType_t releaseTime = xTaskGetTickCount();
      if ((releaseTime - pressTime) > LONG_PRESS_DELAY) {
        Serial.println(F("Long press"));
        buttonState = BUTTON_LONG_PRESS;
        xQueueSend(buttonQueue, &buttonState, portMAX_DELAY);
      } else {
        // The button was pressed and released quickly
        Serial.println(F("Short press"));
        buttonState = BUTTON_SHORT_PRESS;
        xQueueSend(buttonQueue, &buttonState, portMAX_DELAY);
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
  menuState currentScreen = MENU_MAIN;
  encoderStateRotationEnum encoderState = ENCODER_IDLE;
  buttonStateEnum buttonState = BUTTON_IDLE;

  bool menuForceUpdate = false;
  uint8_t menuMainSelectedOption = 1;
  displayMainMenu(tft, menuMainSelectedOption);
  for(;;) {
    // Check encoder queue for new encoder state or new button state
    if ((xQueueReceive(encoderQueue, &encoderState, 0) == pdTRUE) || menuForceUpdate || (xQueueReceive(buttonQueue, &buttonState, 0) == pdTRUE)) {
      menuForceUpdate = false;
      switch (currentScreen) {
        // --------------------------------------------------------------------------- //
        // Call function to display main menu
        case MENU_MAIN:
          if (encoderState == ENCODER_LEFT) {
            if (menuMainSelectedOption > 1) {
              menuMainSelectedOption--;
            }
          } else if (encoderState == ENCODER_RIGHT) {
            if (menuMainSelectedOption < 4) {
              menuMainSelectedOption++;
            }
          }
          displayMainMenu(tft, menuMainSelectedOption);
          if (buttonState == BUTTON_SHORT_PRESS) {
            menuForceUpdate = true;
            clearScreen(tft);
            switch (menuMainSelectedOption) {
              case 1:
                currentScreen = MENU_CAN_DATA;
                break;
              case 2:
                currentScreen = MENU_CAN_MESSAGES;
                break;
              case 3:
                currentScreen = MENU_CAN_SENDMESSAGE;
                break;
              case 4:
                currentScreen = MENU_SYSTEM_SETTINGS;
                break;
            }
          }
          break;
        // --------------------------------------------------------------------------- //
        // Call function to display CAN data
        case MENU_CAN_DATA:
          displayCANData(tft);
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
            clearScreen(tft);
          }
          break;
        // --------------------------------------------------------------------------- //
        // Call function to display CAN messages
        case MENU_CAN_MESSAGES:
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
          }
          break;
        // --------------------------------------------------------------------------- //
        // Call function to display CAN send message screen
        case MENU_CAN_SENDMESSAGE:
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
          }
          break;
        // --------------------------------------------------------------------------- //
        // Call function to display system settings
        case MENU_SYSTEM_SETTINGS:
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
          }
          break;
      }
    }
    encoderState = ENCODER_IDLE;
    buttonState = BUTTON_IDLE;
    vTaskDelay(pdMS_TO_TICKS(5)); // Delay to reduce CPU usage, adjust as needed
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
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

  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), updateEncoder, CHANGE);

  xTaskCreate(buttonATask, "ButtonATask", 1000, NULL, 1, NULL);
  xTaskCreate(encoderTask, "EncoderTask", 1000, NULL, 1, NULL);
  xTaskCreate(interfaceTask, "InterfaceTask", 10000, NULL, 1, NULL);

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
}