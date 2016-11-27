Accepts MIDI signals to control a speech synthesizer IC.

## MIDI Sysex Messages

Function   | Start | Prefix       | Message | End
-----------|-------|--------------|---------|-------
Set Channel| F0    | 77 34 01 00  | cc | f7
Speak Word | F0    | 77 34 01 01  | a1 a2 a3 ... (allophones) | f7
Assign Word| F0    | 77 34 01 02  | nn (note #) a1 a2 a3 ... (allophones) | f7
Save Config| F0    | 77 34 01 03  | (none) | f7
Load Config| F0    | 77 34 01 04  | (none) | f7

After words have been assigned to keys, the device operates as a monophonic synthesizer, playing the word assigned to a key when that key is pressed.

If a "speak word" message is received while a key is down, the word is assigned to that key.


![midi sp0256-al2 layout](atmega_midi_sp0256-al2.png)

This board provides a handy "gate" output that represents the key-down status of incoming MIDI signal.

###Big plans:###

* Add LT6904 (or something) and control pitch.

