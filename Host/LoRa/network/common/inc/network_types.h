#ifndef LORA_NETWORK_TYPES_H
#define LORA_NETWORK_TYPES_H

#include <cstdint>

typedef struct
{
    uint16_t sequenceNumber;
    uint8_t flags;
    uint8_t checkSum;
}network_control_st;

typedef struct
{
    network_control_st* control;
    uint8_t* payload;
}network_frame_st;

#endif
