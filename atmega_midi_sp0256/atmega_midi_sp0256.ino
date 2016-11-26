#include <MIDI.h>
#include <EEPROM.h>
#include "SP0256.h"
#include "AllophoneList.h"
#include <Wire.h>

#define LED_PIN 6
#define GATE_PIN 4

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

  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);

  speechSynth.speakList(noteAssignments[pitch].list, noteAssignments[pitch].count);
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

  byte count;

  if (message[1] != 0x77) return;      // manufacturer ID
  if (message[2] != 0x34) return;      // model ID
  if (message[3] != 1) return;  // device ID

  switch (message[4]) {
    
    case 0:
      setMidiChannel(message[5]);
      break;

    // speak a word
    case 1:
      count = size-6; // disclude first five and eox byte

      digitalWrite(LED_PIN, HIGH);
      speechSynth.speakList(&message[5], count);
      digitalWrite(LED_PIN, LOW);

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
  selectedChannel = EEPROM.read(MIDI_CHANNEL_ADDRESS);
  if (selectedChannel > 16) {
    selectedChannel = 1;
    EEPROM.write(MIDI_CHANNEL_ADDRESS, selectedChannel);
  }

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(GATE_PIN, OUTPUT);
  digitalWrite(GATE_PIN, LOW);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandleSystemExclusive(handleSystemExclusive);
    
  MIDI.begin(selectedChannel);

  loadConfiguration();

  Wire.begin(); // join i2c bus (address optional for master)

  speechSynth.reset();

  byte readyWord[] = { RR1, EH, EH, PA1, DD2, IY, PA4 };
  speechSynth.speakList(readyWord, 7);
}

void assignListToNote(byte notenum, byte *list, byte count) {

  byte *newList = malloc(count);
  memcpy(newList, list, count);
  
  noteAssignments[notenum].list = newList;
  noteAssignments[notenum].count = count;
}

byte pitchCode;

void loop()
{
  Wire.beginTransmission(B0010111); // i2c address is B0010111
  Wire.write(pitchCode);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  MIDI.read();
}

