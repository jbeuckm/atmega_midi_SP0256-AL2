#include "AllophoneList.h"

AllophoneList::AllophoneList() {
  count = 0;
}


int AllophoneList::serialize(int address) {

  EEPROM.write(address, count);
  address++;
  
  for (int i=0; i<count; i++) {
    EEPROM.write(address, list[i]);
    address++;
  }

  return address;
}

int AllophoneList::deserialize(int address) {
  count = EEPROM.read(address);
  address++;

  list = malloc(count);

  for (int i=0; i<count; i++) {
    list[i] = EEPROM.read(address);
    address++;
  }

  return address;
}

