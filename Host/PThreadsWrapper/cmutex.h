#ifndef PTHREADSWRAPPER_CMUTEX_H
#define PTHREADSWRAPPER_CMUTEX_H

#include <pthread.h>

class CMutex
{
public:
    CMutex();
    ~CMutex();
    void lock();
    bool try_lock();
    void unlock();
private:
    pthread_mutex_t m_mutex;
    pthread_mutexattr_t m_attr;
};


#endif
