//* USB Dial for Mediachance Multi Keyboard Macros 
//* Copyright 2020 Mediachance.com Licenced under the GNU GPL Version 3.
//* By deault the encoder is mapped to [ and ] which correspond to Photoshop Brush size down and up.
//* The switch is mapped to spacebar - and it is held when switch is held down
//* With Multi-Keyboard macros this all can be then mapped to whatever key combination you need

//* Uses: Rotary encoder handler code by Ben Buxton. 
                            
#include <Keyboard.h>

// Oscars notes:
// Works with Arduino PRO Micro (32U4), you need to download the sparkfun AVR boards in Board Manager and make sure you use the correct board (3V or 5V)
// To program pro micro, you likely need to also add reset switch between RST and GND 
// When in sketch mode the PRO micro can't upload the sketch - you can't even see the port, it needs to be in bootloader mode and that's what the Reset switch is for - but there is a good timing involved.

// On sparkfun site they say the pro micro will reset itself automatically into bootloader upon clicking upload. It doesn't do that on the clone I've got from ebay (that runs Leonardo bootloader) and the version of arduino IDE
// so you need to always reset it into bootloader with the Reset switch before uploading.
// I usually click Upload, then watch the status bar, it will start compiling. When it says Uploading.. I will hit the Reset switch to start the board in bootloader.
// more info: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide#troubleshooting-and-faq
// also if you plug it to usb 3.0 the arduino IDE may not recognize the port - try to plug it in  USB 2.0 port. If you don't have one and old USB hub will work great as a USB 2.0 port.


//************** Rotary encoder handler **************
// Enable this to emit codes twice per step 

// Oscars notes:
// KEYES 040 I've got from ebay has detents at full and half positions of quadrature (30 detents/ 15 cycles) and so it needs to be set as HALF STEP
// Many other encoders are full step (all four 1/4 steps per detent) - they have a detent only at full position of quadrature (ex: 24 detents/24 cycles)

// !!!if it takes two clicks to issue an action then enable HALF_STEP
//#define HALF_STEP

// Enable weak pullups
// Oscars note: on the Keyes 040 I ignore the external pullup resistors (simply don't wire the + wire)
// this makes it more universal and simpler
// we will enable weak pullups on the arduino itself

#define ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwisestep.
#define DIR_CCW 0x20

#define R_START 0x0


unsigned char state;
unsigned char pin1;
unsigned char pin2; 

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif


void Rotary(char _pin1, char _pin2) {
  // Assign variables.
  pin1 = _pin1;
  pin2 = _pin2;
  // Set pins to input.
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
#ifdef ENABLE_PULLUPS
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
#endif
  // Initialise state.
  state = R_START;
}

unsigned char process() {
  // Grab state of input pins.
  unsigned char pinstate = (digitalRead(pin2) << 1) | digitalRead(pin1);
  // Determine new state from the pins and state table.
  state = ttable[state & 0xf][pinstate];
  // Return emit bits, ie the generated event.
  return state & 0x30;
}

//************************* ACTUAL CODE **********************************

// arduino pro micro
// you can easily have 2 encoders if you want
//pin 3 maps to interrupt 0 (INT0), pin 2 is interrupt 1 (INT1), 
//pin 0 is interrupt 2 (INT2), pin 1 is interrupt 3 (INT3), 
//and pin 7 is interrupt 4 (INT6).

int buttonState = HIGH;
int lastButtonState = HIGH;
int rotaryFlag = 0;

void setup() {

  // rottary encoder switch
   
  pinMode(4,INPUT);
 
#ifdef ENABLE_PULLUPS  
  digitalWrite(4, HIGH); 
#endif

  // encoder plugged to 2,3 that correspond to interupts 0,1
   Rotary(2, 3);
   attachInterrupt(0, rotate, CHANGE);
   attachInterrupt(1, rotate, CHANGE);

}

const long intervalDebouce = 40;
unsigned long previousMillis = 0;  

void rotate() {
  unsigned char result = process();
  if (result == DIR_CW) 
  {
    rotaryFlag = DIR_CW;
  } 
  else if (result == DIR_CCW) 
  {
    rotaryFlag = DIR_CCW;
  }
}


void loop() {

   unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalDebouce) 
  {

    buttonState = digitalRead(4);

    // press - release encoder push button
    // when holding the push buttown down, also the key will be held down. 
   
    if (buttonState==LOW && lastButtonState == HIGH)
    {
      // when pressed encoder switch
       Keyboard.press(' ');
    }

    if (buttonState==HIGH && lastButtonState == LOW)
    {
      // when pressed encoder switch
       Keyboard.release(' ');
    }
    
    if (buttonState!=lastButtonState)
    {
      // DEBOUNCE button
       previousMillis = millis(); 
    }

    lastButtonState = buttonState; 

  
    if (rotaryFlag == DIR_CW)
    {
 
        Keyboard.press('.');
        delay(70);
        Keyboard.releaseAll();
        rotaryFlag = 0;

        previousMillis = millis();
    }
 
    if (rotaryFlag == DIR_CCW)
    {

        Keyboard.press(',');
        delay(70);
        Keyboard.releaseAll();
        rotaryFlag = 0;

        previousMillis = millis();
    }
  }
         
}
