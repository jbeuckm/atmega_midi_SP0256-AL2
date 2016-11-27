#include <MIDI.h>
#include <EEPROM.h>
#include "SP0256.h"
#include "AllophoneList.h"
#include <Wire.h>

#define LTC_ADDRESS B0010110
#define ADR_PIN A3
#define LTC_CONFIG B01  // CLK off and ~CLK on

#define ALL_NOTES_OFF 123

#define NOTE_ASSIGNMENT_ADDRESS 1 // where to store the allophone lists

#define MIDI_CHANNEL_ADDRESS 0  // where to store the configured MIDI channel
byte selectedChannel;


MIDI_CREATE_DEFAULT_INSTANCE();

SP0256 speechSynth(6, A0, 7, A2);

// a word for each midi note
AllophoneList noteAssignments[128];

void saveConfiguration() {
  int address = NOTE_ASSIGNMENT_ADDRESS;

  for (int i=0; i<128; i++) {
    address = noteAssignments[i].serialize(address);
  }
}

void loadConfiguration() {
  int address = NOTE_ASSIGNMENT_ADDRESS;

  for (int i=0; i<128; i++) {
    address = noteAssignments[i].deserialize(address);
  }
}


boolean notePlaying = false;
byte currentNote;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (notePlaying) return;
  currentNote = pitch;
  notePlaying = true;

  speechSynth.speakList(noteAssignments[pitch].list, noteAssignments[pitch].count);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  notePlaying = false;
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


/*
 * oct3 oct2 oct1 oct0 dac9 dac8 dac7 dac6
 * dac5 dac4 dac3 dac2 dac1 dac0 cnf1 cnf0
 */
void setFrequency(byte octave, unsigned int dacFrequency) {
  Wire.beginTransmission(LTC_ADDRESS);
  Wire.write((octave << 4) | ((dacFrequency >> 6) & B1111));
  Wire.write(((dacFrequency << 2) & B11111100) | LTC_CONFIG);
  Wire.endTransmission(); 
}


void assignListToNote(byte notenum, byte *list, byte count) {

  byte *newList = malloc(count);
  memcpy(newList, list, count);
  
  noteAssignments[notenum].list = newList;
  noteAssignments[notenum].count = count;
}


void handleSystemExclusive(byte message[], unsigned size) {

  byte count;

  if (message[1] != 0x77) return;      // manufacturer ID
  if (message[2] != 0x34) return;      // model ID
  if (message[3] != 0x01) return;  // device ID

  switch (message[4]) {
    
    case 0:
      setMidiChannel(message[5]);
      break;

    // speak a word
    case 1:
      count = size-6; // disclude first five and eox byte

      speechSynth.speakList(&message[5], count);

      if (notePlaying) {
        assignListToNote(currentNote, &message[5], count);
      }
      
      break;

    // assign a word to a midi note number
    case 2:
      byte notenum;
      notenum = message[5];
      count = size-7; // disclude first five, note number and eox byte
      assignListToNote(notenum, &message[6], count);
      break;
     
    // save current configuration to eeprom
    case 3:
      saveConfiguration();
      break;
      
    // load eeprom configuration
    case 4:
      loadConfiguration();
      break;
      
    // midi sysex dump current configuration
    case 5:
      break;
      
    default:
      break;
  }

}



// -----------------------------------------------------------------------------


void setup()
{
  loadConfiguration();

  selectedChannel = EEPROM.read(MIDI_CHANNEL_ADDRESS);
  if (selectedChannel > 16) {
    selectedChannel = 1;
    EEPROM.write(MIDI_CHANNEL_ADDRESS, selectedChannel);
  }

  pinMode(ADR_PIN, OUTPUT);
  digitalWrite(ADR_PIN, LOW);

  Wire.begin();
  setFrequency(11, 0);
  
  speechSynth.reset();

  byte readyWord[] = { RR1, EH, EH, PA1, DD2, IY, PA4 };
  speechSynth.speakList(readyWord, 7);

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
