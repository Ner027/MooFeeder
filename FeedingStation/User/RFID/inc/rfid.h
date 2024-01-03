#ifndef FEEDINGSTATION_RFID_H
#define FEEDINGSTATION_RFID_H

#include <stdint-gcc.h>

int rfid_init();
int rfid_get_current_tag(uint64_t* pTag);

#endif
