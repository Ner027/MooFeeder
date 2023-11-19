#ifndef LORA_APP_TYPES_H
#define LORA_APP_TYPES_H

#include <stdint.h>
#include "../../../mac/common/inc/mac_types.h"
#include "../../../network/common/inc/network_types.h"

#define APP_PAYLOAD_LEN 16
#define APP_CTRL_LEN 1

#define RFID_TAG_LEN 8

typedef struct
{
    uint8_t len;
}app_control_st;
typedef struct
{
    app_control_st* control;
    uint8_t* payload;
}app_frame_st;

typedef struct
{
    uint8_t rfidTag[RFID_TAG_LEN];
}budget_request_st;

typedef struct
{
    app_frame_st appFrame;
    mac_frame_st macFrame;
    network_frame_st netFrame;
}multi_frame_st;

#endif