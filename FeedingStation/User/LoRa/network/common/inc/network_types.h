#ifndef LORA_NETWORK_TYPES_H
#define LORA_NETWORK_TYPES_H

#include <stdint.h>
#define NETWORK_CTRL_LEN sizeof(network_control_st)

typedef struct
{
    uint8_t netAddrs;
    uint8_t payloadLen;
}network_control_st;

typedef struct
{
    network_control_st control;
    uint8_t payload[MAC_PAYLOAD_LEN - NETWORK_CTRL_LEN];
}network_frame_st;

#endif
