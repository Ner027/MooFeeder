#include <stdexcept>
#include "ptwrapper/cthread.h"

CThread::CThread() : m_thread(), m_attr(), m_schParam()
{
    int ret = pthread_attr_init(&m_attr);

    if (ret != 0)
        throw std::runtime_error("Unable to initialize thread attributes!");

    m_isReady = false;
}

void CThread::start()
{
    int ret = pthread_create(&m_thread, &m_attr, CThread::internalRun, this);
    if (ret != 0)
        throw std::runtime_error("Unable to create thread!");

    m_isReady = true;
}

void* CThread::internalRun(void* args)
{
    auto thread = static_cast<CThread*>(args);
    return thread->run(args);
}

int CThread::join() const
{
    int ret;

    if (!m_isReady)
        return -EPERM;

    ret = pthread_join(m_thread, nullptr);
    if (ret != 0)
        throw std::runtime_error("Unable to join thread!");

    return 0;
}

int CThread::detach() const
{
    int ret;

    if (!m_isReady)
        return -EPERM;

    ret = pthread_detach(m_thread);
    if (ret != 0)
        throw std::runtime_error("Unable to detach thread!");

    return 0;
}

CThread::CThread(int threadPrio) : CThread()
{
    int ret = pthread_attr_getschedparam(&m_attr, &m_schParam);
    if (ret != 0)
        throw std::runtime_error("Unable to acquire thread scheduling parameters!");

    m_schParam.sched_priority = threadPrio;

    ret = pthread_attr_setschedparam(&m_attr, &m_schParam);
    if (ret != 0)
        throw std::runtime_error("Unable to assign thread scheduling parameters!");
}



