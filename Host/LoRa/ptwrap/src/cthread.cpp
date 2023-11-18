#include "../inc/cthread.h"

CThread::CThread() : m_thread(), m_attr()
{

}

void CThread::start()
{
    pthread_create(&m_thread, &m_attr, CThread::internalRun, this);
}

void *CThread::internalRun(void* args)
{
    auto thread = static_cast<CThread*>(args);
    return thread->run(args);
}

void CThread::join() const
{
    pthread_join(m_thread, nullptr);
}

void CThread::detach() const
{
    pthread_detach(m_thread);
}



