#ifndef LORA_MAC_TYPES_H
#define LORA_MAC_TYPES_H

#include <stdint.h>

#define MAC_PAYLOAD_LEN 212
#define HDR_LEN 1
#define ADDR_LEN 4
#define MAC_CONTROL_LEN (HDR_LEN + (ADDR_LEN * 2))

#define HDR_DOWN_LINK 0xA0
#define HDR_JOIN_REQ  0xB0
#define HDR_JOIN_ACP  0xC0

typedef struct
{
    uint8_t header;
    uint8_t srcAddr[ADDR_LEN];
    uint8_t destAddr[ADDR_LEN];
    uint8_t* payload;
}mac_frame_st;

typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t version;
    uint8_t patch;
}firmware_version_st;

typedef struct
{
    firmware_version_st fwVer;
}join_req_st;

#endif
