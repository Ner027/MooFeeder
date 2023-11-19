#ifndef LORA_QUEUE_HPP
#define LORA_QUEUE_HPP

#include <queue>
#include "../../ptwrap/inc/cmutex.h"
#include "../../ptwrap/inc/cuniquelock.h"
#include <memory.h>

template <typename T>
class QueueItem
{
private:
    size_t itemSize;
public:
    T object;

    QueueItem()
    {
        itemSize = sizeof(T);
    }

    explicit QueueItem(T& src) : itemSize(sizeof(T))
    {
        object = src;
    }

    QueueItem& operator=(const QueueItem& src)
    {
        memcpy(&object, &src.object, itemSize);
        return *this;
    }
};

template <class T>
class CQueue
{
public:
    CQueue();
    void push(QueueItem<T> obj);
    bool pop(QueueItem<T>& ret);
    bool isEmpty();
private:
    std::queue<QueueItem<T>> m_dataQueue;
    CMutex m_dataMutex;
};

template<class T>
bool CQueue<T>::isEmpty()
{
    m_dataMutex.lock();
    bool aux = m_dataQueue.empty();
    m_dataMutex.unlock();

    return aux;
}

template<class T>
bool CQueue<T>::pop(QueueItem<T>& ret)
{
    CUniqueLock lock(m_dataMutex);
    if (m_dataQueue.empty())
        return false;

    ret = m_dataQueue.front();
    m_dataQueue.pop();

    return true;
}

template<class T>
void CQueue<T>::push(QueueItem<T> obj)
{
    m_dataMutex.lock();
    m_dataQueue.push(obj);
    m_dataMutex.unlock();
}

template<class T>
CQueue<T>::CQueue()
{

}

template class CQueue<phy_frame_st>;

#endif
