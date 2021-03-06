/*******************************************************************
 * A multi-mode Macro keyboard with Arduino Pro Micro using      
 * row column matrix.
 * This is a blank sketch to understand key layout and assignments. 
 * (c) 2020 Ryan Bates 
 * Modified for KSP by P1nwheel
 *******************************************************************/
// ----------------------------
// Standard Libraries
// ----------------------------

#include "Keyboard.h"
// Library with a lot of the HID definitions and methods
// Can be useful to take a look at it see whats available
// https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------
#include <Encoder.h> //Library for simple interfacing with encoders (up to two)
//low performance ender response, pins do not have interrupts
Encoder RotaryEncoderA(14, 15); //the LEFT encoder (encoder A)
Encoder RotaryEncoderB(10, 16);  //the RIGHT encoder (encoder B)

#include <Keypad.h>
// This library is for interfacing with the 4x4 Matrix
// 
// Can be installed from the library manager, search for "keypad"
// and install the one by Mark Stanley and Alexander Brevig
// https://playground.arduino.cc/Code/Keypad/

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns




char keys[ROWS][COLS] = {
  {'1', '2', '3', '4'},  // again the keyboard hardware is not a 4x4 grid... 
  {'5', '6', '7', '8'},
  {'9', 'A', '0', 'C'},  //"This won't confuse anyone."
  {'B', 'D', 'E', 'F'},
};
// The library will return the character inside this array when the appropriate
// button is pressed; then look for that case statement. This is the key assignment lookup table.
// It looks funny because the 4x4 wiring (logic) is fudged to match an asymetical layout for 16 keys.
// Meaning, the hardware is not a 4x4 row column layout, but the wiring/ software is.
//          Actual layout is :
//     |-----------------------------|
//     |   [16*/15*]  [14]  [15]     |     *TRS breakout connection
//     |        [10] [11] [12] [13]  |
//     |       [ 6] [ 7] [ 8] [ 9]   |
//     |  [ 1] [ 2] [ 3] [ 4] [ 5]   |
//     |-----------------------------|



// Variables will change:
int modePushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

long positionEncoderA  = -999; //encoderA LEFT position variable
long positionEncoderB  = -999; //encoderB RIGHT position variable

const int ModeButton = A0;    // the pin that the Modebutton is attached to
const int Mode1= A2;
const int Mode2= A3; //Mode status LEDs

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9 }; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  pinMode(ModeButton, INPUT_PULLUP);  // initialize the button pin as a input:  
  Serial.begin(9600); // initialize serial communication:
  pinMode(Mode1,OUTPUT); digitalWrite(Mode1,LOW);
  pinMode(Mode2,OUTPUT); digitalWrite(Mode2,LOW);
  
  Serial.begin(9600);
  Keyboard.begin();
}


void loop() {
char key = keypad.getKey();
      encoderA();
      encoderB();
  
checkModeButton();

  switch (modePushCounter) { // switch between keyboard configurations:
    case 0:    // KSP Launch & Injection Configuration / Pitch - Yaw - Roll

      digitalWrite(Mode1,LOW); digitalWrite(Mode2,LOW); //indicate what mode is loaded
       if (key) {
    Serial.println(key);
    switch (key) {
      case '1':
        Keyboard.press(KEY_BACKSPACE); //Abort Button & Chute stages with delay
        delay(700);
        Keyboard.press(' ');
        delay(700);
        Keyboard.press(' ');
        delay(700);
        Keyboard.press(' '); break;
      case '2': 
        Keyboard.press('r'); break; //RCS Toggle
      case '3':
        Keyboard.press('a'); break; //Yaw Left
      case '4':
        Keyboard.press('s'); break; //Pitch Down
      case '5':
        Keyboard.press('d'); break; //Yaw Right
      case '6':
        Keyboard.press('t'); break; //SAS Toggle
      case '7':
        Keyboard.press('q'); break; //Roll CCW
      case '8':
        Keyboard.press('w'); break; //Pitch Up
      case '9':
        Keyboard.press('e'); break; //Roll CW
      case '0':
        Keyboard.press('x'); break; //Cut Throttle
      case 'A': 
        Keyboard.press('z'); break; //Full Throttle
      case 'B':
        Keyboard.press(' '); break; //Stage
      case 'C':
        Keyboard.press('/'); break; //Cut time drift
      case 'D':
        Keyboard.press('v'); break; //Toggle Camera mode
      case 'E':
        Keyboard.press('m'); break; //Toggle map view
      case 'F': 
        Keyboard.println("Alpha key16"); break;
    }
    delay(100); Keyboard.releaseAll(); // this releases the buttons 
  }
      break;
      
    case 1:    // KSP Orbital Maneuvering Configuration & RCS Translation 
      digitalWrite(Mode1,HIGH); digitalWrite(Mode2,LOW);
      if (key) {
    Serial.println(key);
    switch (key) {
      case '1': 
        Keyboard.press(' '); break; // Stage
      case '2': 
        Keyboard.press('r'); break; // RCS Toggle
      case '3':
        Keyboard.press('j'); break; // RCS Left
      case '4':
        Keyboard.press('n'); break; // RCS Back
      case '5':
        Keyboard.press('l'); break; // RCS Right
      case '6':
        Keyboard.press('t'); break; // SAS Toggle
      case '7':
        Keyboard.press('i'); break; // RCS Down
      case '8':
        Keyboard.press('h'); break; // RCS Forward
      case '9':
        Keyboard.press('k'); break; // RCS Up
      case '0':
        Keyboard.press('x'); break; // Cut Throttle
      case 'A': 
        Keyboard.press('z'); break; // Full Throttle
      case 'B':
        Keyboard.press(KEY_RIGHT_SHIFT);
        Keyboard.press('l'); break; // Lock Staging
      case 'C':
        Keyboard.press('/'); break; // Quit Time Drift
      case 'D':
        Keyboard.press('v'); break; // Toggle Camera View
      case 'E':
        Keyboard.press('m'); break;
      case 'F': 
        Keyboard.println("Beta key16"); break;
    }
    delay(100); Keyboard.releaseAll(); // this releases the buttons 
  }
      break;
      
    case 2:    // Application Delta
      digitalWrite(Mode1,LOW); digitalWrite(Mode2,HIGH);
    if (key) {
    Serial.println(key);
    switch (key) {
      case '1': 
        Keyboard.println("Delta key1"); break;
      case '2': 
        Keyboard.println("Delta key2"); break;
      case '3':
        Keyboard.println("Delta key3"); break;
      case '4':
        Keyboard.println("Delta key4"); break;
      case '5':
        Keyboard.println("Delta key5"); break;
      case '6':
        Keyboard.println("Delta key6"); break;
      case '7':
        Keyboard.println("Delta key7"); break;
      case '8':
        Keyboard.println("Delta key8"); break;
      case '9':
        Keyboard.println("Delta key9"); break;
      case '0':
        Keyboard.println("Delta key10"); break;
      case 'A': 
        Keyboard.println("Delta key11"); break;
      case 'B':
        Keyboard.println("Delta key12"); break;
      case 'C':
        Keyboard.println("Delta key13"); break;
      case 'D':
        Keyboard.println("Delta key14"); break;
      case 'E':
        Keyboard.println("Delta key15"); break;
      case 'F': 
        Keyboard.println("Delta key16"); break;
    }
    delay(100); Keyboard.releaseAll(); // this releases the buttons 
  }
      break;
      
    case 3:    // Application Gamma
      digitalWrite(Mode1,HIGH); ;digitalWrite(Mode2,HIGH);
    if (key) {
    Serial.println(key);
    switch (key) {
      case '1': 
        Keyboard.println("nice shot"); break;
      case '2': 
        Keyboard.println("great pass"); break;
      case '3':
        Keyboard.println("great save"); break;
      case '4':
        Keyboard.println("thanks"); break;
      case '5':
        Keyboard.println("i got it"); break;
      case '6':
        Keyboard.println("take the shot"); break;
      case '7':
        Keyboard.println("defending"); break;
      case '8':
        Keyboard.println("go for it"); break;
      case '9':
        Keyboard.println("oh no"); break;
      case '0':
        Keyboard.println("no problem"); break;
      case 'A': 
        Keyboard.println("whoops"); break;
      case 'B':
        Keyboard.println("my bad"); break;
      case 'C':
        Keyboard.println("nooooo!"); break;
      case 'D':
        Keyboard.println("close one"); break;
      case 'E':
        Keyboard.println("calculated"); break;
      case 'F': 
        Keyboard.println("savage"); break;
    }
    delay(100); Keyboard.releaseAll(); // this releases the buttons 
  }
      break;
  }
  delay(1);  // delay in between reads for stability

}

void checkModeButton(){
  buttonState = digitalRead(ModeButton);
  if (buttonState != lastButtonState) { // compare the buttonState to its previous state
    if (buttonState == LOW) { // if the state has changed, increment the counter
      // if the current state is LOW then the button cycled:
      modePushCounter++;
      Serial.println("pressed");
      Serial.print("number of button pushes: ");
      Serial.println(modePushCounter);
    } 
    delay(50); // Delay a little bit to avoid bouncing
  }
  lastButtonState = buttonState; // save the current state as the last state, for next time through the loop
   if (modePushCounter >3){ //reset the counter after 4 presses (remember we start counting at 0)
      modePushCounter = 0;}
}

void encoderA(){
  long newPos = RotaryEncoderA.read()/4; //When the encoder lands on a valley, this is an increment of 4.
  
  if (newPos != positionEncoderA && newPos > positionEncoderA) {
    positionEncoderA = newPos;
    //Serial.println(positionEncoderA);
    Keyboard.press(KEY_LEFT_CTRL);
    delay(25);
    Keyboard.release(KEY_LEFT_CTRL);                      }

  if (newPos != positionEncoderA && newPos < positionEncoderA) {
    positionEncoderA = newPos;
    //Serial.println(positionEncoderA);
    Keyboard.press(KEY_LEFT_SHIFT);
    delay(25);
    Keyboard.release(KEY_LEFT_SHIFT);                      }
}

void encoderB(){
  long newPos = RotaryEncoderB.read()/4; //When the encoder lands on a valley, this is an increment of 4.
  if (newPos != positionEncoderB && newPos > positionEncoderB) {
    positionEncoderB = newPos;
    //Serial.println(positionEncoderB);
    Keyboard.press(','); // Rotate Left - Reduce Time Acceleration
    Keyboard.release(',');                      }

  if (newPos != positionEncoderB && newPos < positionEncoderB) {
    positionEncoderB = newPos;
    //Serial.println(positionEncoderB);
    Keyboard.press('.'); // Rotate Right - Increase Time Acceleration
    Keyboard.release('.');                      }
}
