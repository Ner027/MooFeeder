#ifndef LORA_RN2483_TYPES_H
#define LORA_RN2483_TYPES_H

#include <stdint.h>
#include "../../serial/inc/serial_types.h"
#define BUFFER_SIZE 512
#define PHY_ADDR_LEN 8

typedef struct
{
    serial_port_st* serialPort;
    uint8_t phyAddr[PHY_ADDR_LEN];
    uint8_t serialBuffer[BUFFER_SIZE];
}rn2483_st;

#endif
