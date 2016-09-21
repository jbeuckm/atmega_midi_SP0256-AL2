#include <MIDI.h>
#include <EEPROM.h>
#include "SP0256.h"
#include "AllophoneList.h"

#define LED_PIN 6
#define GATE_PIN 4

#define ALL_NOTES_OFF 123

#define MIDI_CHANNEL_ADDRESS 0
byte selectedChannel;


MIDI_CREATE_DEFAULT_INSTANCE();

SP0256 speechSynth(12, 11, 13, 9);

// a word for each midi note
AllophoneList noteAssignments[128];

boolean notePlaying = false;
byte currentNote;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (notePlaying) return;
  notePlaying = true;

  speechSynth.speakList(noteAssignments[pitch].list, noteAssignments[pitch].count);

  currentNote = pitch;

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
    digitalWrite(LED_PIN, HIGH);

    
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

    byte readyWord[] = { RR1, EH, EH, PA1, DD2, IY, PA4 };
  speechSynth.speakList(readyWord, 7);

}

void assignListToNote(byte notenum, byte *list, byte count) {

  byte *newList = malloc(count);
  memcpy(newList, list, count);
  
  noteAssignments[notenum].list = newList;
  noteAssignments[notenum].count = count;
}


void loop()
{
    MIDI.read();
}

