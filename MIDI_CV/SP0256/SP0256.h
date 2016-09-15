// Voice Pins -- The SP0256 address pins are all on the same port here.
// This isn't necessary but it does make it a lot easier to pick an
// allophone in code using PORTC in this case.
#define PIN_A1  A0
#define PIN_A2  A1
#define PIN_A3  A2
#define PIN_A4  A3
#define PIN_A5  A4
#define PIN_A6  A5
#define PIN_ALD  2
#define PIN_LRQ  12

// some words to say
byte purple[] = {PP, ER1, PP, LL };
byte monkey[] = {MM, AX, NN1, KK1, IY };
byte garden[] = {GG1, AR, PA3, DD2, IH, NN1 };
byte moment[] = {MM, OW, MM, EH, NN1, TT2 };


/*
  Library for controlling the Radio Shack Speech Synth Chip
*/
#ifndef SP0256_h
#define SP0256_h

#include "Arduino.h"

class SP0256
{

  public:
    SP0256(int aldPin, int lrqPin);
    
    void speak(byte *allophones, byte count);
    void speak( byte allophone );

  private:
    int PIN_ALD, PIN_LRQ;
    
};

#endif

