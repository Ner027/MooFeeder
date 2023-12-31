#ifndef LORA_MAC_TYPES_H
#define LORA_MAC_TYPES_H

#include <stdint.h>
#include "../../../common/inc/lora_timmings.h"
#include "../../../phy/common/inc/phy_types.h"

#define MAC_PAYLOAD_LEN 212
#define HDR_LEN 1
#define ADDR_LEN 4
#define MAC_CTRL_LEN sizeof(mac_control_st)

#define MAC_BROADCAST_ADDR 0xFF

#define HDR_DOWN_LINK 0xA0
#define HDR_JOIN_REQ  0xB0
#define HDR_JOIN_ACP  0xC0
#define HDR_UP_LINK   0xD0
#define HDR_KICK_REQ  0xE0

typedef uint8_t mac_addr_t[ADDR_LEN];

typedef struct
{
    uint8_t header;
    mac_addr_t srcAddr;
    mac_addr_t destAddr;
}mac_control_st;

typedef struct
{
    mac_control_st control;
    uint8_t payload[PHY_PAYLOAD_LEN - MAC_CTRL_LEN];
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

typedef struct
{
    uint8_t timeSlot;
}join_acp_st;

typedef struct
{
    //2 = Broadcast + Server slot
    mac_addr_t devices[NR_OF_SLOTS - 2];
    uint8_t usedSlots;
}nat_table_st;
#endif
