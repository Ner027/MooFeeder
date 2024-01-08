#ifndef OS_WRAPPER_H
#define OS_WRAPPER_H

//#define ENABLE_TU

#define HOST

#ifdef HOST
#include <queue>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

struct SemaphoreHandle_st
{
    std::mutex SmMutex;
    std::condition_variable SmCond;
    int SmCount;
    bool StatusRunning;
    int CountWaiting;
};

struct QueueHandle_st
{
    std::mutex* mutex;
    std::condition_variable* cv;
    uint8_t* queue;
    uint32_t maxSize;
    uint32_t itemSize;
    uint32_t currentSize;
    uint32_t currentPosition;
};
typedef std::thread Thread_t;
typedef std::mutex* Mutex_t;

typedef std::chrono::system_clock::duration duration_t;
typedef std::chrono::system_clock::time_point systick_t;

#define THREAD_EXIT() return
#define GET_CURRENT_TIME() std::chrono::system_clock::now()
#define THREAD_SLEEP_FOR(x) std::this_thread::sleep_for((x))
#define SYSTEM_TICK_FROM_MS(x) std::chrono::milliseconds((x))
#define SYSTEM_TICK_FROM_SEC(x) std::chrono::seconds((x))
#define MS_FROM_SYSTEM_TICK(x) std::chrono::duration_cast<std::chrono::milliseconds>((x))
#define TIME_POINT(x) std::chrono::time_point<std::chrono::system_clock>(SYSTEM_TICK_FROM_MS((x)))
#define MS_FROM_DURATION(x) ((std::chrono::milliseconds((x).count())).count())
#define ENUM1B(name) enum name : unsigned char
#define MAX_DELAY (std::chrono::time_point<std::chrono::system_clock>::max().time_since_epoch())
#elif defined(TARGET)
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

typedef QueueHandle_t QueueHandle_st;
typedef SemaphoreHandle_t Mutex_t;
typedef SemaphoreHandle_t SemaphoreHandle_st;
typedef uint32_t duration_t;
typedef uint32_t systick_t;
#define GET_CURRENT_TIME() xTaskGetTickCount()
#define THREAD_SLEEP_FOR(x) vTaskDelay((x))
#define SYSTEM_TICK_FROM_MS(x) ((x) / portTICK_PERIOD_MS)
#define SYSTEM_TICK_FROM_SEC(x) ((x * 1000) / portTICK_PERIOD_MS)
#define MS_FROM_SYSTEM_TICK(x) ((x) * portTICK_PERIOD_MS)
#define THREAD_EXIT() vTaskDelete(NULL)
#define ENUM1B(name) enum __attribute__ ((__packed__)) name
#define MAX_DELAY portMAX_DELAY
#define null NULL
#endif

typedef struct
{
    uint32_t itemSize;
    uint32_t listSize;
    uint32_t currentSize;
    uint32_t rwPosition;
    uint32_t flags;
    uint8_t* pData;
}ListHandle_st;

/// \brief This function initializes a queue allocating all the resources it needs
/// \param queueHandle Pointer to a queue handler
/// \param queueSize Maximum amount of items that the queue can contain
/// \param itemSize Size (in bytes) of each queue item
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int QueueInit(QueueHandle_st* queueHandle, uint32_t queueSize, uint32_t itemSize);

/// \brief This function destroys a queue freeing its previously allocated resources
/// \param queueHandle Pointer to a queue handler
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int QueueDestroy(QueueHandle_st* queueHandle);

/// \brief This function tries to insert a new item onto a queue
/// \param queueHandle Pointer to a queue handler
/// \param item Item to insert in the queue
/// \param timeToWait Maximum time to wait for inserting the item in the event that the queue is full
/// \return -EINVAL if invalid parameters are passed, -ETIMEDOUT if unable to insert the item in the given time,
/// returns 0 on success
int QueuePush(QueueHandle_st* queueHandle, void* item, duration_t timeToWait);

/// \brief This function tries to pop an item from a queue
/// \param queueHandle Pointer to a queue handler
/// \param dest Pointer to where the item should be returned to
/// \param timeToWait Maximum time to wait for retrieving an item
/// \return -EINVAL if invalid parameters are passed, -ETIMEDOUT if unable to retrieve an item in the given time,
/// returns 0 on success
int QueuePop(QueueHandle_st* queueHandle, void* dest, duration_t timeToWait);

/// \brief This function allows to retrieve an item from a queue without removing it from said queue
/// \param queueHandle Pointer to a queue handler
/// \param dest Pointer to where the item should be return to
/// \param timeToWait Maximum time to wait for retrieving an item
/// \return -EINVAL if invalid parameters are passed, -ETIMEDOUT if unable to retrieve an item in the given time,
/// returns 0 on success
int QueuePeek(QueueHandle_st* queueHandle, void* dest, duration_t timeToWait);

/// \brief This functions retrieves the number of free positions in the queue
/// \param queueHandle Pointer to a queue handler
/// \return -EINVAL if invalid parameters are passed, otherwise returns the number of free spaces
int QueueFreeSpace(QueueHandle_st* queueHandle);

/// \brief This function initializes a mutex allocating the resources needed by it
/// \param mutex Pointer to a mutex handler
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int MutexInit(Mutex_t* mutex);

/// \brief This functions destroys a mutex freeing all resources previously allocated by ut
/// \param mutex Pointer to a mutex handler
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int MutexDestroy(Mutex_t* mutex);

/// \brief This function tries to lock a mutex
/// \param mutex Pointer to a mutex handler
/// \param maxWaitTime Max time to wait for locking the mutex
/// \return -EINVAL if invalid parameters are passed, -ETIMEDOUT if failed to acquire the mutex in the given time,
/// returns 0 on success
int MutexLock(Mutex_t* mutex, duration_t maxWaitTime);

/// \brief This function unlocks a mutex
/// \param mutex Pointer to a mutex handler
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int MutexUnlock(Mutex_t* mutex);

/// \brief This function initializes a semaphore allocating all the resources needed by it
/// \param p_sem Pointer to semaphore handle
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int SemaphoreInit(SemaphoreHandle_st* p_sem);

/// \brief This functions destroys a semaphore, freeing all the resources previously allocated by it
/// \param p_sem Pointer to semaphore handle
/// \return -EINVAL if invalid parameters are passed, returns 0 on success
int SemaphoreDestroy(SemaphoreHandle_st* p_sem);

/// \brief This functions takes a semaphore blocking until being able to take it
/// \param p_sem Pointer to semaphore handle
int SemaphoreTake(SemaphoreHandle_st *p_sem);

int SemaphoreGive(SemaphoreHandle_st *p_sem);

/// \brief This function initializes a list using a pre allocated buffer
/// \param pList Pointer to the list handler
/// \param pBuffer Pointer to the pre allocated buffer
/// \param listSize Max number of items the list can contain (Should be set set accordingly to the given buffer size)
/// \param itemSize Size (in bytes) of each of the lists item
/// \return -EINVAL if invalid parameters are passed, 0 on success
int ListUnsafeInitStatic(ListHandle_st* pList, void* pBuffer, uint32_t listSize, uint32_t itemSize);

int ListUnsafeInit(ListHandle_st* pList, uint32_t listSize, uint32_t itemSize);

/// \brief This function destroys a list by setting its size to 0
/// \param pList Pointer to the list handler
/// \return -EINVAL if invalid parameters are passed, 0 on success
int ListUnsafeDestroy(ListHandle_st* pList);

/// \brief This functions allows to set an item of the list at a given position
/// \param pList Pointer to the list handler
/// \param index List index to set the item to
/// \param pItem Item to write to the list
/// \return -EINVAL if invalid parameters are passed, -EPERM if trying to set an item that exceeds the list size,
/// returns 0 on success
int ListUnsafeSet(ListHandle_st* pList, uint32_t index, void* pItem);

/// \brief This function allows to retrieve an item from a list at a given position
/// \param pList Pointer to the list handler
/// \param index Index to retrieve the item from
/// \param pRet Pointer to return the read item to
/// \return -EINVAL if invalid parameters are passed, -EPERM if trying to access a position that exceeds the current
/// list size, returns 0 on success
int ListUnsafeGet(ListHandle_st* pList, uint32_t index, void* pRet);

/// \brief This function retries the last item of a list and removes it
/// \param pList Pointer to the list handler
/// \param pRet Pointer to return the read item to
/// \return -EINVAL if invalid parameters are passed, -ENODATA if list is empty, returns 0 on success
int ListUnsafePopBack(ListHandle_st* pList, void* pRet);

/// \brief This functions pushes an item to the end a list
/// \param pList Pointer to list handler
/// \param pItem Pointer to item to be insert
/// \return -EINVAL if invalid parameters are passed, -ENOSPC if the list is full, returns 0 on success
int ListUnsafePushBack(ListHandle_st* pList, void* pItem);

/// \brief This functions allows to remove an item from a list at a given position
/// \param pList Pointer to the list handler
/// \param index Index of the item to remove
/// \return -EINVAL if invalid parameters are passed, -EPERM if trying to remove an item at a position that exceeds the
/// current list size, returns 0 on success
int ListUnsafeRemove(ListHandle_st* pList, uint32_t index);

/// \brief This functions allows to insert an item in a list at a given position
/// \param pList Pointer to the list handler
/// \param index Index to insert the item in
/// \param pItem Pointer to the item that should be inserted
/// \return -EINVAL if invalid parameters are passed, -EPERM if trying to insert an item at an invalid position, -ENOSPC
/// if list is full, returns 0 on success
int ListUnsafeInsert(ListHandle_st* pList, uint32_t index, void* pItem);

#ifdef ENABLE_TU
void tu_semaphores();
void tu_queues();
void tu_queues_mt();
void tu_queue_ptr();
void tu_list();
#endif
#endif

