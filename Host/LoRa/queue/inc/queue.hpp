#ifndef LORA_QUEUE_HPP
#define LORA_QUEUE_HPP

#include <queue>
#include "../../ptwrap/inc/cmutex.h"

template <typename T>
class CQueue
{
public:
    CQueue();
    void push(T obj);
    T pop();
    bool isEmpty();
private:
    std::queue<T> m_dataQueue;
    CMutex m_dataMutex;
};

template<typename T>
bool CQueue<T>::isEmpty()
{
    m_dataMutex.lock();
    bool aux = m_dataQueue.size() == 0;
    m_dataMutex.unlock();

    return aux;
}

template<typename T>
T CQueue<T>::pop()
{
    m_dataMutex.lock();
    T aux = m_dataQueue.pop();
    m_dataMutex.unlock();
    return aux;
}

template<typename T>
void CQueue<T>::push(T obj)
{
    m_dataMutex.lock();
    m_dataQueue.push(obj);
    m_dataMutex.unlock();
}

template<typename T>
CQueue<T>::CQueue()
{

}


#endif
