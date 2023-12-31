#include <errno.h>
#include <stdbool.h>
#include <memory.h>
#include "../inc/mac_client.h"
#include "../../common/inc/mac_types.h"
#include "../../../serial/inc/serial_types.h"
#include "../../../rn2483/inc/rn2483_types.h"
#include "../../../oswrapper/inc/oswrapper.h"
#include "../../../rn2483/inc/rn2483.h"
#include "../../../serial/inc/serial.h"

#define MAX_PAIR_LISTEN_TRIES 4

#define MAC_STACK_SIZE 1024
#define MAC_PRIO 16

#define TX_QUEUE_SIZE 4
#define RX_QUEUE_SIZE 4

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
static QueueHandle_st txQueue, rxQueue;
static bool isPaired;

static void mac_state_init();
static void mac_state_sync();
static void mac_state_send_pair();
static void mac_state_wait_pair();
static void mac_state_run();
static void mac_state_idle();
#ifdef HOST
static void mac_thread();
#elif defined(TARGET)
static void mac_thread(void* args);
#endif
static void update_times();
static int mac_sync_tx();
static int mac_sync_rx(bool updateWdt);

static bool keepRunning;

#ifdef HOST
static void producer_thread()
#elif defined(TARGET)
static void producer_thread(void* args)
#endif
{
    int ret;
    phy_frame_st phyFrame;
    mac_frame_st* pMacFrame;

    THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(10000));

    pMacFrame = (mac_frame_st*) phyFrame.payload;

    uint32_t counter = 0;

    pMacFrame->control.header = HDR_UP_LINK;
    memcpy(pMacFrame->control.srcAddr, &loraRadio.phyAddr[4], ADDR_LEN);
    memcpy(pMacFrame->control.destAddr, serverMac, ADDR_LEN);


    while (1)
    {
        ret = QueuePeek(&txQueue, &phyFrame, SYSTEM_TICK_FROM_MS(0));
        if (ret < 0)
        {
            sprintf((char*)pMacFrame->payload, "Hello from LoRa Client! This is frame: %lu", counter++);
            phyFrame.len = MAC_CTRL_LEN + PHY_CONTROL_LEN + strlen((char*)pMacFrame->payload);
            QueuePush(&txQueue, &phyFrame, MAX_DELAY);
        }

        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(50));
    }
}

int mac_init()
{
#ifdef TARGET
    BaseType_t ret;
#endif

    nextState = ST_INIT;
    keepRunning = true;

    QueueInit(&txQueue, TX_QUEUE_SIZE, sizeof(phy_frame_st));
    QueueInit(&rxQueue, RX_QUEUE_SIZE, sizeof(phy_frame_st));

#ifdef TARGET
    ret = xTaskCreate(mac_thread, "T_MAC", MAC_STACK_SIZE, NULL, MAC_PRIO, NULL);
    if (ret != pdPASS)
    {
        printf("Failed to create MAC Task!\n");
        return -ENOSPC;
    }
    xTaskCreate(producer_thread, "T_PROD", 512, NULL, 4, NULL);
#endif

    std::thread mt(mac_thread);
    mt.detach();

    std::thread pt(producer_thread);
    pt.detach();

    return 0;
}

int mac_kill()
{
    if (!keepRunning)
        return -EFAULT;

    keepRunning = false;

    return 0;
}

#ifdef HOST
static void mac_thread()
#elif defined(TARGET)
static void mac_thread(void* args)
#endif
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
    memset(&radioPort, 0, sizeof(serial_port_st));

#ifdef HOST
    serial_open("/dev/ttyUSB0", B57600, &radioPort);
#elif defined(TARGET)
    serial_open(&radioPort, &huart2);
#endif
    rn2483_init(&loraRadio, &radioPort);

    isPaired = false;
    nextState = ST_SYNC;
}

static void mac_state_sync()
{
    int ret;
    mac_frame_st* pMacFrame;

    printf("Entering sync...\n");
    rn2483_set_wdt(&loraRadio, MAX_WDT_TIME);
    ret = rn2483_rx(&loraRadio, (uint8_t*)&currentFrame, PHY_FRAME_LEN);

    //If no message was received keep waiting
    if (ret < 0)
        return;

    pMacFrame = (mac_frame_st*) currentFrame.payload;

    //Only DownLink and JoinAccept frames can be used as synchronization messages
    if ((pMacFrame->control.header != HDR_DOWN_LINK) && (pMacFrame->control.header != HDR_JOIN_ACP) && (pMacFrame->control.header != HDR_KICK_REQ))
        return;

    if (isPaired)
    {
        nextState = ST_RUNNING;
    }
    else
    {
        memcpy(serverMac, pMacFrame->control.srcAddr, ADDR_LEN);
        nextState = ST_SEND_PAIR;
        timeSlot = BROADCAST_SLOT;
    }

    update_times();
}

static void mac_state_send_pair()
{
    int ret;
    mac_frame_st* pMacFrame;
    join_req_st* joinRequest;

    pMacFrame = (mac_frame_st*) currentFrame.payload;
    pairListenCounter = 0;

    pMacFrame->control.header = HDR_JOIN_REQ;
    //Dest -> Server, Source -> This device
    memcpy(pMacFrame->control.destAddr, serverMac, ADDR_LEN);
    memcpy(pMacFrame->control.srcAddr, &loraRadio.phyAddr[4], ADDR_LEN);

    joinRequest = (join_req_st*) pMacFrame->payload;
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
    mac_frame_st* pMacFrame;
    join_acp_st* pJoinAccept;

    //Check if client is in sync with the server
    int ret = mac_sync_rx(true);
    if (ret < 0)
    {
        //If not try to re-sync
        nextState = ST_SYNC;
        return;
    }

    pMacFrame = (mac_frame_st*) currentFrame.payload;
    pJoinAccept = (join_acp_st*) pMacFrame->payload;

    //If a message was received with success check if it is a join accept
    if ((pMacFrame->control.header != HDR_JOIN_ACP))
    {
        //After a few tries, quit waiting for the pairing accept
        if (pairListenCounter++ > MAX_PAIR_LISTEN_TRIES)
            nextState = ST_WAIT_PAIR;
        else nextState = ST_IDLE;
    }
    else
    {
        //Check if this client was the destination of the message
        if (strncmp((char*) pMacFrame->control.destAddr, (char*) &loraRadio.phyAddr[4], ADDR_LEN) != 0)
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
    mac_frame_st* pMacFrame;

    systick_t currentTime = GET_CURRENT_TIME();
    printf("Running...\n");
    //Try to receive something, if unable to receive anything, sync was lost, so try to re-sync
    int ret = mac_sync_rx(true);
    if (ret < 0)
    {
        nextState = ST_SYNC;
        return;
    }

    pMacFrame = (mac_frame_st*) currentFrame.payload;

    //Check if received message is from the current server this device is using
    if (strncmp((char*) pMacFrame->control.srcAddr, (char*) serverMac, ADDR_LEN) != 0)
    {
        nextState = ST_SYNC;
        return;
    }

    //Check the message type
    if (pMacFrame->control.header == HDR_DOWN_LINK)
    {
        //If it is not an empty message push it to the reception queue
        if (currentFrame.len != (MAC_CTRL_LEN + PHY_CONTROL_LEN))
            QueuePush(&rxQueue, &currentFrame, MAX_DELAY);
    }
    else if (pMacFrame->control.header == HDR_KICK_REQ)
    {
        isPaired = false;
        nextState = ST_SYNC;
        return;
    }

    //Check if there is anything to transmit
    ret = QueuePop(&txQueue, &currentFrame, SYSTEM_TICK_FROM_MS(0));
    if (ret < 0)
    {
        //If not, yield during the transmission time
        THREAD_SLEEP_FOR(txTimeBegin - currentTime);
        txTimeBegin = txTimeBegin + SYSTEM_TICK_FROM_MS(timeSlot * TIME_SLOT_MS);
        txTimeEnd = txTimeBegin + SYSTEM_TICK_FROM_MS(TIME_SLOT_MS);
        return;
    }

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
    THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(30000));
}

static void update_times()
{
    syncTime = GET_CURRENT_TIME();
    txTimeBegin = syncTime + SYSTEM_TICK_FROM_MS(timeSlot * TIME_SLOT_MS);
    txTimeEnd = txTimeBegin + SYSTEM_TICK_FROM_MS(TIME_SLOT_MS);
}

static int mac_sync_tx()
{
    systick_t currentTime = GET_CURRENT_TIME();

    //Wait for transmission slot
    while ((currentTime < txTimeBegin))
    {
        THREAD_SLEEP_FOR(txTimeBegin - currentTime);
        currentTime = GET_CURRENT_TIME();
    }

    if (currentTime > txTimeEnd)
        return -ETIMEDOUT;

    return rn2483_tx(&loraRadio, (uint8_t*) &currentFrame, currentFrame.len);
}

static int mac_sync_rx(bool updateWdt)
{
    int ret;
    mac_frame_st* pMacFrame;
    duration_t timeoutMillis;

    if (updateWdt)
    {
        systick_t expectedTxTime = txTimeBegin + SYSTEM_TICK_FROM_MS(NR_OF_SLOTS * TIME_SLOT_MS);
        timeoutMillis = MS_FROM_SYSTEM_TICK(expectedTxTime - GET_CURRENT_TIME());

        if (timeoutMillis < SYSTEM_TICK_FROM_MS(0))
            timeoutMillis = SYSTEM_TICK_FROM_MS(NR_OF_SLOTS * TIME_SLOT_MS);

        rn2483_set_wdt(&loraRadio, MS_FROM_DURATION(timeoutMillis));
    }

    ret = rn2483_rx(&loraRadio, (uint8_t*)&currentFrame, PHY_FRAME_LEN);
    if (ret < 0)
        return -ETIMEDOUT;

    pMacFrame = (mac_frame_st*) currentFrame.payload;

    if ((pMacFrame->control.header != HDR_DOWN_LINK) && (pMacFrame->control.header != HDR_JOIN_ACP) && (pMacFrame->control.header != HDR_KICK_REQ))
        return -EBADMSG;

    update_times();

    return 0;
}