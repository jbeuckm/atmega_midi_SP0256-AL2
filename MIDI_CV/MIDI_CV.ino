#include <MIDI.h>
#include <EEPROM.h>
#include "SP0256.h"

#define LED_PIN 6
#define GATE_PIN 4

#define ALL_NOTES_OFF 123

#define MIDI_CHANNEL_ADDRESS 0
byte selectedChannel;


MIDI_CREATE_DEFAULT_INSTANCE();

SP0256 speechSynth(12, 11, 13, 9);
/*
class AllophoneList {
  public:
    byte list[];
    byte count;
};


// a word for each midi note
AllophoneList noteLists[128];
*/

boolean notePlaying = false;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (notePlaying) return;
  notePlaying = true;

//  speechSynth.speak(noteLists[pitch].list, noteLists[pitch].count);
byte purple[] = {0x09, 0x33, 0x09, 0x2d };
//  speechSynth.speakList(purple, 0x04);

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
      speechSynth.reset();
      break;
  }
}


void handlePitchBend(byte channel, int bend)
{
}

void setMidiChannel(int channel) {
    EEPROM.write(MIDI_CHANNEL_ADDRESS, channel);
    MIDI.begin(channel);
}

void handleSystemExclusive(byte message[], unsigned size) {
  
  if (message[1] != 0x77) return;      // manufacturer ID
  if (message[2] != 0x34) return;      // model ID
  if (message[3] != 1) return;  // device ID

  switch (message[4]) {
    
    case 0:
      setMidiChannel(message[5]);
      break;

    // speak a word
    case 1:
      speechSynth.speakList(&message[5], size-6);
      break;

    // assign a word to a midi note number
    case 2:
      byte i;
      i = message[5];
//      noteLists[i].list = &message[6];
//      noteLists[i].count = size-7;
      break;
     
    // save current configuration to default
    case 3:
      break;
      
    // request dump current configuration
    case 4:
      break;
      
    // load current configuration
    case 5:
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
    selectedChannel = 1;
    EEPROM.write(MIDI_CHANNEL_ADDRESS, selectedChannel);
  }
    selectedChannel = 1;

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pinMode(GATE_PIN, OUTPUT);
    digitalWrite(GATE_PIN, LOW);

    speechSynth.reset();

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

