#ifndef LORA_PHY_TYPES_H
#define LORA_PHY_TYPES_H

/***********************************************************************************************************************
 * Defines
 **********************************************************************************************************************/
#define MAX_PHY_PAYLOAD_LEN 222
#define PHY_FRAME_LEN (MAX_PHY_PAYLOAD_LEN + 1)
#define PHY_CONTROL_LEN 1

/***********************************************************************************************************************
 * Typedefs
 **********************************************************************************************************************/
typedef struct
{
    uint8_t len;
    uint8_t payload [MAX_PHY_PAYLOAD_LEN];
}phy_frame_st;

#endif
