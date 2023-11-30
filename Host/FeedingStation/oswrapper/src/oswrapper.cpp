#include <cerrno>
#include <iostream>
#include "../inc/oswrapper.h"

#include <sstream>

int QueueFreeSpace(QueueHandle_st *queueHandle)
{
    int size;

    if (!queueHandle)
        return -EINVAL;

    {
        std::lock_guard<std::mutex> lock(*queueHandle->mutex);
        size = (int) (queueHandle->maxSize - queueHandle->currentSize);
    }

    return size;
}

int QueueInit(QueueHandle_st *queueHandle, uint32_t queueSize, uint32_t itemSize)
{
    if (!queueHandle)
        return -EINVAL;

    queueHandle->maxSize = queueSize;
    queueHandle->itemSize = itemSize;
    queueHandle->currentSize = 0;
    queueHandle->currentPosition = 0;

    queueHandle->mutex = new std::mutex;
    queueHandle->cv = new std::condition_variable;
    queueHandle->queue = new uint8_t[itemSize * queueSize];


#ifdef ENABLE_TU
    printf("Queue Allocated Bytes: %d\n", itemSize * queueSize);
#endif
    return 0;
}

int QueueDestroy(QueueHandle_st *queueHandle)
{
    if (!queueHandle)
        return -EINVAL;

    delete queueHandle->queue;
    delete queueHandle->mutex;
    delete queueHandle->cv;

    return 0;
}

void QueueInternalPush(QueueHandle_st *queueHandle, void *item)
{
    auto *ptr = (uint8_t *) item;

    for (uint32_t i = 0; i < queueHandle->itemSize; ++i)
        queueHandle->queue[queueHandle->currentPosition++] = *ptr++;

    queueHandle->currentSize++;
}

int QueuePush(QueueHandle_st *queueHandle, void *item, duration_t timeToWait)
{
    std::cv_status status = std::cv_status::no_timeout;
    systick_t startTime = GET_CURRENT_TIME();

    if (!queueHandle || !item)
        return -EINVAL;

    std::unique_lock<std::mutex> lock(*queueHandle->mutex);

    while (((queueHandle->maxSize - queueHandle->currentSize) <= 0) && (status != std::cv_status::timeout))
        status = queueHandle->cv->wait_until(lock, startTime + timeToWait);

    if (status == std::cv_status::timeout)
        return -ETIMEDOUT;

    QueueInternalPush(queueHandle, item);
    queueHandle->cv->notify_one();

    return 0;
}

void QueueInternalPop(QueueHandle_st *queueHandle, void *item)
{
    uint32_t aux;
    auto *ptr = (uint8_t *) item;

    queueHandle->currentPosition -= queueHandle->itemSize;
    aux = queueHandle->currentPosition;

    for (uint32_t i = 0; i < queueHandle->itemSize; ++i)
        *ptr++ = queueHandle->queue[aux++];

    queueHandle->currentSize--;
}

int QueuePop(QueueHandle_st* queueHandle, void* dest, duration_t timeToWait)
{
    systick_t startTime;
    std::cv_status status = std::cv_status::no_timeout;

    if (!queueHandle || !dest)
        return -EINVAL;

    if (timeToWait != MAX_DELAY)
        startTime = GET_CURRENT_TIME();
    else startTime = std::chrono::system_clock::from_time_t(0);

    std::unique_lock<std::mutex> lock(*queueHandle->mutex);
    status = std::cv_status::no_timeout;
    while ((queueHandle->currentSize == 0) && ((status != std::cv_status::timeout) || (timeToWait == MAX_DELAY)))
        status = queueHandle->cv->wait_until(lock, startTime + timeToWait);

    if (status == std::cv_status::timeout)
        return -ETIMEDOUT;

    QueueInternalPop(queueHandle, dest);

    return 0;
}

void QueueInternalPeek(QueueHandle_st *queueHandle, void *item)
{
    uint32_t aux = queueHandle->currentPosition;
    auto *ptr = (uint8_t *) item;
    ptr += queueHandle->itemSize;

    for (uint32_t i = 0; i < queueHandle->itemSize; ++i)
        *--ptr = queueHandle->queue[aux--];
}

int QueuePeek(QueueHandle_st *queueHandle, void *dest, duration_t timeToWait)
{
    std::cv_status status = std::cv_status::no_timeout;
    systick_t startTime = GET_CURRENT_TIME();

    if (!queueHandle || !dest)
        return -EINVAL;

    std::unique_lock<std::mutex> lock(*queueHandle->mutex);
    while ((queueHandle->currentSize <= 0) && (status != std::cv_status::timeout))
        status = queueHandle->cv->wait_until(lock, startTime + timeToWait);

    if (status == std::cv_status::timeout)
        return -ETIMEDOUT;

    QueueInternalPeek(queueHandle, dest);

    return 0;
}

int MutexInit(Mutex_t *mutex)
{
    if (!mutex)
        return -EINVAL;

    *mutex = new std::mutex;

    return 0;
}

int MutexDestroy(Mutex_t *mutex)
{
    if (!mutex)
        return -EINVAL;

    delete *mutex;
    *mutex = nullptr;

    return 0;
}

int MutexLock(Mutex_t *mutex, duration_t maxWaitTime)
{
    systick_t startTime;
    bool result = false;

    if (!mutex)
        return -EINVAL;

    startTime = GET_CURRENT_TIME();

    while (!result && ((GET_CURRENT_TIME() - startTime) < maxWaitTime))
        result = (*mutex)->try_lock();

    if (!result)
        return -ETIMEDOUT;

    return 0;
}

int MutexUnlock(Mutex_t *mutex)
{
    if (!mutex)
        return -EINVAL;

    (*mutex)->unlock();

    return 0;
}

int SemaphoreInit(SemaphoreHandle_st *p_sem)
{
    if (!p_sem)
        return -EINVAL;

    std::unique_lock<std::mutex> Lock(p_sem->SmMutex);
    p_sem->StatusRunning = true;
    p_sem->CountWaiting = 0;
    p_sem->SmCount = 1;

    return 0;
}

int SemaphoreDestroy(SemaphoreHandle_st *p_sem)
{
    if (!p_sem)
        return -EINVAL;

    std::unique_lock<std::mutex> Lock(p_sem->SmMutex);
    p_sem->StatusRunning = false;
    p_sem->SmCond.notify_all();

    return 0;
}

int SemaphoreTake(SemaphoreHandle_st *p_sem)
{
    if (!p_sem)
        return -EINVAL;

    std::unique_lock<std::mutex> Lock(p_sem->SmMutex);
    while (p_sem->StatusRunning && p_sem->SmCount == 0)
    {
        p_sem->CountWaiting++;
        p_sem->SmCond.wait(Lock, [p_sem]()
        { return p_sem->SmCount > 0 || !p_sem->StatusRunning; });
        p_sem->CountWaiting--;
    }
    p_sem->SmCount--;

    return p_sem->SmCount;
}

int SemaphoreGive(SemaphoreHandle_st *p_sem)
{
    if (!p_sem)
        return -EINVAL;

    std::unique_lock<std::mutex> Lock(p_sem->SmMutex);
    p_sem->SmCount++;
    if (p_sem->CountWaiting)
        p_sem->SmCond.notify_one();

    return p_sem->SmCount;
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

int ListUnsafeInit(ListHandle_st* pList, uint8_t* pBuffer, uint32_t listSize, uint32_t itemSize)
{
    if (!pList || !pBuffer)
        return -EINVAL;

    pList->pData = pBuffer;
    pList->itemSize = itemSize;
    pList->listSize = listSize;
    pList->currentSize = 0;
    pList->rwPosition = 0;

    return 0;
}

int ListUnsafeDestroy(ListHandle_st* pList)
{
    if (!pList)
        return -EINVAL;

    pList->listSize = 0;
    pList->itemSize = 0;

    return 0;
}

/// \brief This functions pushes an item to the end a list
/// \param pList Pointer to list handler
/// \param pItem Pointer to item to be insert
/// \return -EINVAL if invalid parameters are passed, -ENOSPC if the list is full, returns 0 on success
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
void th_consumer(SemaphoreHandle_st *p_sem, int id, int *p_count)
{
    std::stringstream msg;
    msg << "Thread ready " << id << std::endl;
    std::cout << msg.str();
    SemaphoreTake(p_sem);
    (*p_count)--;
    msg.str("");
    msg << "Thread released " << id << std::endl;
    std::cout << msg.str();
}

void tu_semaphores(void)
{
    std::thread Tx[10];
    SemaphoreHandle_st S1;
    int th_id = 0, count = 10;
    SemaphoreInit(&S1);
    for (th_id = 0; th_id < 10; th_id++)
    {
        Tx[th_id] = std::thread(th_consumer, &S1, th_id, &count);
    }

    std::this_thread::sleep_for(std::chrono::seconds(15));
    while (count > 0)
    {
        SemaphoreGive(&S1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void tu_queues_mt()
{
    QueueHandle_st queue{};
    srand(0);
    QueueInit(&queue, 8, sizeof(uint32_t));
    std::thread([&queue]()
                {
                    uint32_t rand;
                    while (1)
                    {
                        QueuePush(&queue, &rand, MAX_DELAY);
                        std::cout << "Thread push " << rand << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                }).detach();

    std::thread([&queue]()
                {
                    uint32_t dest;
                    while (1)
                    {
                        QueuePop(&queue, &dest, MAX_DELAY);
                        std::cout << "Thread pop: " << dest << std::endl;
                    }
                }).join();
}

void tu_queues()
{
    uint32_t item;
    QueueHandle_st queue{};

    QueueInit(&queue, 8, sizeof(uint32_t));

    item = 0xDEADC0DE;
    QueuePush(&queue, &item, MAX_DELAY);
    printf("Pushed %x\n", item);

    item = 0xFAFEDEAD;
    QueuePush(&queue, &item, MAX_DELAY);
    printf("Pushed %x\n", item);

    item = 0xDEADBEEF;
    QueuePush(&queue, &item, MAX_DELAY);
    printf("Pushed %x\n", item);

    item = 0x00C0FFEE;
    QueuePush(&queue, &item, MAX_DELAY);
    printf("Pushed %x\n", item);

    while (queue.currentSize != 0)
    {
        QueuePop(&queue, &item, MAX_DELAY);
        printf("Popped %x\n", item);
    }
}

typedef struct
{
    char text[128];
    uint8_t textLen;
}msg_st;


#define MAX_NOF_MSGS 8
static msg_st allocatedMessages[MAX_NOF_MSGS];

void tu_queue_ptr()
{
    QueueHandle_st emptyList = {};
    QueueHandle_st usedList = {};

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

    //Producer Thread
    std::thread([&emptyList, &usedList]()
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
                        snprintf(messageFrame->text, 128,
                                 "Hello from producer thread, this is message: %d", messageCount++);

                        QueuePush(&usedList, &messageFrame, MAX_DELAY);

                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                }).detach();

    //Consumer Thread
    std::thread([&emptyList, &usedList]()
                {
                    //As in the producer thread, since we are actually retrieving pointers to statically allocated
                    //Message frames, a pointer to a message frame is used
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
                }).join();
}

uint8_t listBuffer[sizeof(msg_st*) * MAX_NOF_MSGS];

void tu_list()
{
    ListHandle_st frameList;
    ListUnsafeInit(&frameList, listBuffer, MAX_NOF_MSGS, sizeof(msg_st*));

    for (int i = 0; i < MAX_NOF_MSGS; ++i)
    {
        msg_st* msg = &allocatedMessages[i];
        ListUnsafePushBack(&frameList, &msg);
        printf("Inserting to list: %p\n", msg);
    }

    msg_st* pMsg = (msg_st*) 0xFAFEDEAD;

    ListUnsafePushBack(&frameList, &pMsg);

    ListUnsafeRemove(&frameList, 2);
    ListUnsafeInsert(&frameList, 2, &pMsg);
    ListUnsafeInsert(&frameList, 0, &pMsg);

    for (int i = 0; (unsigned) i < frameList.currentSize; ++i)
    {
        msg_st* msg;
        ListUnsafeGet(&frameList, i, &msg);
        printf("Object at index %d is %p\n", i, msg);
    }
}

#endif
