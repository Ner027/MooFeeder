#include <cstring>
#include "../inc/mac_client.h"
#include "../../../common/inc/oswrap.h"
#include "../../../serial/inc/serial_types.h"
#include "../../../rn2483/inc/rn2483_types.h"
#include "../../../serial/inc/serial.h"
#include "../../../rn2483/inc/rn2483.h"
#include "../../common/inc/mac_types.h"
#include "../../../phy/inc/phy_types.h"
#include "../../common/inc/mac_util.h"
#include "../../../queue/inc/queue.hpp"

#define MAX_PAIR_LISTEN_TRIES 4

typedef enum
{
    ST_INIT,
    ST_SYNC,
    ST_SEND_PAIR,
    ST_WAIT_PAIR,
    ST_RUNNING,
    ST_IDLE
}mac_state_et;

static systick_t txTimeBegin, txTimeEnd, syncTime;
static mac_state_et currentState, nextState;
static serial_port_st radioPort;
static rn2483_st loraRadio;
static phy_frame_st currentFrame;
static uint8_t timeSlot;
static uint8_t pairListenCounter;
static mac_addr_t serverMac;
static CQueue<phy_frame_st> txQueue, rxQueue;
static bool isPaired;

static void mac_state_init();
static void mac_state_sync();
static void mac_state_send_pair();
static void mac_state_wait_pair();
static void mac_state_run();
static void mac_state_idle();
static void mac_thread();
static void update_times();
static int mac_sync_tx();
static int mac_sync_rx(bool updateWdt);

static bool keepRunning;

void producer_thread()
{
    QueueItem<phy_frame_st> phyFrame;
    mac_frame_st macFrame;

    SLEEP_FOR(DURATION_FROM_MS(10000));

    phy2mac(&phyFrame.object, &macFrame);

    uint32_t counter = 0;

    macFrame.control->header = HDR_UP_LINK;
    memcpy(macFrame.control->srcAddr, &loraRadio.phyAddr[4], ADDR_LEN);
    memcpy(macFrame.control->destAddr, serverMac, ADDR_LEN);


    while (1)
    {
        if (txQueue.isEmpty())
        {
            sprintf((char*)macFrame.payload, "Hello from LoRa Client! This is frame: %d", counter++);
            phyFrame.object.len = MAC_CTRL_LEN + PHY_CONTROL_LEN + strlen((char*)macFrame.payload);
            txQueue.push(phyFrame);
        }

        SLEEP_FOR(DURATION_FROM_MS(50));
    }
}

int mac_init()
{
    nextState = ST_INIT;
    keepRunning = true;

    std::thread t1(mac_thread);
    t1.detach();

    std::thread tProducer(producer_thread);
    tProducer.detach();

    return 0;
}

int mac_kill()
{
    if (!keepRunning)
        return -EFAULT;

    keepRunning = false;

    return 0;
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
            case ST_IDLE:
                mac_state_idle();
                break;
        }
    }
}


static void mac_state_init()
{
    mac_frame_st macFrame;

    serial_open("/dev/ttyUSB0", B57600, &radioPort);
    rn2483_init(&loraRadio, &radioPort);

    isPaired = false;
    nextState = ST_SYNC;
}

static void mac_state_sync()
{
    int ret;
    mac_frame_st macFrame;

    printf("Entering sync...\n");
    rn2483_set_wdt(&loraRadio, MAX_WDT_TIME);
    ret = rn2483_rx(&loraRadio, (uint8_t*)&currentFrame, PHY_FRAME_LEN);

    //If no message was received keep waiting
    if (ret < 0)
        return;

    phy2mac(&currentFrame, &macFrame);

    //Only DownLink and JoinAccept frames can be used as synchronization messages
    if ((macFrame.control->header != HDR_DOWN_LINK) && (macFrame.control->header != HDR_JOIN_ACP))
        return;

    if (isPaired)
    {
        nextState = ST_RUNNING;
    }
    else
    {
        memcpy(serverMac, macFrame.control->srcAddr, ADDR_LEN);
        nextState = ST_SEND_PAIR;
        timeSlot = BROADCAST_SLOT;
    }

    update_times();
}

static void mac_state_send_pair()
{
    int ret;
    mac_frame_st macFrame;
    join_req_st* joinRequest;

    pairListenCounter = 0;

    phy2mac(&currentFrame, &macFrame);
    macFrame.control->header = HDR_JOIN_REQ;
    //Dest -> Server, Source -> This device
    memcpy(macFrame.control->destAddr, serverMac, ADDR_LEN);
    memcpy(macFrame.control->srcAddr, &loraRadio.phyAddr[4], ADDR_LEN);

    joinRequest = (join_req_st*) macFrame.payload;
    joinRequest->fwVer.major   = 0xDE;
    joinRequest->fwVer.minor   = 0xAD;
    joinRequest->fwVer.version = 0xDE;
    joinRequest->fwVer.patch   = 0xC0;

    currentFrame.len = sizeof(join_req_st) + MAC_CTRL_LEN + 1;

    ret = mac_sync_tx();
    if (ret < 0)
        nextState = ST_SYNC;
    else nextState = ST_WAIT_PAIR;
}

static void mac_state_wait_pair()
{
    mac_frame_st macFrame;
    join_acp_st* pJoinAccept;

    //Check if client is in sync with the server
    int ret = mac_sync_rx(true);
    if (ret < 0)
    {
        //If not try to re-sync
        nextState = ST_SYNC;
        return;
    }

    phy2mac(&currentFrame, &macFrame);
    pJoinAccept = (join_acp_st*) macFrame.payload;

    //If a message was received with success check if it is a join accept
    if ((macFrame.control->header != HDR_JOIN_ACP))
    {
        //After a few tries, quit waiting for the pairing accept
        if (pairListenCounter++ > MAX_PAIR_LISTEN_TRIES)
            nextState = ST_WAIT_PAIR;
        else nextState = ST_IDLE;
    }
    else
    {
        //Check if this client was the destination of the message
        if (strncmp((char*) macFrame.control->destAddr, (char*) &loraRadio.phyAddr[4], ADDR_LEN) != 0)
        {
            //TODO: Maybe check server address as well
            printf("Received join accept target at other device!\n");
            nextState = ST_WAIT_PAIR;
        }
        else
        {
            timeSlot = pJoinAccept->timeSlot;
            printf("Pairing request accepted! Paired at slot: %d\n", timeSlot);
            isPaired = true;
            nextState = ST_RUNNING;
        }
    }
}

static void mac_state_run()
{
    QueueItem<phy_frame_st> phyFrame;
    mac_frame_st macFrame;

    systick_t currentTime = GET_CURRENT_TIME();
    printf("Running...\n");
    //Try to receive something, if unable to receive anything, sync was lost, so try to re-sync
    int ret = mac_sync_rx(true);
    if (ret < 0)
    {
        nextState = ST_SYNC;
        return;
    }

    phy2mac(&currentFrame, &macFrame);

    //Check if received message is from the current server this device is using
    if (strncmp((char*) macFrame.control->srcAddr, (char*) serverMac, ADDR_LEN) != 0)
    {
        nextState = ST_SYNC;
        return;
    }

    //Check the message type
    if (macFrame.control->header == HDR_DOWN_LINK)
    {
        //If it is not an empty message push it to the reception queue
        if (currentFrame.len != (MAC_CTRL_LEN + PHY_CONTROL_LEN))
            rxQueue.push(QueueItem<phy_frame_st>(currentFrame));
    }

    //Check if there is anything to transmit
    if (txQueue.isEmpty())
    {
        //If not, yield during the transmission time
        SLEEP_FOR(txTimeBegin - currentTime);
        txTimeBegin = txTimeBegin + DURATION_FROM_MS(timeSlot * TIME_SLOT_MS);
        txTimeEnd = txTimeBegin + DURATION_FROM_MS(TIME_SLOT_MS);
        return;
    }

    //If there is something to transmit, set the current frame to it
    txQueue.pop(phyFrame);
    memcpy(&currentFrame, &phyFrame.object, phyFrame.object.len);

    //Check if sync was lost whilst trying to transmit
    ret = mac_sync_tx();
    if (ret < 0)
    {
        nextState = ST_SYNC;
        return;
    }

    printf("TX Done!\n");
}

static void mac_state_idle()
{
    printf("MAC in idle!\n");
    SLEEP_FOR(DURATION_FROM_MS(30000));
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
    while ((currentTime < txTimeBegin))
    {
        SLEEP_FOR(txTimeBegin - currentTime);
        currentTime = GET_CURRENT_TIME();
    }

    if (currentTime > txTimeEnd)
        return -ETIMEDOUT;

    return rn2483_tx(&loraRadio, (uint8_t*) &currentFrame, currentFrame.len);
}

static int mac_sync_rx(bool updateWdt)
{
    int ret;
    mac_frame_st macFrame;
    int64_t timeoutMillis;

    if (updateWdt)
    {
        systick_t expectedTxTime = txTimeBegin + DURATION_FROM_MS(NR_OF_SLOTS * TIME_SLOT_MS);
        timeoutMillis = MS_FROM_DURATION(expectedTxTime - GET_CURRENT_TIME());

        if (timeoutMillis < 0)
            timeoutMillis = NR_OF_SLOTS * TIME_SLOT_MS;

        rn2483_set_wdt(&loraRadio, timeoutMillis);
    }

    ret = rn2483_rx(&loraRadio, (uint8_t*)&currentFrame, PHY_FRAME_LEN);
    if (ret < 0)
        return -ETIMEDOUT;

    phy2mac(&currentFrame, &macFrame);

    if ((macFrame.control->header != HDR_DOWN_LINK) && (macFrame.control->header != HDR_JOIN_ACP))
        return -EBADMSG;

    update_times();

    return 0;
}