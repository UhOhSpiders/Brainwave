// Changes:
// moved ledpin to 8 for Teensy 4.0 compatiability
// Issues:
// 
// WIP:
// 30/09/22 RMS

// Iain's Changes 30/10/24
// switched LED color palettes from RGBW to RGB for new strips
// updated NUMPULSES and NUMPIXELS to match new strips 
// updated WAIT to 15


#include "pulse.h"

#define NUMPULSES 300
#define NUMPIXELS 300 
#define SPRITELENGTH 7
//#define MIDICHANNEL 1 // you can choose this - stays constant
// midi channels for strip 1
#define MIDICHANNEL_A 1 // channel for button A
#define MIDICHANNEL_B 2 // channel for button B
#define MIDICHANNEL_CA 3 // channel for collision A
#define MIDICHANNEL_CB 4 // channel for collision B

// midi channels for strip 2 ??
#define MIDICHANNEL_C 5
#define MIDICHANNEL_D 6
#define MIDICHANNEL_CC 7 // channel for collision C
#define MIDICHANNEL_CD 8 // channel for collision D

#define COLOURS 7 // must match size of palette arrays
#define WAIT 15 // (60px/m strip)

boolean midiEnable = true;

int nextPulse = 0;
int currentPulseA = 0;
int currentPulseB = 0;

int count = 0;

boolean buttonAState = false;
boolean buttonBState = false;
boolean buttonCState = false;
boolean buttonDState = false;

int activeCount = 0;

int killCount = 0;
int collisionTimeOut = 100;

Pulse pulses[NUMPULSES]; // = {Pulse(),Pulse()};

// the setup function runs once when you press reset or power the board
void setup() {
  for (int i = 0; i < NUMPULSES; i++) {
    pulses[i] = Pulse();
    pulses[i].init(NUMPIXELS, SPRITELENGTH);
    pulses[i].index = i;
  }
  buttonSetup();
  pixelsSetup();
  buttonBreatheSetup();
  blinkSetup();
}

// the loop function runs over and over again forever
void loop() {
  //  colorWipe(0x00FF0000, 1500000);

  blinkLoop();
  buttonBreatheLoop();
  buttonRead();
  updatePulses();
  if (activeCount < NUMPULSES) {
    triggerDecide();
  }
  render();

  //  autoPresser();

  killCount++;
  if (killCount >= collisionTimeOut) {
    Serial.println("KILL CHANNELS 3 & 4");
    midiPanic34();
    killCount = 0;
  }
//  Serial.println(millis());
  delayMicroseconds(WAIT * 1000);
}

void updatePulses() {
  activeCount = 0;
  for (int i = 0; i < NUMPULSES; i++) { // step through the pulse object array
    if (pulses[i].active == true) { // check for active pulses
      activeCount++;
      debugPulses(i); // print debug info for this pulses
      collisionDetect(i, 0); // check for collisions on this pulse from POV of vector 0
      int colliding = pulses[i].update();// update this pulse
      if (colliding != 9999) {
        pulses[i].expire(); //expire this pulse
      }
    }
  }
}

void debugPulses(int i) {
  Serial.print(pulses[i].index);
  Serial.print(", ");
  Serial.print(pulses[i].vector);
  Serial.print(", ");
  Serial.print(pulses[i].position);
  Serial.print(", ");
  Serial.print(pulses[i].end);
  Serial.print(", ");
  Serial.print(pulses[i].hold);
  Serial.print(", ");
  Serial.print(pulses[i].seed);
  Serial.print(", ");
  Serial.print(pulses[i].collision);
  Serial.print(", ");
  Serial.println("");
}

void incrementPulse() {
  nextPulse++;
  Serial.println(nextPulse);
  if (nextPulse >= NUMPULSES) {
    Serial.println("=========ROLLOVER PULSES==========");
    nextPulse = 0;
  }
}

void press(int vector, int velocity) {
  if (vector == 0) {
    incrementPulse();
    currentPulseA = nextPulse; // claim the current pulse
    pulses[currentPulseA].trigger(vector, velocity);
  }
  if (vector == 1) {
    incrementPulse();
    currentPulseB = nextPulse; // claim the current pulse
    pulses[currentPulseB].trigger(vector, velocity);
  }
}

void sustain(int vector) {
  if (vector == 0) {
    pulses[currentPulseA].sustain();
  }
  if (vector == 1) {
    pulses[currentPulseB].sustain();
  }
}

void release(int vector) {
  if (vector == 0) {
    pulses[currentPulseA].detrigger();
  }
  if (vector == 1) {
    pulses[currentPulseB].detrigger();
  }
}

void autoPresser() {
  int velocity = 31;

  if ((count + 300) % 400 == 0) {
    press(0, velocity);
  }
  if ((count + 200) % 400 == 0) {
    release(0);
  }

  if ((count + 100) % 400 == 0) {
    press(1, velocity);
  }
  if (count % 400 == 0) {
    release(1);
  }
  count++;
}
