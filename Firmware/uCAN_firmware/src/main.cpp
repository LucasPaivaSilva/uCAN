#include "libs.h"
#include "userDefs.h"

Adafruit_ST7789 tft = Adafruit_ST7789(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
QueueHandle_t encoderQueue = xQueueCreate(1, sizeof(encoderStateRotationEnum));
QueueHandle_t buttonQueue = xQueueCreate(1, sizeof(buttonStateEnum));

BluetoothSerial SerialBT;

uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
int messagesSent = 0;
int messagesReceived = 0;
bool CANEnabled = false;
bool CANTimestamp = false;
bool CANcr = false;
int CANspeed = 500;
CAN_device_t              CAN_cfg;
static uint8_t hexval[17] = "0123456789ABCDEF";

void send_canmsg(char *buf, boolean rtr, boolean ext) {
  if (!CANEnabled) {

  } else {
    CAN_frame_t tx_frame;
    int msg_id = 0;
    int msg_ide = 0;
    if (rtr) {
      if (ext) {
        sscanf(&buf[1], "%04x%04x", &msg_ide, &msg_id);
        tx_frame.FIR.B.RTR = CAN_RTR;
        tx_frame.FIR.B.FF = CAN_frame_ext;
      } else {
        sscanf(&buf[1], "%03x", &msg_id);
        tx_frame.FIR.B.RTR = CAN_RTR;
        tx_frame.FIR.B.FF = CAN_frame_std;
      }
    } else {
      if (ext) {
        sscanf(&buf[1], "%04x%04x", &msg_ide, &msg_id);
        tx_frame.FIR.B.RTR = CAN_no_RTR;
        tx_frame.FIR.B.FF = CAN_frame_ext;
      } else {
        sscanf(&buf[1], "%03x", &msg_id);
        tx_frame.FIR.B.RTR = CAN_no_RTR;
        tx_frame.FIR.B.FF = CAN_frame_std;
      }
    }
    tx_frame.MsgID = msg_ide*65536 + msg_id;
    int msg_len = 0;
    if (ext) {
      sscanf(&buf[9], "%01x", &msg_len);
    } else {
      sscanf(&buf[4], "%01x", &msg_len);
    }
    tx_frame.FIR.B.DLC = msg_len;
    int candata = 0;
    if (ext) {
      for (int i = 0; i < msg_len; i++) {
        sscanf(&buf[10 + (i*2)], "%02x", &candata);
        tx_frame.data.u8[i] = candata;
      }
    } else {
      for (int i = 0; i < msg_len; i++) {
        sscanf(&buf[5 + (i*2)], "%02x", &candata);
        tx_frame.data.u8[i] = candata;
      }
    }
    ESP32Can.CANWriteFrame(&tx_frame);
  }
} // send_canmsg()

void slcan_ack()
{
  SerialBT.write('\r');
} // slcan_ack()

void slcan_nack()
{
  SerialBT.write('\a');
} // slcan_nack()

void pars_slcancmd(char *buf)
{                           // LAWICEL PROTOCOL
  Serial.println(buf);
  switch (buf[0]) {
    case 'O':               // OPEN CAN
      CANEnabled=true;
      ESP32Can.CANInit();
      slcan_ack();
      break;
    case 'C':               // CLOSE CAN
      CANEnabled=false;
      ESP32Can.CANStop();
      slcan_ack();
      break;
    case 't':               // SEND STD FRAME
      send_canmsg(buf,false,false);
      slcan_ack();
      break;
    case 'T':               // SEND EXT FRAME
      send_canmsg(buf,false,true);
      slcan_ack();
      break;
    case 'r':               // SEND STD RTR FRAME
      send_canmsg(buf,true,false);
      slcan_ack();
      break;
    case 'R':               // SEND EXT RTR FRAME
      send_canmsg(buf,true,true);
      slcan_ack();
      break;
    case 'Z':               // ENABLE TIMESTAMPS
      switch (buf[1]) {
        case '0':           // TIMESTAMP OFF  
          CANTimestamp = false;
          slcan_ack();
          break;
        case '1':           // TIMESTAMP ON
          CANTimestamp = true;
          slcan_ack();
          break;
        default:
          break;
      }
      break;
    case 'M':               ///set ACCEPTANCE CODE ACn REG
      slcan_ack();
      break;
    case 'm':               // set ACCEPTANCE CODE AMn REG
      slcan_ack();
      break;
    case 's':               // CUSTOM CAN bit-rate
      slcan_nack();
      break;
    case 'S':               // CAN bit-rate
      switch (buf[1]) {
        case '0':           // 10k  
          slcan_nack();
          break;
        case '1':           // 20k
          slcan_nack();
          break;
        case '2':           // 50k
          slcan_nack();
          break;
        case '3':           // 100k
          CAN_cfg.speed=CAN_SPEED_100KBPS;
          CANspeed = 100;
          slcan_ack();
          break;
        case '4':           // 125k
          CAN_cfg.speed=CAN_SPEED_125KBPS;
          CANspeed = 125;
          slcan_ack();
          break;
        case '5':           // 250k
          CAN_cfg.speed=CAN_SPEED_250KBPS;
          CANspeed = 250;
         slcan_ack();
          break;
        case '6':           // 500k
          CAN_cfg.speed=CAN_SPEED_500KBPS;
          CANspeed = 500;
          slcan_ack();
          break;
        case '7': // 800k
          CAN_cfg.speed=CAN_SPEED_800KBPS;
          CANspeed = 800;
          slcan_ack();
          break;
        case '8':           // 1000k
          CAN_cfg.speed=CAN_SPEED_1000KBPS;
          CANspeed = 1000;
          slcan_ack();
          break;
        default:
          slcan_nack();
          break;
      }
      break;
    case 'F':               // STATUS FLAGS
      SerialBT.print("F00");
      slcan_ack();
      break;
    case 'V':               // VERSION NUMBER
      SerialBT.print("V1234");
      slcan_ack();
      break;
    case 'N':               // SERIAL NUMBER
      SerialBT.print("N2208");
      slcan_ack();
      break;
    case 'l':               // (NOT SPEC) TOGGLE LINE FEED ON SERIAL
      CANcr = !CANcr;
      slcan_nack();
      break;
    case 'h':               // (NOT SPEC) HELP SERIAL
      Serial.println();
      Serial.println("mintynet.com - slcan esp32");
      Serial.println();
      Serial.println("O\t=\tStart slcan");
      Serial.println("C\t=\tStop slcan");
      Serial.println("t\t=\tSend std frame");
      Serial.println("r\t=\tSend std rtr frame");
      Serial.println("T\t=\tSend ext frame");
      Serial.println("R\t=\tSend ext rtr frame");
      Serial.println("Z0\t=\tTimestamp Off");
      Serial.println("Z1\t=\tTimestamp On");
      Serial.println("snn\t=\tSpeed 0xnnk N/A");
      Serial.println("S0\t=\tSpeed 10k N/A");
      Serial.println("S1\t=\tSpeed 20k N/A");
      Serial.println("S2\t=\tSpeed 50k N/A");
      Serial.println("S3\t=\tSpeed 100k");
      Serial.println("S4\t=\tSpeed 125k");
      Serial.println("S5\t=\tSpeed 250k");
      Serial.println("S6\t=\tSpeed 500k");
      Serial.println("S7\t=\tSpeed 800k");
      Serial.println("S8\t=\tSpeed 1000k");
      Serial.println("F\t=\tFlags N/A");
      Serial.println("N\t=\tSerial No");
      Serial.println("V\t=\tVersion");
      Serial.println("-----NOT SPEC-----");
      Serial.println("h\t=\tHelp");
      Serial.print("l\t=\tToggle CR ");
      if (CANcr) {
        Serial.println("ON");
      } else {
        Serial.println("OFF");
      }
      Serial.print("CAN_SPEED:\t");
      switch(CANspeed) {
        case 100:
          Serial.print("100");
          break;
        case 125:
          Serial.print("125");
          break;
        case 250:
          Serial.print("250");
          break;
        case 500:
          Serial.print("500");
          break;
        case 800:
          Serial.print("800");
          break;
        case 1000:
          Serial.print("1000");
          break;
        default:
          break;
      }
      Serial.print("kbps");
      if (CANTimestamp) {
        Serial.print("\tT");
      }
      if (CANEnabled) {
        Serial.print("\tON");
      } else {
        Serial.print("\tOFF");
      }
      Serial.println();
      slcan_nack();
      break;
    default:
      slcan_nack();
      break;
  }
} // pars_slcancmd()

void transfer_tty2can()
{
  int ser_length;
  static char cmdbuf[32];
  static int cmdidx = 0;
  if (true) {
    if ((ser_length = SerialBT.available()) > 0) {
      for (int i = 0; i < ser_length; i++) {
        char val = SerialBT.read();
        cmdbuf[cmdidx++] = val;
        if (cmdidx == 32)
        {
          slcan_nack();
          cmdidx = 0;
        } else if (val == '\r')
        {
          cmdbuf[cmdidx] = '\0';
          pars_slcancmd(cmdbuf);
          cmdidx = 0;
        }
      }
    }
  } else {
    if ((ser_length = Serial.available()) > 0) {
      for (int i = 0; i < ser_length; i++) {
        char val = Serial.read();
        cmdbuf[cmdidx++] = val;
        if (cmdidx == 32)
        {
          slcan_nack();
          cmdidx = 0;
        } else if (val == '\r')
        {
          cmdbuf[cmdidx] = '\0';
          pars_slcancmd(cmdbuf);
          cmdidx = 0;
        }
      }
    }
  }
} // transfer_tty2can()

void transfer_can2tty()
{
  CAN_frame_t rx_frame;
  String command = "";
  long time_now = 0;
  //receive next CAN frame from queue
  if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE) {
    messagesReceived++;
    //do stuff!
    if(CANEnabled) {
      if(rx_frame.FIR.B.FF==CAN_frame_ext) {
        if (rx_frame.FIR.B.RTR==CAN_RTR) {
          command = command + "R";
        } else {
          command = command + "T";
        }
        command = command + char(hexval[ (rx_frame.MsgID>>28)&1]);
        command = command + char(hexval[ (rx_frame.MsgID>>24)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>20)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>16)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>12)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>8)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>4)&15]);
        command = command + char(hexval[ rx_frame.MsgID&15]);
        command = command + char(hexval[ rx_frame.FIR.B.DLC ]);
      } else {
        if (rx_frame.FIR.B.RTR==CAN_RTR) {
          command = command + "r";
        } else {
          command = command + "t";
        }
        command = command + char(hexval[ (rx_frame.MsgID>>8)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>4)&15]);
        command = command + char(hexval[ rx_frame.MsgID&15]);
        command = command + char(hexval[ rx_frame.FIR.B.DLC ]);
      }
      for(int i = 0; i < rx_frame.FIR.B.DLC; i++){
        command = command + char(hexval[ rx_frame.data.u8[i]>>4 ]);
        command = command + char(hexval[ rx_frame.data.u8[i]&15 ]);
        //printf("%c\t", (char)rx_frame.data.u8[i]);
      }
    if (CANTimestamp) {
      time_now = millis() % 60000;
      command = command + char(hexval[ (time_now>>12)&15 ]);
      command = command + char(hexval[ (time_now>>8)&15 ]);
      command = command + char(hexval[ (time_now>>4)&15 ]);
      command = command + char(hexval[ time_now&15 ]);
    }
    command = command + '\r';
    SerialBT.print(command);
    if (CANcr) Serial.println("");
    }
  }
} // transfer_can2tty()

void setupCan()
{
  CAN_cfg.speed=CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = PIN_CAN_TX;
  CAN_cfg.rx_pin_id = PIN_CAN_RX;
  CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
  ESP32Can.CANInit();
}

void IRAM_ATTR updateEncoder() {
  int MSB = digitalRead(pinA); //MSB = most significant bit
  int LSB = digitalRead(pinB); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (encoderLastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderCallCounter ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderCallCounter --;

  if(encoderCallCounter >= 4) {
    encoderPos++;
    encoderCallCounter = 0;
  } else if(encoderCallCounter <= -4) {
    encoderPos--;
    encoderCallCounter = 0;
  }

  encoderLastEncoded = encoded; //store this value for next time
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
    vTaskDelay(pdMS_TO_TICKS(5)); // Check encoder every 5ms
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
          displayCANData(tft, 500, messagesSent, messagesReceived, false);
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
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
          displaySendMessage(tft, "CAN_frame_std", "0x123", "8", data);  
          if (buttonState == BUTTON_SHORT_PRESS) {
            CAN_frame_t testFrame;
            testFrame.FIR.B.FF = CAN_frame_std;
            testFrame.MsgID = 0x123;
            testFrame.FIR.B.DLC = 8;
            testFrame.data.u8[0] = 0x01;
            testFrame.data.u8[1] = 0x02;
            testFrame.data.u8[2] = 0x03;
            testFrame.data.u8[3] = 0x04;
            testFrame.data.u8[4] = 0x05;
            testFrame.data.u8[5] = 0x06;
            testFrame.data.u8[6] = 0x07;
            testFrame.data.u8[7] = 0x08;

            ESP32Can.CANWriteFrame(&testFrame);
            messagesSent++;
          }
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
          }
          break;
        // --------------------------------------------------------------------------- //
        // Call function to display system settings
        case MENU_SYSTEM_SETTINGS:
          displaySystemInfo(tft, 70, true, 4.096, millis());
          if (buttonState == BUTTON_LONG_PRESS) {
            currentScreen = MENU_MAIN;
            menuForceUpdate = true;
          }
          break;
      }
    }
    encoderState = ENCODER_IDLE;
    buttonState = BUTTON_IDLE;
    if (menuForceUpdate) { // If menuForceUpdate is true, clear screen and latter update it
      clearScreen(tft); 
    }
    vTaskDelay(pdMS_TO_TICKS(20)); // Delay to reduce CPU usage, adjust as needed
  }
}

void canTask(void * parameter) {
  for(;;) {
    transfer_can2tty();
    transfer_tty2can(); 
    vTaskDelay(pdMS_TO_TICKS(20)); // Delay to reduce CPU usage, adjust as needed
  }
}


void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void setup() {
  Serial.begin(115200);
  Serial.print(F("Starting...\n"));


  tft.init(TFT_DISPLAY_WIDTH, TFT_DISPLAY_HEIGHT, SPI_MODE3);           // Init ST7789 320x240
  tft.setRotation(1);
  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);

  setupCan();

  SerialBT.begin("uCANTest1");
  Serial.println("BT Switch ON");

  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_A), buttonAISR, CHANGE);

  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), updateEncoder, CHANGE);

  xTaskCreate(buttonATask, "ButtonATask", 1000, NULL, 1, NULL);
  xTaskCreate(encoderTask, "EncoderTask", 1000, NULL, 1, NULL);
  xTaskCreate(interfaceTask, "InterfaceTask", 10000, NULL, 1, NULL);
  xTaskCreate(canTask, "CanTask", 10000, NULL, 1, NULL);

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  Serial.println(time, DEC);

  Serial.print(F("Setup done!\n"));
  SerialBT.println("Setup done!");
}