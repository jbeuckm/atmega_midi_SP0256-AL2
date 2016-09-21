
#ifndef AllophoneList_h
#define AllophoneList_h

#include "Arduino.h"
#include <EEPROM.h>

class AllophoneList {
  
  public:
    AllophoneList();
    byte *list;
    byte count;

    int serialize(int address);
    int deserialize(int address);
};

#endif

