#include <cstring>
#include "../inc/mac_client.h"
#include "../../../common/inc/oswrap.h"
#include "../../../serial/inc/serial_types.h"
#include "../../../rn2483/inc/rn2483_types.h"
#include "../../../serial/inc/serial.h"
#include "../../../rn2483/inc/rn2483.h"
#include "../../common/inc/mac_types.h"
#include "../../../phy/inc/phy_types.h"
#include "../../../common/inc/lora_timmings.h"
#include "../../../queue/inc/queue.hpp"

typedef enum
{
    ST_INIT,
    ST_SYNC,
    ST_SEND_PAIR,
    ST_WAIT_PAIR,
    ST_RUNNING
}mac_state_et;

static systick_t txTimeBegin, txTimeEnd, syncTime;
static mac_state_et currentState, nextState;
static serial_port_st radioPort;
static rn2483_st loraRadio;
static phy_frame_st txFrame, rxFrame;
static uint8_t timeSlot;
static CQueue<mac_frame_st> txQueue, rxQueue;
static bool isPaired;

static void mac_state_init();
static void mac_state_sync();
static void mac_state_send_pair();
static void mac_state_wait_pair();
static void mac_state_run();
static void mac_thread();
static void update_times();
static int mac_sync_tx();

static bool keepRunning;

int mac_init()
{
    nextState = ST_INIT;
    keepRunning = true;

    std::thread t1(mac_thread);
    t1.detach();

    return 0;
}

int mac_kill()
{
    if (!keepRunning)
        return -EFAULT;

    keepRunning = false;
}

static void mac_thread()
{
    while (1)
    {
         currentState = nextState;

        switch (currentState)
        {
            case ST_INIT:
                mac_state_init();
                break;
            case ST_SYNC:
                mac_state_sync();
                break;
            case ST_SEND_PAIR:
                mac_state_send_pair();
                break;
            case ST_WAIT_PAIR:
                mac_state_wait_pair();
                break;
            case ST_RUNNING:
                mac_state_run();
                break;
        }
    }
}

static void mac_state_init()
{
    mac_frame_st* pMac;

    serial_open("/dev/ttyUSB1", B57600, &radioPort);
    rn2483_init(&loraRadio, &radioPort);

    pMac = (mac_frame_st*) txFrame.payload;
    memcpy(pMac->srcAddr, &loraRadio.phyAddr[4], ADDR_LEN);

    isPaired = false;
    nextState = ST_SYNC;
}

static void mac_state_sync()
{
    int ret;
    mac_frame_st* pMac;

    ret = rn2483_rx(&loraRadio, (uint8_t*)&rxFrame, PHY_FRAME_LEN);

    //If no message was received keep waiting
    if (ret < 0)
        return;

    pMac = (mac_frame_st*) rxFrame.payload;

    //Only DownLink and JoinAccept frames can be used as synchronization messages
    if ((pMac->header != HDR_DOWN_LINK) && (pMac->header != HDR_JOIN_ACP))
        return;

    if (isPaired)
    {
        nextState = ST_RUNNING;
    }
    else
    {
        nextState = ST_SEND_PAIR;
        timeSlot = BROADCAST_SLOT;
    }

    update_times();
}

static void mac_state_send_pair()
{
    int ret;
    mac_frame_st* pMac;
    firmware_version_st* pFwVer;

    pMac = (mac_frame_st*) txFrame.payload;
    pMac->header = HDR_JOIN_REQ;

    pFwVer = (firmware_version_st*) pMac->payload;
    pFwVer->major   = 0xDE;
    pFwVer->minor   = 0xAD;
    pFwVer->version = 0xDE;
    pFwVer->patch   = 0xC0;

    ret = mac_sync_tx();

    if (ret < 0)
        nextState = ST_SYNC;
    else nextState = ST_WAIT_PAIR;
}

static void mac_state_wait_pair()
{

}

static void mac_state_run()
{

}

static void update_times()
{
    syncTime = GET_CURRENT_TIME();
    txTimeBegin = syncTime + DURATION_FROM_MS(timeSlot * TIME_SLOT_MS);
    txTimeEnd = txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);
}

static int mac_sync_tx()
{
    systick_t currentTime = GET_CURRENT_TIME();

    //Wait for transmission slot
    while ((currentTime < txTimeBegin) && (currentTime > txTimeEnd))
    {
        SLEEP_FOR(txTimeBegin - currentTime);
        currentTime = GET_CURRENT_TIME();
    }

    if (currentTime > txTimeEnd)
        return -ETIMEDOUT;

    return rn2483_tx(&loraRadio, (uint8_t*) &txFrame, txFrame.len);
}