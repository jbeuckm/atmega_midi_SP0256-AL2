#include "SP0256.h"
#include "Arduino.h"


SP0256::SP0256(int aldPin, int lrqPin) {
    
    PIN_ALD = aldPin;
    PIN_LRQ = lrqPin;
    
  // Set pin modes
  pinMode( PIN_ALD, OUTPUT );
  pinMode( PIN_LRQ, INPUT );
    
  DDRC = B00111111;  // Sets Analog pins 0-5 to output

  digitalWrite(PIN_ALD, HIGH);

  speak( purple, (byte)(sizeof(purple) / sizeof(byte)) );
  speak( monkey, (byte)(sizeof(monkey) / sizeof(byte)) );
  speak( garden, (byte)(sizeof(garden) / sizeof(byte)) );
  speak( moment, (byte)(sizeof(moment) / sizeof(byte)) );
}


void SP0256::speak( byte* allophones, byte count ) {
  for( byte b = 0; b < count; b++ ) {
    speak( allophones[b] );
  }
  speak( PA4 ); // short pause after each word
}

void SP0256::speak( byte allophone ) {
   while ( digitalRead(PIN_LRQ) == HIGH )
    ; // Wait for LRQ to go low

  PORTC = allophone; // select the allophone

  // Tell it to speak by toggling ALD
  digitalWrite(PIN_ALD, LOW);
  digitalWrite(PIN_ALD, HIGH);
}


