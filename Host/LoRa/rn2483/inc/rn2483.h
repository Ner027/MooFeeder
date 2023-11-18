#ifndef LORA_RN2483_DRIVER_H
#define LORA_RN2483_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rn2483_types.h"

int rn2483_init(rn2483_st *dev, serial_port_st *serialPort);
int rn2483_tx(rn2483_st *dev, const uint8_t *pSrc, uint8_t srcLen);
int rn2483_rx(rn2483_st *dev, uint8_t *pDst, uint8_t destLen);

#ifdef __cplusplus
}
#endif
#endif
