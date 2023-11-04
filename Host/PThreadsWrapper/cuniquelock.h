#ifndef PTHREADSWRAPPER_CUNIQUELOCK_H
#define PTHREADSWRAPPER_CUNIQUELOCK_H


#include "cmutex.h"

class CUniqueLock
{
public:
    explicit CUniqueLock(CMutex& mutex);
    ~CUniqueLock();
private:
    CMutex& m_mutex;
};


#endif
