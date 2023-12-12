#include "../inc/cloranetwork.h"

#include <cstdio>
#include <asm-generic/errno-base.h>

#include "../../common/inc/network_types.h"

void* CLoRaNetwork::run(void*)
{

}

int CLoRaNetwork::sendMessage(app_frame_st& appFrame, uint8_t src, uint8_t dest)
{
    network_frame_st netFrame;

    netFrame.control.netAddrs = ((dest << 4) & 0xF0) | (src & 0x0F) ;
    netFrame.control.payloadLen = appFrame.control.len + APP_CTRL_LEN;
    memcpy(netFrame.payload, &appFrame, appFrame.control.len + APP_CTRL_LEN);

    networkTxQueues[dest].push(QueueItem(netFrame));

    return 0;
}

int CLoRaNetwork::decodeMacMessage(mac_frame_st& src, uint8_t srcLen)
{
    int macPayloadLen;
    uint8_t i = 0, srcAddr, dstAddr;
    network_frame_st* pNet;
    uint8_t* pPayload = src.payload;

    macPayloadLen = srcLen - MAC_CTRL_LEN;
    while (i < macPayloadLen)
    {
        pNet = (network_frame_st*) &pPayload[i];
        i += pNet->control.payloadLen + NETWORK_CTRL_LEN;

        dstAddr = (pNet->control.netAddrs >> 4) & 0xF;
        srcAddr = (pNet->control.netAddrs & 0xF);

        if ((dstAddr > NR_OF_SLOTS) || (srcAddr > NR_OF_SLOTS))
        {
            printf("Received wrong Net Frame!\n");
            continue;
        }

        networkRxQueues[srcAddr].push(QueueItem(*pNet));
    }

    return macPayloadLen;
}

int CLoRaNetwork::composeMacMessage()
{
    uint8_t macLen;
    uint8_t netLen;
    uint8_t* pPayload;
    mac_frame_st macFrame;
    QueueItem<network_frame_st> txNetFrame;

    macLen = 0;
    for (uint8_t i = 0; i < NR_OF_DEVICES; ++i)
    {
        if (networkTxQueues[i].isEmpty())
            continue;

        networkTxQueues[i].pop(txNetFrame);

        pPayload = &macFrame.payload[macLen];
        netLen = txNetFrame.object.control.payloadLen + NETWORK_CTRL_LEN;
        memcpy(pPayload, &txNetFrame.object, netLen);
        macLen += netLen;

        if (macLen > MAC_PAYLOAD_LEN)
        {
            printf("MAC Payload size was exceeded!\n");
            return -ENOSPC;
        }
    }

    decodeMacMessage(macFrame, macLen);

    return macLen;
}
