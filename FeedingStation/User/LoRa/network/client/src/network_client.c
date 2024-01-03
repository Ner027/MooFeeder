#include <sys/errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "../../../mac/client/inc/mac_client.h"
#include "../../common/inc/network_util.h"
#include "../../../oswrapper/inc/oswrapper.h"
#include "../../common/inc/network_types.h"

#define NET_STACK_SIZE (configMINIMAL_STACK_SIZE * 8)
#define NET_TASK_PRIO 8

#define TX_QUEUE_SIZE 4
#define RX_QUEUE_SIZE 4

static QueueHandle_st txQueue, rxQueue;

static int decode_mac_frame(mac_frame_st* pMac, uint8_t srcLen)
{
    int ret, decodedMessages;
    uint8_t i;
    uint8_t dstAddr, srcAddr;
    network_frame_st* pNet;

    i = 0;
    decodedMessages = 0;
    while (i < srcLen)
    {
        pNet = (network_frame_st*) (pMac->payload + i);
        i += pNet->control.payloadLen + NETWORK_CTRL_LEN;

        dstAddr = (pNet->control.netAddrs >> 4) & 0xF;
        srcAddr = (pNet->control.netAddrs & 0xF);

        if (dstAddr != mac_get_slot())
        {
            NET_LOG("Received Network message for other client!\n");
            continue;
        }

        if (srcAddr != SERVER_SLOT)
        {
            NET_LOG("Received Network message with origin that differs from the server!\n");
            continue;
        }

        ret = QueuePush(&rxQueue, pNet, SYSTEM_TICK_FROM_MS(0));
        if (ret < 0)
            NET_LOG("Network is full! Discarding Frame!\n");
        else decodedMessages++;
    }

    return decodedMessages;
}

void network_thread(void* args)
{
    int ret, timeSlot;
    mac_frame_st macFrame;
    network_frame_st* pNetFrame;

    pNetFrame = (network_frame_st*) macFrame.payload;

    while (1)
    {
        ret = mac_receive(&macFrame);
        if (ret >= 0)
        {
            decode_mac_frame(&macFrame, ret);
        }

        timeSlot = mac_get_slot();
        if (timeSlot < 0)
        {
            NET_LOG("MAC not paired! Not transmitting in this slot...\n");
        }
        else
        {
            ret = QueuePop(&txQueue, pNetFrame, SYSTEM_TICK_FROM_MS(0));
            if (ret >= 0)
            {
                pNetFrame->control.netAddrs = ((SERVER_SLOT << 4) & 0xF0) | (timeSlot & 0x0F);

                ret = mac_send(&macFrame, pNetFrame->control.payloadLen + NETWORK_CTRL_LEN);
                if (ret < 0)
                    NET_LOG("Failed while trying to submit frame to MAC! Exited with code: (%d)\n", ret);
            }
        }

        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(TIME_SLOT_MS / 2));
    }

    vTaskDelete(NULL);
}

int network_send(network_frame_st* pNetFrame, duration_t maxWait)
{
    int ret;

    if (!pNetFrame)
        return -EINVAL;

    NET_LOG("Submitting message to Network!\n");

    ret = QueuePush(&txQueue, pNetFrame, maxWait);
    if (ret < 0)
    {
        NET_LOG("Network is full! Failed to push Net Message!\n");
        return -ENOSPC;
    }

    return 0;
}

int network_receive(network_frame_st* pNetFrame, duration_t maxWait)
{
    int ret;

    if (!pNetFrame)
        return -EINVAL;

    ret = QueuePop(&rxQueue, pNetFrame, maxWait);
    if (ret < 0)
        return -ENODATA;

    return 0;
}

int network_init()
{
    int ret;
    BaseType_t bRet;

    QueueInit(&txQueue, TX_QUEUE_SIZE, sizeof(network_frame_st));
    QueueInit(&rxQueue, RX_QUEUE_SIZE, sizeof(network_frame_st));

    ret = mac_init();
    if (ret < 0)
    {
        NET_LOG("Failed to initialize MAC Layer! Exited with code: (%d)\n", ret);
        return -EFAULT;
    }

    ret = mac_wait_on_ready();
    if (ret < 0)
    {
        NET_LOG("Error while trying to wait for MAC to be ready! Exited with code: (%d)\n", ret);
        return -EFAULT;
    }

    ret = mac_wait_on_pair();
    if (ret < 0)
    {
        NET_LOG("Error while trying to wait for MAC to be paired! Exited with code: (%d)\n", ret);
        return -EFAULT;
    }

    bRet = xTaskCreate(network_thread, "NET_THREAD", NET_STACK_SIZE, NULL, NET_TASK_PRIO, NULL);
    if (bRet != pdPASS)
    {
        NET_LOG("Failed to create Network Thread!\n");
        return -EFAULT;
    }

    return 0;
}