#include "../inc/cloranetwork.h"
#include "../../common/inc/network_util.h"

#include <cstdio>
#include <asm-generic/errno-base.h>

CLoRaNetwork* CLoRaNetwork::m_instance = nullptr;

void* CLoRaNetwork::run(void*)
{
    while (m_keepRunning)
    {
        m_currentState = m_nextState;

        switch (m_currentState)
        {
            case NETWORK_INIT:
                stateInit();
                break;
            case NETWORK_TX:
                stateTx();
                break;
            case NETWORK_RX:
                stateRx();
                break;
        }
    }

    return nullptr;
}

int CLoRaNetwork::sendMessage(app_frame_st& appFrame, uint8_t src, uint8_t dest)
{
    network_frame_st netFrame;

    netFrame.control.netAddrs = ((dest << 4) & 0xF0) | (src & 0x0F) ;
    netFrame.control.payloadLen = appLenLut[appFrame.control.frameType] + APP_CTRL_LEN;

    memcpy(netFrame.payload, &appFrame, netFrame.control.payloadLen);

    networkTxQueues[dest].push(netFrame);

    return 0;
}

int CLoRaNetwork::decodeMacMessage(mac_frame_st& src, uint8_t srcLen)
{
    int macPayloadLen;
    uint8_t i = 0, srcAddr, dstAddr;
    network_frame_st* pNet;
    uint8_t* pPayload = src.payload;

    macPayloadLen = (uint8_t) srcLen;
    while (i < macPayloadLen)
    {
        pNet = (network_frame_st*) &pPayload[i];
        i += pNet->control.payloadLen + NETWORK_CTRL_LEN;

        dstAddr = (pNet->control.netAddrs >> 4) & 0xF;
        srcAddr = (pNet->control.netAddrs & 0xF);

        if ((dstAddr > NR_OF_SLOTS) || (srcAddr > NR_OF_SLOTS) || (srcAddr < 2))
        {
            NET_LOG("Wrong addresses on MAC Frame! Src Addr: %d Dst Addr: %d\n", srcAddr, dstAddr);
            continue;
        }

        networkRxQueues[srcAddr - 2].push(*pNet);
    }

    return pNet->control.payloadLen;
}

int CLoRaNetwork::composeMacMessage()
{
    uint8_t macLen;
    uint8_t netLen;
    uint8_t* pPayload;
    mac_frame_st macFrame;
    network_frame_st txNetFrame;

    macLen = 0;
    for (auto& networkTxQueue : networkTxQueues)
    {
        if (!networkTxQueue.try_pop(txNetFrame))
            continue;

        pPayload = &macFrame.payload[macLen];
        netLen = txNetFrame.control.payloadLen + NETWORK_CTRL_LEN;
        memcpy(pPayload, &txNetFrame, netLen);
        macLen += netLen;

        if (macLen > MAC_PAYLOAD_LEN)
        {
            NET_LOG("MAC Payload size was exceeded!\n");
            return -ENOSPC;
        }
    }

    if (macLen != 0)
        m_macInstance->pushMessage(macFrame, macLen);

    return macLen;
}

CLoRaNetwork *CLoRaNetwork::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new CLoRaNetwork;

    return m_instance;
}

void CLoRaNetwork::killInstance()
{
    m_keepRunning = false;
    delete m_instance;
    m_instance = nullptr;
}

CLoRaNetwork::CLoRaNetwork() : m_currentState()
{
    SemaphoreInit(&m_networkReady);
    m_macInstance = nullptr;
    m_keepRunning = true;
    m_nextState = NETWORK_INIT;
    this->start();
}

void CLoRaNetwork::stateInit()
{
    bool rc;

    m_macInstance = CLoRaMac::getInstance();

    rc = m_macInstance->waitOnReady(MAX_DELAY);
    if (!rc)
    {
        NET_LOG("MAC not ready in time! Aborting network!\n");
        m_keepRunning = false;
        return;
    }

    NET_LOG("Network Starting!\n");

    SemaphoreGive(&m_networkReady);

    m_nextState = NETWORK_TX;
}

void CLoRaNetwork::stateTx()
{
    composeMacMessage();
    m_nextState = NETWORK_RX;
}

bool CLoRaNetwork::waitOnReady()
{
    return (SemaphoreTake(&m_networkReady) == 0);
}

void CLoRaNetwork::stateRx()
{
    int ret;
    mac_frame_st macFrame;

    m_nextState = NETWORK_TX;

    ret = m_macInstance->popMessage(&macFrame);
    if (ret < 0)
        return;

    ret = decodeMacMessage(macFrame, ret);
    if (ret < 0)
    {
        NET_LOG("Received corrupted MAC Frame!\n");
        return;
    }
}
