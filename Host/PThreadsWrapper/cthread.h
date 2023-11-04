#ifndef PTHREADSWRAPPER_CTHREAD_H
#define PTHREADSWRAPPER_CTHREAD_H

#include <pthread.h>

class CThread
{
public:
    CThread();
    void join() const;
    void detach() const;
private:
    pthread_t m_thread;
    pthread_attr_t m_attr;
    static void* internalRun(void*);
    virtual void* run(void*) = 0;
};

#endif
