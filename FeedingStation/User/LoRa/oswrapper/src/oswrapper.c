#include <sys/errno.h>
#include <stdio.h>
#include "../inc/oswrapper.h"

int QueueInit(QueueHandle_st* queueHandle, uint32_t queueSize, uint32_t itemSize)
{
    if (!queueHandle)
        return -EINVAL;

    *queueHandle = xQueueCreate(queueSize, itemSize);

    return 0;
}

int QueueDestroy(QueueHandle_st* queueHandle)
{
    if (!queueHandle)
        return -EINVAL;

    vQueueDelete(*queueHandle);

    return 0;
}

int QueuePush(QueueHandle_st* queueHandle, void* item, duration_t timeToWait)
{
    if (!queueHandle || !item)
        return -EINVAL;

    if (xQueueSend(*queueHandle, item, timeToWait) == pdTRUE)
        return 0;

    return -ETIMEDOUT;
}

int QueuePop(QueueHandle_st* queueHandle, void* dest, duration_t timeToWait)
{
    if (!queueHandle || !dest)
        return -EINVAL;

    if (xQueueReceive(*queueHandle, dest, timeToWait) != pdTRUE)
        return -ETIMEDOUT;

    return 0;
}

int QueuePeek(QueueHandle_st* queueHandle, void* dest, duration_t timeToWait)
{
    if (!queueHandle || !dest)
        return -EINVAL;

    if (xQueuePeek(*queueHandle, dest, timeToWait) != pdTRUE)
        return -ETIMEDOUT;

    return 0;
}

int QueueFreeSpace(QueueHandle_st* queueHandle)
{
    if (!queueHandle)
        return -EINVAL;

    return (int) uxQueueSpacesAvailable(*queueHandle);
}

int MutexInit(Mutex_t* mutex)
{
    if (!mutex)
        return -EINVAL;

    (*mutex) = xSemaphoreCreateMutex();

    return 0;
}

int MutexDestroy(Mutex_t* mutex)
{
    if (!mutex)
        return -EINVAL;

    vSemaphoreDelete(*mutex);

    return 0;
}

int MutexLock(Mutex_t* mutex, duration_t maxWaitTime)
{
    if (!mutex)
        return -EINVAL;

    if (xSemaphoreTake(*mutex, maxWaitTime) != pdTRUE)
        return -ETIMEDOUT;

    return 0;
}

int MutexUnlock(Mutex_t* mutex)
{
    if (!mutex)
        return -EINVAL;

    xSemaphoreGive(*mutex);

    return 0;
}

int SemaphoreInit(SemaphoreHandle_st* pSem)
{
    if (!pSem)
        return -EINVAL;

    (*pSem) = xSemaphoreCreateBinary();

    return 0;
}

int SemaphoreDestroy(SemaphoreHandle_st* pSem)
{
    if (!pSem)
        return -EINVAL;

    vSemaphoreDelete(*pSem);

    return 0;
}

int SemaphoreTake(SemaphoreHandle_st* pSem)
{
    if (!pSem)
        return -EINVAL;

    xSemaphoreTake(*pSem, portMAX_DELAY);

    return 0;
}

int SemaphoreGive(SemaphoreHandle_st* pSem)
{
    if (!pSem)
        return -EINVAL;

    xSemaphoreGive(*pSem);

    return 0;
}


static void ListUnsafeInternalSet(ListHandle_st* pList, uint32_t index, void* pItem)
{
    uint32_t i;
    uint8_t* ptr = (uint8_t*) pItem;
    uint8_t* pStart = &pList->pData[pList->itemSize * index];

    for (i = 0; i < pList->itemSize; ++i)
        pStart[i] = ptr[i];
}

static void ListUnsafeInternalGet(ListHandle_st* pList, uint32_t index, void* pRet)
{
    uint32_t i;
    uint8_t* ptr = (uint8_t*) pRet;
    uint8_t* pStart = &pList->pData[pList->itemSize * index];

    for (i = 0; i < pList->itemSize; ++i)
        ptr[i] = pStart[i];
}

static void ListUnsafeSwap(ListHandle_st* pList, uint32_t src, uint32_t dest)
{
    uint32_t i;
    uint8_t temp;
    uint8_t* pSrc = (uint8_t*) &pList->pData[pList->itemSize * src];
    uint8_t* pDest = (uint8_t*) &pList->pData[pList->itemSize * dest];

    for (i = 0; i < pList->itemSize; ++i)
    {
        temp = *pDest;
        *pDest = *pSrc;
        *pSrc = temp;

        pDest++;
        pSrc++;
    }
}

int ListUnsafeInitStatic(ListHandle_st* pList, void* pBuffer, uint32_t listSize, uint32_t itemSize)
{
    if (!pList || !pBuffer)
        return -EINVAL;

    pList->pData = (uint8_t*)pBuffer;
    pList->itemSize = itemSize;
    pList->listSize = listSize;
    pList->currentSize = 0;
    pList->rwPosition = 0;
    pList->flags = 0;

    return 0;
}

int ListUnsafeInit(ListHandle_st* pList, uint32_t listSize, uint32_t itemSize)
{
    if (!pList)
        return -EINVAL;

    pList->pData = (uint8_t*) pvPortMalloc(listSize * itemSize);
    pList->itemSize = itemSize;
    pList->listSize = listSize;
    pList->currentSize = 0;
    pList->rwPosition = 0;
    pList->flags = 1;

    return 0;
}

int ListUnsafeDestroy(ListHandle_st* pList)
{
    if (!pList)
        return -EINVAL;

    pList->listSize = 0;
    pList->itemSize = 0;

    if (pList->flags & 0x01)
        vPortFree(pList->pData);

    return 0;
}

int ListUnsafePushBack(ListHandle_st* pList, void* pItem)
{
    if (!pList || !pItem)
        return -EINVAL;

    if (pList->currentSize >= pList->listSize)
        return -ENOSPC;

    ListUnsafeInternalSet(pList, pList->currentSize, pItem);
    pList->currentSize++;

    return 0;
}

int ListUnsafeSet(ListHandle_st* pList, uint32_t index, void* pItem)
{
    if (!pList || !pItem)
        return -EINVAL;

    if (index >= pList->listSize)
        return -EPERM;

    ListUnsafeInternalSet(pList, index, pItem);

    return 0;
}

int ListUnsafeGet(ListHandle_st* pList, uint32_t index, void* pRet)
{
    if (!pList || !pRet)
        return -EINVAL;

    if (index >= pList->currentSize)
        return -EPERM;

    ListUnsafeInternalGet(pList, index, pRet);

    return 0;
}

int ListUnsafePopBack(ListHandle_st* pList, void* pRet)
{
    if (!pList || !pRet)
        return -EINVAL;

    if (pList->currentSize == 0)
        return -ENODATA;

    ListUnsafeInternalGet(pList, --pList->currentSize, pRet);

    return 0;
}

int ListUnsafeRemove(ListHandle_st* pList, uint32_t index)
{
    uint32_t i;

    if (!pList)
        return -EINVAL;

    if (index >= pList->currentSize)
        return -EPERM;

    if (pList->currentSize == (index - 1))
    {
        pList->currentSize--;
        return 0;
    }

    for (i = index; i < pList->currentSize - 1; ++i)
        ListUnsafeSwap(pList, i + 1, i);

    pList->currentSize--;

    return 0;
}

int ListUnsafeInsert(ListHandle_st* pList, uint32_t index, void* pItem)
{
    uint32_t i;

    if (!pList || !pItem)
        return -EINVAL;

    if (index >= pList->currentSize)
        return -EPERM;

    if (pList->currentSize == pList->listSize)
        return -ENOSPC;

    for (i = pList->currentSize; i != index; --i)
        ListUnsafeSwap(pList, i - 1, i);

    ListUnsafeInternalSet(pList, index, pItem);

    pList->currentSize++;

    return 0;
}

#ifdef ENABLE_TU
typedef struct
{
    char text[128];
    uint32_t textLen;
}msg_st;


#define MAX_NOF_MSGS 8
static msg_st allocatedMessages[MAX_NOF_MSGS];
static QueueHandle_st emptyList;
static QueueHandle_st usedList;

void producer_thread(void* args)
{
    //Since the queue is storing pointers to message frames, this variable will be used
    //To retrieve items from the queue
    msg_st* messageFrame;
    uint32_t messageCount = 0;
    while (1)
    {
        //Since QueuePop retrieves the item using a pointer, and we are trying to retrieve a pointer
        //The parameter passed to QueuePop is in this case, a pointer to a pointer
        QueuePop(&emptyList, &messageFrame, MAX_DELAY);
        sprintf(messageFrame->text, "Hello from produced thread | This is message: %lu", messageCount++);
        QueuePush(&usedList, &messageFrame, MAX_DELAY);

        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(500));
    }
}

void consumer_thread(void* args)
{
    msg_st* messageFrame;
    int ret;
    while (1)
    {
        //Try to retrieve an empty frame
        ret = QueuePop(&usedList, &messageFrame, MAX_DELAY);

        //If unable to retrieve an empty frame don't proceed
        if (ret < 0)
            continue;

        //Consume the message
        printf("%s\n", messageFrame->text);

        //After consuming, give the message frame back to the empty list
        //Again note, that a reference to the pointer is passed
        QueuePush(&emptyList, &messageFrame, MAX_DELAY);
    }
}
void tu_queue_ptr()
{
    //The queue will save references to msg_st, so the size of the item is the size of the pointer to said struct
    QueueInit(&emptyList, MAX_NOF_MSGS, sizeof(msg_st*));
    QueueInit(&usedList, MAX_NOF_MSGS, sizeof(msg_st*));

    for (int i = 0; i < MAX_NOF_MSGS; ++i)
    {
        //Insert all the empty message frames on the empty queue
        //Note that what is inserted to the queue is actually a pointer to the message frame
        msg_st* frame = &allocatedMessages[i];
        QueuePush(&emptyList, &frame, MAX_DELAY);
    }

    xTaskCreate(producer_thread, "Producer Thread", configMINIMAL_STACK_SIZE * 16, NULL, 0, NULL);
    xTaskCreate(consumer_thread, "Consumer Thread", configMINIMAL_STACK_SIZE * 16, NULL, 0, NULL);

    while (1) { THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(1000));}
}
#endif