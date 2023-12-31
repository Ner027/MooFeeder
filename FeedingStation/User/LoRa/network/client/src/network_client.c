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

void network_thread(void* args)
{
    int ret;
    mac_frame_st macFrame;
    network_frame_st* pNetFrame;

    pNetFrame = (network_frame_st*) macFrame.payload;

    while (1)
    {
        ret = mac_receive(&macFrame);
        if (ret >= 0)
        {
            //Decode mac message
        }

        ret = QueuePop(&txQueue, pNetFrame, SYSTEM_TICK_FROM_MS(TIME_SLOT_MS / 2));
        if (ret < 0)
            continue;

        ret = mac_get_slot();
        if (ret < 0)
        {
            NET_LOG("MAC not paired! Discarding message...\n");
        }
        else
        {
            pNetFrame->control.netAddrs = ((SERVER_SLOT << 4) & 0xF0) | (ret & 0x0F);
            ret = mac_send(&macFrame);
            if (ret < 0)
            {
                NET_LOG("Failed while trying to submit frame to MAC! Exited with code: (%d)\n", ret);
            }
        }
    }

    vTaskDelete(NULL);
}

int network_send(network_frame_st* pNetFrame)
{
    int ret;

    if (!pNetFrame)
        return -EINVAL;

    ret = QueuePush(&txQueue, pNetFrame, SYSTEM_TICK_FROM_MS(0));
    if (ret < 0)
    {
        NET_LOG("Network is full! Failed to push Net Message!\n");
        return -ENOSPC;
    }

    return 0;
}

int network_init()
{
    int ret;
    BaseType_t bRet;

    QueueInit(&txQueue, TX_QUEUE_SIZE, sizeof(network_frame_st));
    QueueInit(&txQueue, RX_QUEUE_SIZE, sizeof(network_frame_st));

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

    bRet = xTaskCreate(network_thread, "NET_THREAD", NET_STACK_SIZE, NULL, 8, NULL);
    if (bRet != pdPASS)
    {
        NET_LOG("Failed to create Network Thread!\n");
        return -EFAULT;
    }

    return 0;
}