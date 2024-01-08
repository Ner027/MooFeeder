#include "ptwrapper/clockguard.h"

CLockGuard::CLockGuard(CMutex& mutex) : m_mutex(mutex)
{
    mutex.lock();
}

CLockGuard::~CLockGuard()
{
    m_mutex.unlock();
}
