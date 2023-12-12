#include <stdexcept>
#include "../inc/cmutex.h"

CMutex::CMutex()
{
    pthread_mutexattr_init(&m_attr);
    if (pthread_mutex_init(&m_mutex, &m_attr) != 0)
        throw std::runtime_error("Unable to create mutex!");
}

void CMutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}

bool CMutex::try_lock()
{
    return (pthread_mutex_trylock(&m_mutex) == 0);
}

void CMutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

CMutex::~CMutex()
{
    this->unlock();
    pthread_mutex_destroy(&m_mutex);
}
