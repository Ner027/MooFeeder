#include <thread>
#include <cstring>
#include "serial/serial.h"
#include "LoRa/mac/server/cloramac.h"
#include "LoRa/mac/common/mac_util.h"
#include "LoRa/phy/common/rn2483/rn2483.h"
#include "oswrapper/oswrapper.h"

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

    serial_close(m_loraRadio.serialPort);

    return nullptr;
}

CLoRaMac::CLoRaMac() : m_loraRadio() , m_radioPort(), m_syncMsg(), m_currentState(), m_natTable()
{
    SemaphoreInit(&m_macReady);
    m_txTimeBegin = GET_CURRENT_TIME();
    m_txTimeEnd = GET_CURRENT_TIME();
    m_keepRunning = true;
    m_nextState = ST_INIT;
    this->start();
}

void CLoRaMac::updateTxTime()
{
    m_lastTxTime = m_txTimeBegin;
    auto tNow = GET_CURRENT_TIME();
    auto dur = SYSTEM_TICK_FROM_MS(TIME_SLOT_MS * NR_OF_SLOTS);
    m_txTimeBegin = tNow + dur;
    m_txTimeEnd = m_txTimeBegin + SYSTEM_TICK_FROM_MS(TIME_SLOT_MS);
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
    m_txTimeEnd = m_txTimeBegin + SYSTEM_TICK_FROM_MS(TIME_SLOT_MS);

    pMacFrame = (mac_frame_st*) m_syncMsg.payload;

    //Compose sync message
    pMacFrame->control.header = HDR_DOWN_LINK;

    //Set MAC Addresses (Server MAC Address is the 4 LSBs of the radio EUI)
    memcpy(pMacFrame->control.srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);
    memset(pMacFrame->control.destAddr, MAC_BROADCAST_ADDR, ADDR_LEN);
    m_syncMsg.len = MAC_CTRL_LEN + 1;

    memset(&m_natTable, 0, sizeof(m_natTable));

    SemaphoreGive(&m_macReady);

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
    static auto lastTxTime = GET_CURRENT_TIME();
    auto oldTxEnd = m_txTimeEnd;
    phy_frame_st phyFrame;

    //Check if there is something to transmit, if not transmit an empty sync message
    if (m_txQueue.try_pop(phyFrame))
    {
        rn2483_tx(&m_loraRadio, (uint8_t*)&phyFrame, phyFrame.len);
    }
    else
    {
        rn2483_tx(&m_loraRadio, (uint8_t*)&m_syncMsg, m_syncMsg.len);
    }

    //Yield until the end of the TX Slot
    auto tNow = GET_CURRENT_TIME();
    updateTxTime();
    if (tNow < oldTxEnd)
        THREAD_SLEEP_FOR(oldTxEnd - tNow);

    auto timeNow = GET_CURRENT_TIME();
    printf("Time since last TX: %ld\n", std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastTxTime).count());
    lastTxTime = timeNow;

    //TODO: Maybe should go to Eval
    m_nextState = ST_RX;
}

void CLoRaMac::stateRx()
{
    phy_frame_st phyFrame;
    mac_frame_st* pMacFrame;
    join_acp_st* pJoinAccept;
    uint8_t clientSlot;

    m_nextState = ST_EVAL;

    auto timeSinceSync = GET_CURRENT_TIME() - m_lastTxTime;
    auto timeMs = MS_FROM_DURATION(timeSinceSync);

    systick_t currentTime = GET_CURRENT_TIME();
    int64_t timeoutMillis = MS_FROM_SYSTEM_TICK(m_txTimeBegin - currentTime).count();

    if (timeoutMillis < 0)
        timeoutMillis = 0;

    rn2483_set_wdt(&m_loraRadio, (uint32_t)timeoutMillis);

    int ret = rn2483_rx(&m_loraRadio, (uint8_t*) &phyFrame, PHY_FRAME_LEN);
    if (ret < 0)
        return;

    pMacFrame = (mac_frame_st*) phyFrame.payload;

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
                phyFrame.len = MAC_CTRL_LEN + 1;

                m_natTable.usedSlots &= ~(1 << ret);
                m_txQueue.push(phyFrame);

                return;
            }

            //Server + Broadcast slot
            ret += 2;

            timeSinceSync = GET_CURRENT_TIME() - m_lastTxTime;
            timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceSync).count();
            clientSlot = timeMs / TIME_SLOT_MS;

            MAC_LOG("Time since last sync message: %ld ms... This is slot: %d\n", timeMs, clientSlot);

            //Check if the client is transmitting in the wrong time slot, this can be caused by clock skews, or restarts
            //From both the client and the server
            if (ret != clientSlot)
            {
                MAC_LOG("Client transmitting in the wrong slot!\n");
                //TODO: Kick client from network
            }

            m_rxQueue.push(phyFrame);

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

            phyFrame.len = MAC_CTRL_LEN + sizeof(join_acp_st) + PHY_CONTROL_LEN;

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

CLoRaMac* CLoRaMac::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new CLoRaMac;

    return m_instance;
}

void CLoRaMac::killInstance()
{
    m_keepRunning = false;
    delete m_instance;
    m_instance = nullptr;
}

bool CLoRaMac::waitOnReady(duration_t maxWait)
{
    return (SemaphoreTake(&m_macReady) == 0);
}

int CLoRaMac::pushMessage(mac_frame_st& macFrame, uint8_t msgLen)
{
    phy_frame_st phyFrame;
    mac_frame_st* pMacFrame;

    pMacFrame = (mac_frame_st*) phyFrame.payload;

    //Message from server to clients aka Down-link
    pMacFrame->control.header = HDR_DOWN_LINK;

    //Src -> Server, Dst -> Broadcast
    memcpy(pMacFrame->control.srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);
    memset(pMacFrame->control.destAddr, MAC_BROADCAST_ADDR, ADDR_LEN);

    memcpy(pMacFrame->payload, macFrame.payload, msgLen);

    phyFrame.len = msgLen + MAC_CTRL_LEN + PHY_CONTROL_LEN;

    m_txQueue.push(phyFrame);

    return phyFrame.len;
}

int CLoRaMac::popMessage(mac_frame_st* macFrame)
{
    phy_frame_st phyFrame;

    if (!macFrame)
        return -EINVAL;

    if (!m_rxQueue.try_pop(phyFrame))
        return -ENODATA;

    memcpy(macFrame, phyFrame.payload, phyFrame.len - PHY_CONTROL_LEN);

    return (int)(phyFrame.len - PHY_CONTROL_LEN - MAC_CTRL_LEN);
}
