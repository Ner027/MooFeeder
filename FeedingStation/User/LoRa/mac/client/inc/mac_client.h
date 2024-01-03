#ifndef LORA_MAC_CLIENT_H
#define LORA_MAC_CLIENT_H

#include "../../common/inc/mac_types.h"

int mac_init();
int mac_kill();
int mac_get_slot();
int mac_wait_on_pair();
int mac_wait_on_ready();
int mac_send(mac_frame_st* pMacFrame, uint8_t payloadLen);
int mac_receive(mac_frame_st* pMacFrame);

#endif
