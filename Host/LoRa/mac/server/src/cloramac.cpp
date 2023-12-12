#include <cstdio>
#include <thread>
#include <cstring>
#include "../inc/cloramac.h"
#include "../../../rn2483/inc/rn2483.h"
#include "../../../serial/inc/serial.h"
#include "../../common/inc/mac_util.h"

CLoRaMac* CLoRaMac::m_instance = nullptr;

void* CLoRaMac::run(void* args)
{
    while (m_keepRunning)
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
    m_keepRunning = true;
    m_nextState = ST_INIT;
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
    int ret;
    mac_frame_st* pMacFrame;

    //TODO: Replace hardcode port in favor of a more flexible approach
    ret = serial_open("/dev/ttyUSB0", B57600, &m_radioPort);
    if (ret < 0)
    {
        MAC_LOG("Failed to open RN2483 Serial Port!\n");
        m_keepRunning = false;
        return;
    }

    ret = rn2483_init(&m_loraRadio, &m_radioPort);
    if (ret < 0)
    {
        MAC_LOG("Failed to initialize RN2383 Module!\n");
        m_keepRunning = false;
        return;
    }

    m_txTimeBegin = GET_CURRENT_TIME();
    m_txTimeEnd = m_txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);

    pMacFrame = (mac_frame_st*) m_syncMsg.payload;

    //Compose sync message
    pMacFrame->control.header = HDR_DOWN_LINK;

    //Set MAC Addresses (Server MAC Address is the 4 LSBs of the radio EUI)
    memcpy(pMacFrame->control.srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);
    memset(pMacFrame->control.destAddr, 0, ADDR_LEN);
    m_syncMsg.len = MAC_CTRL_LEN + 1;

    memset(&m_natTable, 0, sizeof(m_natTable));

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

    //Check if there is something to transmit, if not transmit an empty sync message
    if (m_txQueue.isEmpty())
    {
        rn2483_tx(&m_loraRadio, (uint8_t*)&m_syncMsg, m_syncMsg.len);
    }
    else
    {
        m_txQueue.pop(phyFrame);
        rn2483_tx(&m_loraRadio, (uint8_t*)&phyFrame.object, phyFrame.object.len);
    }

    //Yield until the end of the TX Slot
    updateTxTime();
    SLEEP_FOR(oldTxEnd - GET_CURRENT_TIME());

    m_nextState = ST_RX;
}

void CLoRaMac::stateRx()
{
    QueueItem<phy_frame_st> phyFrame;
    mac_frame_st* pMacFrame;
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

    pMacFrame = (mac_frame_st*) phyFrame.object.payload;

    if (checkDestination(&pMacFrame->control.destAddr) != 0)
    {
        MAC_LOG("Received message targeted at another server!\n");
        return;
    }

    switch (pMacFrame->control.header)
    {
        case HDR_UP_LINK:
            ret = findDevice(&pMacFrame->control.srcAddr);
            if (ret < 0)
            {
                MAC_LOG("Received message from client that is currently not in this network!\n");

                //Compose a kick request
                pMacFrame->control.header = HDR_KICK_REQ;
                memcpy(pMacFrame->control.destAddr, pMacFrame->control.srcAddr, ADDR_LEN);
                memcpy(pMacFrame->control.srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);
                phyFrame.object.len = MAC_CTRL_LEN + 1;

                m_natTable.usedSlots &= ~(1 << ret);
                m_txQueue.push(phyFrame);

                return;
            }

            //Server + Broadcast slot
            ret += 2;

            timeSinceSync = GET_CURRENT_TIME() - m_lastTxTime;
            timeMs = MS_FROM_DURATION(timeSinceSync);
            clientSlot = timeMs / TIME_SLOT_MS;

            MAC_LOG("Time since last sync message: %ld ms... This is slot: %d\n", timeMs, clientSlot);
            MAC_LOG("Data Received from client at address: %d! -> %.*s\n", ret,
                (int) (phyFrame.object.len - MAC_CTRL_LEN - 1),
                (char*)pMacFrame->payload);

            //Check if the client is transmitting in the wrong time slot, this can be caused by clock skews, or restarts
            //From both the client and the server
            if (ret != clientSlot)
            {
                MAC_LOG("Client transmitting in the wrong slot!\n");
                //TODO: Kick client from network
            }

            return;

        case HDR_JOIN_REQ:
            if (strncmp((char*) pMacFrame->control.destAddr, (char*) &m_loraRadio.phyAddr[4], ADDR_LEN) != 0)
            {
                MAC_LOG("Received Join Request targeted at other server! Dest address: 0x%x", *((uint32_t*) pMacFrame->control.destAddr));
                return;
            }

            ret = findDevice((mac_addr_t*) pMacFrame->control.srcAddr);
            if (ret >= 0)
            {
                clientSlot = (uint8_t) (ret + 2);
                MAC_LOG("Client rejoined the server at slot: %d\n", clientSlot);
            }
            else
            {
                ret = getFreeSlot();

                if (ret < 0)
                {
                    MAC_LOG("Server is full! Won't accept more clients!\n");
                    return;
                }

                clientSlot = ret + 2;
            }

            pMacFrame->control.header = HDR_JOIN_ACP;

            //Dest -> Device requesting to join, Source -> Server
            memcpy(pMacFrame->control.destAddr, pMacFrame->control.srcAddr, ADDR_LEN);
            memcpy(pMacFrame->control.srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);

            //Save address to NAT Table
            memcpy(m_natTable.devices[ret], pMacFrame->control.destAddr, ADDR_LEN);
            m_natTable.usedSlots |= (1 << ret);

            pJoinAccept = (join_acp_st*) pMacFrame->payload;
            pJoinAccept->timeSlot = clientSlot;

            phyFrame.object.len = MAC_CTRL_LEN + sizeof(join_acp_st) + PHY_CONTROL_LEN;

            m_txQueue.push(phyFrame);

            MAC_LOG("Device with address: 0x%x added to network at slot: %d!\n", *((uint32_t*)pMacFrame->control.destAddr), clientSlot);
            break;
        default:
            MAC_LOG("Unknown header received!\n");
            break;
    }
}

int CLoRaMac::findDevice(mac_addr_t* natEntry)
{
    if (!natEntry)
        return -EINVAL;

    for (uint8_t i = 0; i < NR_OF_DEVICES; ++i)
    {
        if (strncmp((char*)m_natTable.devices[i], (char*)natEntry, ADDR_LEN) == 0)
            return i;
    }

    return -ENOMSG;
}

int CLoRaMac::checkDestination(mac_addr_t* pAddr)
{
    if (!pAddr)
        return -EINVAL;

    if (strncmp((char*)pAddr, (char*)&m_loraRadio.phyAddr[4], ADDR_LEN) != 0)
        return -EBADMSG;

    return 0;
}

int CLoRaMac::getFreeSlot() const
{
    for (uint8_t i = 0; i < NR_OF_DEVICES; ++i)
    {
        if ((m_natTable.usedSlots & (1 << i)) == 0)
            return i;
    }

    return -ENOSPC;
}

CLoRaMac *CLoRaMac::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new CLoRaMac;

    return m_instance;
}

void CLoRaMac::killInstance()
{
    m_instance->m_keepRunning = false;
    delete m_instance;
    m_instance = nullptr;
}
