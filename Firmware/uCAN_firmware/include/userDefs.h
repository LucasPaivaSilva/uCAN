

/* ---- PIN DEFINITIONS ---- */
// PIN DEFINITIONS FOR THE CAN BUS

// PIN DEFINITIONS FOR THE LCD
#define PIN_TFT_CS         4         // Chip select control pin
#define PIN_TFT_RST        -1        // Display Reset pin (-1 if unused/tied to microcontroller reset)
#define PIN_TFT_DC         2         // Data Command control pin
#define TFT_DISPLAY_WIDTH  240
#define TFT_DISPLAY_HEIGHT 320

/* ---- Button Definitions ---- */

#define PIN_BUTTON_A 5
volatile int buttonStateA = HIGH;
volatile TickType_t lastInterruptTimeButtonA = 0;

#define DEBOUNCE_DELAY pdMS_TO_TICKS(50)
#define LONG_PRESS_DELAY pdMS_TO_TICKS(1000)


/* ---- Menu Definitions ---- */
enum menuState{
    MENU_MAIN,
    MENU_CAN_DATA,
    MENU_CAN_MESSAGES,
    MENU_CAN_SENDMESSAGE,
    MENU_SYSTEM_SETTINGS,
};

/* ---- Encoder Definitions ---- */
#define PIN_ENCODER_A 17
#define PIN_ENCODER_B 16

volatile int encoderStateA = HIGH;
volatile int encoderStateB = HIGH;
volatile int encoderPosition = 0;