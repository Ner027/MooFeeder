#ifndef PTHREADSWRAPPER_CUNIQUELOCK_H
#define PTHREADSWRAPPER_CUNIQUELOCK_H


#include "cmutex.h"

class CLockGuard
{
public:
    explicit CLockGuard(CMutex& mutex);
    ~CLockGuard();
private:
    CMutex& m_mutex;
};


#endif
