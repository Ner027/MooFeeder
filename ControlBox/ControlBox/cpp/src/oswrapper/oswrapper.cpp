#include <cerrno>
#include <iostream>
#include "oswrapper/oswrapper.h"

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
    p_sem->SmCount = 0;

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
