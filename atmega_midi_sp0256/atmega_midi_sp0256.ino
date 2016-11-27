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

// an allophone list for each midi note on and off
AllophoneList noteOnAssignments[128];
AllophoneList noteOffAssignments[128];


void saveConfiguration() {
  int address = NOTE_ASSIGNMENT_ADDRESS;

  for (int i=0; i<128; i++) {
    address = noteOnAssignments[i].serialize(address);
  }
  for (int i=0; i<128; i++) {
    address = noteOffAssignments[i].serialize(address);
  }
}

void loadConfiguration() {
  int address = NOTE_ASSIGNMENT_ADDRESS;

  for (int i=0; i<128; i++) {
    address = noteOnAssignments[i].deserialize(address);
  }
  for (int i=0; i<128; i++) {
    address = noteOffAssignments[i].deserialize(address);
  }
}


boolean notePlaying = false;
byte currentNote;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (notePlaying) return;
  currentNote = pitch;
  notePlaying = true;

  speechSynth.speakList(noteOnAssignments[pitch].list, noteOnAssignments[pitch].count);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  speechSynth.speakList(noteOffAssignments[pitch].list, noteOffAssignments[pitch].count);

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


void setFreqKhz(float freq) 
{
  unsigned int oct = (int) (3.322*log10(freq * 1000.0 / 1039.0));
  unsigned int dac = (int) (2048.0 - 2078.0 * pow(2, 10 + oct) / (freq * 1000.0) + 0.5);
   
  unsigned int reg = oct << 12 | dac << 2 | LTC_CONFIG;
   
  byte high = (reg >> 8) & 0xff;
  byte low = reg & 0xff;
   
  Wire.beginTransmission(LTC_ADDRESS);
  Wire.write(high);
  Wire.write(low);
  Wire.endTransmission();  
}


void assignListToNoteOn(byte notenum, byte *list, byte count) {

  byte *newList = malloc(count);
  memcpy(newList, list, count);
  
  noteOnAssignments[notenum].list = newList;
  noteOnAssignments[notenum].count = count;
}

void assignListToNoteOff(byte notenum, byte *list, byte count) {

  byte *newList = malloc(count);
  memcpy(newList, list, count);
  
  noteOffAssignments[notenum].list = newList;
  noteOffAssignments[notenum].count = count;
}


void handleSystemExclusive(byte message[], unsigned size) {

  byte count;

  if (message[1] != 0x77) return;      // manufacturer ID
  if (message[2] != 0x34) return;      // model ID
  if (message[3] != 0x01) return;  // device ID

  byte notenum;
      
  switch (message[4]) {
    
    case 0x00:
      setMidiChannel(message[5]);
      break;

    // speak a word
    case 0x01:
      count = size-6; // disclude first five and eox byte

      speechSynth.speakList(&message[5], count);

      if (notePlaying) {
        assignListToNoteOn(currentNote, &message[5], count);
      }
      
      break;

    // assign a word to a midi note number
    case 0x02:
      notenum = message[5];
      count = size-7; // disclude first five, note number and eox byte
      assignListToNoteOn(notenum, &message[6], count);
      break;

    // assign a word to a midi note number
    case 0x03:
      notenum = message[5];
      count = size-7; // disclude first five, note number and eox byte
      assignListToNoteOff(notenum, &message[6], count);
      break;
      
    // set clock frequency of the LTC6904
    case 0x40: 
      {
        int clockFrequency = ((int)message[5] << 7);
        clockFrequency |= message[6];
        setFreqKhz((float)clockFrequency);
      }
      break;
     
    // save current configuration to eeprom
    case 0x70:
      saveConfiguration();
      break;
      
    // load eeprom configuration
    case 0x71:
      loadConfiguration();
      break;
      
    // request midi sysex dump current configuration
    case 0x72:
      break;
      
    // accept midi sysex dump current configuration
    case 0x73:
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

