#include "../inc/cuniquelock.h"

CUniqueLock::CUniqueLock(CMutex& mutex) : m_mutex(mutex)
{
    mutex.lock();
}

CUniqueLock::~CUniqueLock()
{
    m_mutex.unlock();
}
