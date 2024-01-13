#ifndef PTHREADSWRAPPER_CTHREAD_H
#define PTHREADSWRAPPER_CTHREAD_H

#include <pthread.h>

class CThread
{
public:
    /// \brief Default Constructor
    CThread();

    /// \brief Constructor that allows to create a task with a pre-defined priority
    /// \param threadPrio Priority to assign to the new task being created
    explicit CThread(int threadPrio);

    /// \brief Tries to join the thread associated with this object
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    /// \return -EPERM if thread was not yet started, 0 on success
    [[nodiscard]] int join() const;

    /// \brief Tries to detach the task associated with this object
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    /// \return -EPERM if thread was not yet started, 0 on success
    [[nodiscard]] int detach() const;

    /// \brief Starts the thread
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    void start();
private:
    pthread_t m_thread;
    pthread_attr_t m_attr;
    sched_param m_schParam;
    bool m_isReady;
    static void* internalRun(void*);
    virtual void* run(void*) = 0;
};

#endif
