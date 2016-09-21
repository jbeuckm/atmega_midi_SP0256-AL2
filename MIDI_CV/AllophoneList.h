#include <EEPROM.h>

#ifndef AllophoneList_h
#define AllophoneList_h

class AllophoneList {
  
  public:
    AllophoneList();
    unsigned char *list;
    unsigned char count;

    int serialize(int address);
    int deserialize(int address);
};

#endif

