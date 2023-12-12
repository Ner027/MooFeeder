#include <errno.h>
#include "../inc/app_util.h"

int phy2multi(phy_frame_st* phyFrame, multi_frame_st* multiFrame)
{
    if (!phyFrame || !multiFrame)
        return -EINVAL;

    multiFrame->netFrame.control = (network_control_st*)multiFrame->macFrame.payload;
    multiFrame->netFrame.payload = (uint8_t*)&multiFrame->macFrame.payload[NETWORK_CTRL_LEN];

    multiFrame->appFrame.control = (app_control_st*)multiFrame->netFrame.payload;
    multiFrame->appFrame.payload = (uint8_t*)&multiFrame->netFrame.payload[APP_CTRL_LEN];

    return 0;
}