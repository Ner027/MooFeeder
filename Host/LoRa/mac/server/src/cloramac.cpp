#include <cstdio>
#include <thread>
#include <cstring>
#include "../inc/cloramac.h"
#include "../../../common/inc/lora_timmings.h"
#include "../../../rn2483/inc/rn2483.h"
#include "../../../serial/inc/serial.h"

void* CLoRaMac::run(void* args)
{
    systick_t currentTime;

    while (true)
    {
        currentTime = GET_CURRENT_TIME();

        if ((currentTime >= m_txTimeBegin) && (currentTime <= m_txTimeEnd))
            txFlow();
        else rxFlow();
    }
}

CLoRaMac::CLoRaMac() : m_loraRadio() , m_radioPort(), m_syncMsg()
{
    mac_frame_st macFrame;

    serial_open("/dev/ttyUSB0", B57600, &m_radioPort);
    rn2483_init(&m_loraRadio, &m_radioPort);

    m_txTimeBegin = GET_CURRENT_TIME();
    m_txTimeEnd = m_txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);

    //Compose sync message
    macFrame.header = HDR_DOWN_LINK;
    //Set MAC Addresses
    memcpy(macFrame.srcAddr, &m_loraRadio.phyAddr[4], ADDR_LEN);
    memset(macFrame.destAddr, 0, ADDR_LEN);
    m_syncMsg.len = MAC_CONTROL_LEN + 1;
    memcpy(m_syncMsg.payload, &macFrame, MAC_CONTROL_LEN);

    this->start();
}

void CLoRaMac::txFlow()
{
    auto oldTxEnd = m_txTimeEnd;

    if (m_txQueue.isEmpty())
        rn2483_tx(&m_loraRadio, (uint8_t*)&m_syncMsg, m_syncMsg.len);

    updateTxTime();
    SLEEP_FOR(oldTxEnd - GET_CURRENT_TIME());
}

void CLoRaMac::rxFlow()
{
    uint8_t rxBuffer[256];

    int ret = rn2483_rx(&m_loraRadio, rxBuffer, 255);
    if (ret > 0)
        printf("Data Received! %.*s\n", ret, rxBuffer);
}

void CLoRaMac::updateTxTime()
{
    m_txTimeBegin = GET_CURRENT_TIME() + DURATION_FROM_MS(TIME_SLOT_MS * NR_OF_SLOTS);
    m_txTimeEnd = m_txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);
}
