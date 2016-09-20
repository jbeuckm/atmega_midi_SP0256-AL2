/*
  Library for controlling the Radio Shack Speech Synth Chip
*/
#ifndef SP0256_h
#define SP0256_h

// Voice Pins -- The SP0256 address pins are all on the same port here.
// This isn't necessary but it does make it a lot easier to pick an
// allophone in code using PORTC in this case.
/*
#define PIN_A1  A0
#define PIN_A2  A1
#define PIN_A3  A2
#define PIN_A4  A3
#define PIN_A5  A4
#define PIN_A6  A5
#define PIN_ALD  2
#define PIN_LRQ  12
*/
#define PA1 0x00
#define PA2 0x01
#define PA3 0x02
#define PA4 0x03
#define PA5 0x04
/* 
#define OY  0x05
#define AY  0x06
#define EH  0x07
#define KK3 0x08
#define PP  0x09
#define JH  0x0A
#define NN1 0x0B
#define IH  0x0C
#define TT2 0x0D
#define RR1 0x0E
#define AX  0x0F
#define MM  0x10
#define TT1 0x11
#define DH1 0x12
#define IY  0x13
#define EY  0x14
#define DD1 0x15
#define UW1 0x16
#define AO  0x17
#define AA  0x18
#define YY2 0x19
#define AE  0x1A
#define HH1 0x1B
#define BB1 0x1C
#define TH  0x1D
#define UH  0x1E
#define UW2 0x1F
#define AW  0x20
#define DD2 0x21
#define GG3 0x22
#define VV  0x23
#define GG1 0x24
#define SH  0x25
#define ZH  0x26
#define RR2 0x27
#define FF  0x28
#define KK2 0x29
#define KK1 0x2A
#define ZZ  0x2B
#define NG  0x2C
#define LL  0x2D
#define WW  0x2E
#define XR  0x2F
#define WH  0x30
#define YY1 0x31
#define CH  0x32
#define ER1 0x33
#define ER2 0x34
#define OW  0x35
#define DH2 0x36
#define SSS 0x37
#define NN2 0x38
#define HH2 0x39
#define OR  0x3A
#define AR  0x3B
#define YR  0x3C
#define GG2 0x3D
#define EL  0x3E
#define BB2 0x3F
*/
#include "Arduino.h"

class SP0256
{

  public:
    SP0256(int aldPin, int lrqPin, int sePin, int resetPin);
    
    void speakList(byte *allophones, byte count);
    void speak( byte allophone );
    void reset(void);

  private:
    int PIN_ALD, PIN_LRQ, PIN_SE, PIN_RESET;
    
};

#endif

