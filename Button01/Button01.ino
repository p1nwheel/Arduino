#include "Keyboard.h"

const int buttonPin = 2;
int buttonState = 0;
int prevButtonState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(buttonPin, HIGH);
  Keyboard.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if ((buttonState != prevButtonState) && (buttonState == HIGH)) {
    Keyboard.press(KEY_BACKSPACE); 
    delay(100);
    Keyboard.releaseAll(); // This is important after every Keyboard.press it will continue to be pressed
  }
  prevButtonState = buttonState;
}
