#ifndef LORA_APP_UTIL_H
#define LORA_APP_UTIL_H

#include "app_types.h"
#include "../../../phy/inc/phy_types.h"

#ifdef __cplusplus
extern "C" {
#endif
int phy2multi(phy_frame_st *phyFrame, multi_frame_st *multiFrame);
#ifdef __cplusplus
}
#endif
#endif
