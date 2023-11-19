#ifndef LORA_MAC_UTIL_H
#define LORA_MAC_UTIL_H

#include "mac_types.h"
#include "../../../phy/inc/phy_types.h"

#ifdef __cplusplus
extern "C" {
#endif
void phy2mac(phy_frame_st* phyFrame, mac_frame_st* macFrame);
#ifdef __cplusplus
}
#endif

#endif
