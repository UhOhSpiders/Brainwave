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
#include <WS2812Serial.h>

#define LEDPIN1 8
#define LEDPIN2 1

#define NUMPULSES 300
#define NUMPIXELS 300
#define SPRITELENGTH 7
// #define MIDICHANNEL 1 // you can choose this - stays constant
//  midi channels for strip 1
#define MIDICHANNEL_A 1  // channel for button A
#define MIDICHANNEL_B 2  // channel for button B
#define MIDICHANNEL_CA 3 // channel for collision A
#define MIDICHANNEL_CB 4 // channel for collision B

// midi channels for strip 2 ??
#define MIDICHANNEL_C 5
#define MIDICHANNEL_D 6
#define MIDICHANNEL_CC 7 // channel for collision C
#define MIDICHANNEL_CD 8 // channel for collision D

#define COLOURS 7 // must match size of palette arrays
#define WAIT 15   // (60px/m strip)

boolean midiEnable = true;
byte strip1DrawingMemory[NUMPIXELS * 4];         //  4 bytes per LED
DMAMEM byte strip1DisplayMemory[NUMPIXELS * 16]; 
byte strip2DrawingMemory[NUMPIXELS * 4];         //  4 bytes per LED
DMAMEM byte strip2DisplayMemory[NUMPIXELS * 16];// 16 bytes per LED

WS2812Serial ledStrip1(NUMPIXELS, strip1DisplayMemory, strip1DrawingMemory, LEDPIN1, WS2812_RGB);
WS2812Serial ledStrip2(NUMPIXELS, strip2DisplayMemory, strip2DrawingMemory, LEDPIN2, WS2812_RGB);

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

Pulse pulsesStrip1[NUMPULSES]; // = {Pulse(),Pulse()};
Pulse pulsesStrip2[NUMPULSES];


// the setup function runs once when you press reset or power the board
void setup()
{
  for (int i = 0; i < NUMPULSES; i++)
  {
    pulsesStrip1[i] = Pulse();
    pulsesStrip1[i].init(NUMPIXELS, SPRITELENGTH, 1);
    pulsesStrip1[i].index = i;
    pulsesStrip2[i] = Pulse();
    pulsesStrip2[i].init(NUMPIXELS, SPRITELENGTH, 2);
    pulsesStrip2[i].index = i;
  }
  buttonSetup();
  ledSetup(ledStrip1);
  ledSetup(ledStrip2);
  buttonBreatheSetup();
  blinkSetup();
}

// the loop function runs over and over again forever
void loop()
{
  String pressedButton = buttonRead();
  blinkLoop();
  buttonBreatheLoop();
  updatePulses(pulsesStrip1);
  updatePulses(pulsesStrip2);
  if (activeCount < NUMPULSES && (pressedButton == "Button A" || pressedButton == "Button B")) {
    triggerDecide(pulsesStrip1);
  } else if (activeCount < NUMPULSES && (pressedButton == "Button C" || pressedButton == "Button D")) {
    triggerDecide(pulsesStrip2);
  }
  render(ledStrip1, pulsesStrip1);
  render(ledStrip2, pulsesStrip2);

  killCount++;
  if (killCount >= collisionTimeOut)
  {
    Serial.println("KILL CHANNELS 3 & 4");
    midiPanic34();
    killCount = 0;
  }
  delayMicroseconds(WAIT * 1000);
}

void updatePulses(Pulse pulses[NUMPULSES])
{

  activeCount = 0;
  for (int i = 0; i < NUMPULSES; i++)
  { // step through the pulse object array
    if (pulses[i].active == true)
    { // check for active pulses
      activeCount++;
      debugPulses(pulses,i);                     // print debug info for this pulses
      collisionDetect(pulses, i, 0);              // check for collisions on this pulse from POV of vector 0
      int colliding = pulses[i].update(); // update this pulse
      if (colliding != 9999)
      {
        pulses[i].expire(); // expire this pulse
      }
    }
  }
}

void debugPulses(Pulse pulses[NUMPULSES],int i)
{
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

void incrementPulse()
{
  nextPulse++;
  Serial.println(nextPulse);
  if (nextPulse >= NUMPULSES)
  {
    Serial.println("=========ROLLOVER PULSES==========");
    nextPulse = 0;
  }
}

void press(Pulse pulses[NUMPULSES],int vector, int velocity)
{
  if (vector == 0)
  {
    incrementPulse();
    currentPulseA = nextPulse; // claim the current pulse
    pulses[currentPulseA].trigger(vector, velocity);
  }
  if (vector == 1)
  {
    incrementPulse();
    currentPulseB = nextPulse; // claim the current pulse
    pulses[currentPulseB].trigger(vector, velocity);
  }
}

void sustain(Pulse pulses[NUMPULSES],int vector)
{
  if (vector == 0)
  {
    pulses[currentPulseA].sustain();
  }
  if (vector == 1)
  {
    pulses[currentPulseB].sustain();
  }
}

void release(Pulse pulses[NUMPULSES],int vector)
{
  if (vector == 0)
  {
    pulses[currentPulseA].detrigger();
  }
  if (vector == 1)
  {
    pulses[currentPulseB].detrigger();
  }
}

// void autoPresser()
// {
//   int velocity = 31;

//   if ((count + 300) % 400 == 0)
//   {
//     press(0, velocity);
//   }
//   if ((count + 200) % 400 == 0)
//   {
//     release(0);
//   }

//   if ((count + 100) % 400 == 0)
//   {
//     press(1, velocity);
//   }
//   if (count % 400 == 0)
//   {
//     release(1);
//   }
//   count++;
// }
