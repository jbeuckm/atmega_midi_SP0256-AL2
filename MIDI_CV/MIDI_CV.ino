#include <MIDI.h>
#include "SP0256/SP0256.h"

#define LED 13   		    // LED pin on Arduino Uno

#define GATE_PIN 4
#define VELOCITY_PIN 6
#define PWM_OUT_PIN 5

#define ALL_NOTES_OFF 123


MIDI_CREATE_DEFAULT_INSTANCE();

byte selectedChannel = 17;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  liveNoteCount++;
  
  baseNoteFrequency = (pitch - 12) * 42;
  AnalogOutput1.setValue(baseNoteFrequency + pitchbendOffset);
  AnalogOutput2.setValue(velocity * 32);

  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(LED, HIGH);
  analogWrite(VELOCITY_PIN, 2 * velocity);
 }

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
}


void handleControlChange(byte channel, byte number, byte value)
{
  switch (number) {
        
    case ALL_NOTES_OFF:
      break;
  }

}


void handlePitchBend(byte channel, int bend)
{
  pitchbendOffset = bend >> 4;

  AnalogOutput1.setValue(baseNoteFrequency + pitchbendOffset);
}


// -----------------------------------------------------------------------------

void setup()
{
    selectedChannel = 1;

    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    pinMode(GATE_PIN, OUTPUT);
    digitalWrite(GATE_PIN, LOW);

    delay(500);

    // calibrate 8V
    baseNoteFrequency = (108 - 12) * 42;
    AnalogOutput1.setValue(baseNoteFrequency);
    // calibrate full velocity
    AnalogOutput2.setValue(32 * 127);

    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandlePitchBend(handlePitchBend);
    MIDI.setHandleControlChange(handleControlChange);
    MIDI.setHandleSystemExclusive(handleSystemExclusive);
    
    MIDI.begin(selectedChannel);
}


void loop()
{
    MIDI.read();
}

