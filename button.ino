#include <Bounce.h>

#define BUTTONA 16 // button pin
#define BUTTONB 17 // button pin
#define BUTTONC 18
#define BUTTOND 19

// Instantiate a Bounce object with a 25 millisecond debounce time
Bounce bouncerA = Bounce(BUTTONA, 5); // (pin, milliseconds)
Bounce bouncerB = Bounce(BUTTONB, 5); // (pin, milliseconds) // decrease milliseconds from 25 to fix false triggering issue
Bounce bouncerC = Bounce(BUTTONC, 5);
Bounce bouncerD = Bounce(BUTTOND, 5);

void buttonSetup() {
  pinMode(BUTTONA, INPUT_PULLUP); // sets up a button input pin
  pinMode(BUTTONB, INPUT_PULLUP); // sets up a button input pin
  pinMode(BUTTONC, INPUT_PULLUP);
  pinMode(BUTTOND, INPUT_PULLUP);
}

void buttonRead() {
  boolean buttonAChanged = bouncerA.update(); // Update debouncer
  boolean buttonBChanged = bouncerB.update(); // Update debouncer
  boolean buttonCChanged = bouncerC.update();
  boolean buttonDChanged = bouncerD.update();
  if (bouncerA.read())
  {
    buttonAState = false; // flag as pressed
  } else {
    buttonAState = true; // flag as released
    Serial.println("Button A Press");
  }
  if (bouncerB.read())
  {
    buttonBState = false; // flag as pressed
  } else {
    buttonBState = true; // flag as released
      Serial.println("Button B Press");
  }
  if (bouncerC.read())
  {
    buttonCState = false;
  } else {
    buttonCState = true;
    Serial.println("Button C Press");
  }
  if (bouncerD.read())
  {
    buttonDState = false;
  } else {
    buttonDState = true;
    Serial.println("Button D Press");
  }
}