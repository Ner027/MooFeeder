#include <cstdio>
#include <thread>
#include <cstring>
#include "../inc/cloramac.h"
#include "../../../rn2483/inc/rn2483.h"
#include "../../../serial/inc/serial.h"
#include "../../common/inc/mac_util.h"

void* CLoRaMac::run(void* args)
{
    while (true)
    {
        m_currentState = m_nextState;

        switch (m_currentState)
        {
            case ST_INIT:
                stateInit();
                break;
            case ST_EVAL:
                stateEval();
                break;
            case ST_TX:
                stateTx();
                break;
            case ST_RX:
                stateRx();
                break;
        }
    }
}

CLoRaMac::CLoRaMac() : m_loraRadio() , m_radioPort(), m_syncMsg(), m_currentState(), m_natTable()
{
    m_nextState = ST_INIT;
    m_natTable.nofDevices = 0;

    this->start();
}

void CLoRaMac::updateTxTime()
{
    m_lastTxTime = m_txTimeBegin;
    m_txTimeBegin = GET_CURRENT_TIME() + DURATION_FROM_MS(TIME_SLOT_MS * NR_OF_SLOTS);
    m_txTimeEnd = m_txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);
}

void CLoRaMac::stateInit()
{
    mac_frame_st macFrame;

    serial_open("/dev/ttyUSB1", B57600, &m_radioPort);
    rn2483_init(&m_loraRadio, &m_radioPort);

    m_txTimeBegin = GET_CURRENT_TIME();
    m_txTimeEnd = m_txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);

    phy2mac(&m_syncMsg, &macFrame);

    //Compose sync message
    macFrame.control->header = HDR_DOWN_LINK;
    //Set MAC Addresses
    memcpy(macFrame.control->srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);
    memset(macFrame.control->destAddr, 0, ADDR_LEN);
    m_syncMsg.len = MAC_CTRL_LEN + 1;

    m_nextState = ST_EVAL;
}

void CLoRaMac::stateEval()
{
    systick_t currentTime = GET_CURRENT_TIME();

    if ((currentTime >= m_txTimeBegin) && (currentTime <= m_txTimeEnd))
        m_nextState = ST_TX;
    else m_nextState = ST_RX;
}

void CLoRaMac::stateTx()
{
    auto oldTxEnd = m_txTimeEnd;
    QueueItem<phy_frame_st> phyFrame;

    if (m_txQueue.isEmpty())
    {
        rn2483_tx(&m_loraRadio, (uint8_t*)&m_syncMsg, m_syncMsg.len);
    }
    else
    {
        m_txQueue.pop(phyFrame);
        rn2483_tx(&m_loraRadio, (uint8_t*)&phyFrame.object, phyFrame.object.len);
    }

    updateTxTime();
    SLEEP_FOR(oldTxEnd - GET_CURRENT_TIME());

    m_nextState = ST_RX;
}

void CLoRaMac::stateRx()
{
    QueueItem<phy_frame_st> phyFrame;
    mac_frame_st macFrame;
    join_acp_st* pJoinAccept;
    uint8_t clientSlot;

    m_nextState = ST_EVAL;

    auto timeSinceSync = GET_CURRENT_TIME() - m_lastTxTime;
    auto timeMs = MS_FROM_DURATION(timeSinceSync);

    systick_t currentTime = GET_CURRENT_TIME();
    int64_t timeoutMillis = MS_FROM_DURATION(m_txTimeBegin - currentTime);

    if (timeoutMillis < 0)
        timeoutMillis = 0;

    rn2483_set_wdt(&m_loraRadio, (uint32_t)timeoutMillis);

    int ret = rn2483_rx(&m_loraRadio, (uint8_t*) &phyFrame.object, PHY_FRAME_LEN);
    if (ret < 0)
        return;

    phy2mac(&phyFrame.object, &macFrame);

    switch (macFrame.control->header)
    {
        case HDR_UP_LINK:
            ret = findDevice(&macFrame.control->srcAddr);
            if (ret < 0)
            {
                printf("Received message from client from other network!\n");
                return;
            }

            //Server + Broadcast slot
            ret += 2;

            timeSinceSync = GET_CURRENT_TIME() - m_lastTxTime;
            timeMs = MS_FROM_DURATION(timeSinceSync);
            clientSlot = timeMs / TIME_SLOT_MS;

            printf("Time since last sync message: %ld ms... This is slot: %d\n", timeMs, clientSlot);
            printf("Data Received from client at address: %d! -> %.*s\n", ret, phyFrame.object.len - MAC_CTRL_LEN - 1, macFrame.payload);

            if (ret != clientSlot)
            {
                printf("Client transmitting in the wrong slot!\n");
                //TODO: Kick client from network
            }

            return;

        case HDR_JOIN_REQ:
            if (strncmp((char*) macFrame.control->destAddr, (char*) &m_loraRadio.phyAddr[4], ADDR_LEN) != 0)
            {
                printf("Received Join Request targeted at other server! Dest address: 0x%x", *((uint32_t*) macFrame.control->destAddr));
                return;
            }

            ret = findDevice((mac_addr_t*) macFrame.control->srcAddr);
            if (ret >= 0)
            {
                clientSlot = (uint8_t) (ret + 2);
                printf("Client rejoined the server at slot: %d\n", clientSlot);
            }
            else
            {
                if (m_natTable.nofDevices >= (NR_OF_SLOTS - 2))
                {
                    printf("Server is full! Won't accept more clients!\n");
                    return;
                }
                clientSlot = m_natTable.nofDevices + 2;
            }

            macFrame.control->header = HDR_JOIN_ACP;

            //Dest -> Device requesting to join, Source -> Server
            memcpy(macFrame.control->destAddr, macFrame.control->srcAddr, ADDR_LEN);
            memcpy(macFrame.control->srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);

            //Save address to NAT Table
            memcpy(m_natTable.devices[m_natTable.nofDevices++], macFrame.control->destAddr, ADDR_LEN);

            pJoinAccept = (join_acp_st*) macFrame.payload;
            pJoinAccept->timeSlot = clientSlot;

            phyFrame.object.len = MAC_CTRL_LEN + sizeof(join_acp_st) + 1;

            m_txQueue.push(phyFrame);

            printf("Device with address: 0x%x added to network at slot: %d!\n", *((uint32_t*)macFrame.control->destAddr), clientSlot);
            break;
        default:
            printf("Unknown header received!\n");
            break;
    }
}

int CLoRaMac::findDevice(mac_addr_t* natEntry)
{
    for (uint8_t i = 0; i < m_natTable.nofDevices; ++i)
    {
        if (strncmp((char*)m_natTable.devices[i], (char*)natEntry, ADDR_LEN) == 0)
            return i;
    }

    return -ENOMSG;
}

