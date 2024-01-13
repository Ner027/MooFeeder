#include <stdexcept>
#include "ptwrapper/cmutex.h"

CMutex::CMutex()
{
    int ret = pthread_mutexattr_init(&m_attr);
    if (ret != 0)
        throw std::runtime_error("Unable to create mutex attributes!");

    ret = pthread_mutex_init(&m_mutex, &m_attr);
    if (ret != 0)
        throw std::runtime_error("Unable to create mutex!");
}
CMutex::~CMutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void CMutex::lock()
{
    int ret = pthread_mutex_lock(&m_mutex);
    if (ret != 0)
        throw std::runtime_error("Unable to lock mutex!");
}

bool CMutex::try_lock()
{
    return (pthread_mutex_trylock(&m_mutex) == 0);
}

void CMutex::unlock()
{
    int ret = pthread_mutex_unlock(&m_mutex);
    if (ret != 0)
        throw std::runtime_error("Unable to unlock mutex!");
}

