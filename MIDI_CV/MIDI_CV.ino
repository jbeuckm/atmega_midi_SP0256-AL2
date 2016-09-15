#include <MIDI.h>
#include "SP0256/SP0256.h"
#include <EEPROM.h>

#define LED_PIN 6
#define GATE_PIN 4

#define ALL_NOTES_OFF 123

#define MIDI_CHANNEL_ADDRESS 0
byte selectedChannel;

MIDI_CREATE_DEFAULT_INSTANCE();

SP0256 speech = new SP0256(12, 11, 13);


boolean notePlaying = false;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (notePlaying) return;
  notePlaying = true;

  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  notePlaying = false;
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}


void handleControlChange(byte channel, byte number, byte value)
{
  switch (number) {
        
    case ALL_NOTES_OFF:
      speech.reset();
      break;
  }
}


void handlePitchBend(byte channel, int bend)
{
  pitchbendOffset = bend >> 4;

  AnalogOutput1.setValue(baseNoteFrequency + pitchbendOffset);
}

void setMidiChannel(int channel) {
    selectedChannel = message[4] % 17;
    EEPROM.write(MIDI_CHANNEL_ADDRESS, selectedChannel);
    midiIn.begin(selectedChannel);    
}

void handleSystemExclusive(byte message[], unsigned size) {
  
  if (message[1] != 0x77) return;      // manufacturer ID
  if (message[2] != 0x34) return;      // model ID
  if (message[3] != 1) return;  // device ID as set with trim pot

  switch (message[4]) {
    
    case 0:
      setMidiChannel(message[5]);
      break;
    
    case 1:
      sendPatchDump();
      break;

    case 2:
      receivePatchDump(message);
      break;
      
    default:
      break;
  }

}

// -----------------------------------------------------------------------------

void setup()
{
  selectedChannel = EEPROM.read(MIDI_CHANNEL_ADDRESS);
  if (selectedChannel > 16) {
    selectedChannel = 0;
    EEPROM.write(MIDI_CHANNEL_ADDRESS, selectedChannel);
  }

    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    pinMode(GATE_PIN, OUTPUT);
    digitalWrite(GATE_PIN, LOW);

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

