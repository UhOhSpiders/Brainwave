#include "pulse.h"
void triggerDecide(Pulse pulses[NUMPULSES])
{
  int velocity = 31;
  if (bouncerA.fallingEdge() || midiRisingEdge())
  {                     // check for button press / midi start
    press(pulses, 0, velocity); // do the triggering stuff
    Serial.println("BUTTON A TRIGGER");
    
    if (midiEnable)
    {
      Serial.print("MIDI ON ");
      Serial.print(", ");
      Serial.println(MIDICHANNEL_A);
      midiSendOn(41, MIDICHANNEL_A); // start a midi note
    }
  }
  else if (bouncerA.risingEdge() || midiFallingEdge())
  { // check for button release / midi end
    release(pulses, 0);
    Serial.println("BUTTON A DETRIGGER");
    if (midiEnable)
    {
      // int currentPitchA = getPitch(currentPulseA); // [seed % PITCHES];
      Serial.print("MIDI OFF ");
      // Serial.print(currentPitchA);
      Serial.print(", ");
      Serial.println(MIDICHANNEL_A);
      midiSendOff(41, MIDICHANNEL_A); // end a midi note
    }
  }
  if (bouncerB.fallingEdge())
  {                     // check for button press / midi start / audio on
    press(pulses, 1, velocity); // do the triggering stuff
    Serial.println("BUTTON B TRIGGER");
    if (midiEnable)
    {
      // int currentPitchB = getPitch(currentPulseB); // - 24; //
      Serial.print("MIDI ON ");
      // Serial.print(currentPitchB);
      Serial.print(", ");
      // Serial.println(MIDICHANNEL_B);
      midiSendOn(21, MIDICHANNEL_B); // start a midi note
    }
  }
  else if (bouncerB.risingEdge())
  { // check for button release / midi end
    release(pulses, 1);
    Serial.println("BUTTON B DETRIGGER");
    if (midiEnable)
    {
      // int currentPitchB = getPitch(currentPulseB); // - 24; //
      Serial.print("MIDI OFF ");
      // Serial.print(currentPitchB);
      Serial.print(", ");
      // Serial.println(MIDICHANNEL_B);
      midiSendOff(21, MIDICHANNEL_B); // end a midi note
    }
  }
  if (bouncerC.fallingEdge())
  {
    press(pulses, 1, velocity);
    Serial.println("BUTTON C TRIGGER");
    if (midiEnable)
    {
      // int currentPitchC = getPitch(currentPulseB); // - 24; //
      Serial.print("MIDI ON ");
      // Serial.print(currentPitchC);
      Serial.print(", ");
      // Serial.println(MIDICHANNEL_C);
      midiSendOn(50, MIDICHANNEL_C); // start a midi note
    }
  }
  else if (bouncerC.risingEdge())
  {
    release(pulses, 1);
    Serial.println("BUTTON C DETRIGGER");
    if (midiEnable)
    {
      Serial.print("MIDI OFF ");
      midiSendOff(50, MIDICHANNEL_C); // end a midi note
    }
  }
  if (bouncerD.fallingEdge())
  {
    Serial.println("BUTTON D TRIGGER");
    press(pulses, 0, velocity);
    if (midiEnable)
    {
      midiSendOn(33, MIDICHANNEL_D); // start a midi note
    }
  }
  else if (bouncerD.risingEdge())
  {
    Serial.println("BUTTON D DETRIGGER");
    release(pulses, 0);
    if (midiEnable)
    {
      Serial.print("MIDI OFF ");
      midiSendOff(33, MIDICHANNEL_D); // end a midi note
    }
  }

  if (buttonAState || midiState)
  { // find button held / midi held
    sustain(pulses, 0);
  }
  if (buttonBState || midiState)
  { // find button held / midi held
    sustain(pulses, 1);
  }
}

void collisionDetect(Pulse pulses[NUMPULSES], int i, int vect)
{
  if (pulses[i].vector == vect)
  {                                  // check for this vector only
    int startA = pulses[i].position; // get data for this pulse
    int endA = pulses[i].end;
    for (int j = 0; j < NUMPULSES; j++)
    { // step through pulse object array
      if (pulses[j].active)
      { // check for active pulses only
        if (pulses[j].vector != vect)
        { // check for other vectors only
          //          Serial.println(j);
          int startB = pulses[j].position; // get data for this other pulse
          int endB = pulses[j].end;
          if (startA >= startB)
          {
            //              Serial.println("HERE");/
            if (endA - SPRITELENGTH <= endB + SPRITELENGTH)
            { // check for crossover
              Serial.println("COLLISION DETECTED");
              pulses[i].collision = true;            // flag as this pulse colliding
              pulses[i].colliding = pulses[j].index; // update pointer to other pulse
              pulses[j].collision = true;            // flag as other pulse colliding
              pulses[j].colliding = pulses[i].index; // update pointer to this pulse
            }
            else
            { // if no collision, update collision flag to false
              pulses[i].collision = false;
              pulses[j].collision = false;
            }
          }
        }
      }
    }
    if (pulses[i].collision == true && pulses[i].prevCollision == false)
    { // check for collision state change rising
      Serial.println("COLLISION START");
    }
    if (pulses[i].collision == false && pulses[i].prevCollision == true)
    { // check for collision state change falling
      Serial.println("COLLISION END");
      
    }
  }
  pulses[i].prevCollision = pulses[i].collision; // update prevCollision flag
}
