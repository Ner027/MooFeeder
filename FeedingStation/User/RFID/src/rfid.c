#include <sys/errno.h>
#include <string.h>
#include "../inc/rfid.h"
#include "../../yrm100x/inc/yrm100x.h"
#include "usart.h"

#define NOF_READS 3
#define READ_WINDOW_SIZE 4
#define SAMPLE_TIME_MS 250

#define RFID_PRIO 8
#define RFID_STACK_SIZE 512

#define ENABLE_RFID_LOGS

#ifdef ENABLE_RFID_LOGS
#include "../../util/inc/util.h"
#define RFID_LOG(format, ...) print("RFID", format, ##__VA_ARGS__)
#else
#define RFID_LOG(format, ...) do{}while(0)
#endif

static yrm100x_st rfidReader;
static SemaphoreHandle_t keepRunning;
static SemaphoreHandle_t windowMutex;
static uint64_t readWindow[READ_WINDOW_SIZE];
static uint64_t readWindowCopy[READ_WINDOW_SIZE];
static uint8_t wrIndex;
static uint8_t isInitialized = 0;

static void rfid_thread(void* args)
{
    int ret;
    uint64_t temp;
    yrm100x_ret_st yrmRet;
    yrm100x_inv_ut yrmInv;
    systick_t currentTime, nextSampleTime;

    RFID_LOG("Started RFID Thread!\n");

    while (xSemaphoreTake(keepRunning, 0) == pdFALSE)
    {
        ret = yrm100x_write_command(&rfidReader, CMD_INV_SINGLE, NULL, 0);
        if (ret < 0)
        {
            RFID_LOG("Failed to write command to YRM100x device!\n");
            continue;
        }

        ret = yrm100x_read_parse(&rfidReader, &yrmRet, yrmInv.raw, 17);
        if (ret < 0)
        {
            if ((ret != -EFAULT) && (yrmRet.errCode != ERR_INV_FAIL))
            {
                RFID_LOG("YRM100x read attempt exited with error code: (%d)\n", ret);
                continue;
            }

            temp = 0;
        }
        else
        {
            //ARMs will hard-fault when casting to or from 64-bit unaligned memory, so there is a need to copy data
            memcpy(&temp, yrmInv.data.epc + 4, 8);
        }
        xSemaphoreTake(windowMutex, portMAX_DELAY);
        readWindow[wrIndex++ & (READ_WINDOW_SIZE - 1)] = temp;
        xSemaphoreGive(windowMutex);

        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(SAMPLE_TIME_MS));
    }

    RFID_LOG("RFID Thread stopping!\n");

    vTaskDelete(NULL);
}

int rfid_get_current_tag(uint64_t* pTag)
{
    uint8_t i, j, k;
    int mostFreq = -1;

    if (!isInitialized)
        return -EPERM;

    if (!pTag)
        return -EINVAL;

    //Copy the read window since the operation of checking what tag is predominant in it takes a while
    xSemaphoreTake(windowMutex, portMAX_DELAY);
    memcpy(readWindowCopy, readWindow, sizeof(readWindow));
    xSemaphoreGive(windowMutex);

    for (i = 0; i < READ_WINDOW_SIZE; ++i)
    {
        k = 0;
        for (j = 0; j < READ_WINDOW_SIZE; ++j)
        {
            if (readWindowCopy[i] == readWindowCopy[j])
                k++;
        }

        if (k > mostFreq)
        {
            mostFreq = k;
            *pTag = readWindowCopy[i];
        }
    }

    if (*pTag == 0)
        return -ENODATA;

    return mostFreq;
}

int rfid_init()
{
    int ret;
    BaseType_t bRet;

    ret = yrm100x_init(&rfidReader, &huart1);
    if (ret < 0)
    {
        RFID_LOG("Failed to initialize YRM100x Device!\n");
        return -EFAULT;
    }

    windowMutex = xSemaphoreCreateMutex();
    keepRunning = xSemaphoreCreateBinary();
    if (!keepRunning || !windowMutex)
    {
        RFID_LOG("Failed to allocate FreeRTOS Resources!\n");
        return -ENOMEM;
    }

    bRet = xTaskCreate(rfid_thread, "T_RFID", RFID_STACK_SIZE, NULL, RFID_PRIO, NULL);
    if (bRet != pdTRUE)
    {
        RFID_LOG("Failed to create RFID Thread!\n");
        return -ENOMEM;
    }

    memset(readWindow, 0, sizeof(readWindow));
    wrIndex = 0;
    isInitialized = 1;

    return 0;
}