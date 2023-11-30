#ifndef FEEDINGSTATION_RFID_H
#define FEEDINGSTATION_RFID_H
#include "../../common.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef HOST
void t_scan_rfid();
#elif defined(TARGET)
void t_scan_rfid(void* args);
#endif
#ifdef __cplusplus
}
#endif

#endif
