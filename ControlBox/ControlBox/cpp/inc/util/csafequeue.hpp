#ifndef LORA_QUEUE_HPP
#define LORA_QUEUE_HPP

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <queue>
#include <memory.h>
#include "ptwrapper/cmutex.h"
#include "ptwrapper/clockguard.h"


template <class T>
class CQueue
{
public:
    CQueue();
    void push(T obj);
    bool try_pop(T& ret);
    bool isEmpty();
private:
    std::queue<T> m_dataQueue;
    CMutex m_dataMutex;
    std::condition_variable m_cv;
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
bool CQueue<T>::try_pop(T& ret)
{
    CLockGuard lock(m_dataMutex);

    if (m_dataQueue.empty())
        return false;

    ret = m_dataQueue.front();
    m_dataQueue.pop();

    return true;
}

template<class T>
void CQueue<T>::push(T obj)
{
    m_dataMutex.lock();
    m_dataQueue.push(obj);
    m_dataMutex.unlock();
}

template<class T>
CQueue<T>::CQueue() {}

#endif
