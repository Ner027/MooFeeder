#include "../inc/mac_types.h"
#include "../../../phy/inc/phy_types.h"

void phy2mac(phy_frame_st* phyFrame, mac_frame_st* macFrame)
{
    macFrame->control = (mac_control_st*) phyFrame->payload;
    macFrame->payload = (uint8_t*) (&phyFrame->payload[MAC_CTRL_LEN]);
}