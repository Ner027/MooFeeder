#ifndef PTHREADSWRAPPER_CMUTEX_H
#define PTHREADSWRAPPER_CMUTEX_H

#include <pthread.h>

class CMutex
{
public:
    /// \brief Default Constructor
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    CMutex();

    /// \brief Destructor
    ~CMutex();

    /// \brief Locks the mutex associated with this object
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    void lock();

    /// \brief Tries to the mutex associated with this object
    /// \return True if able to lock the mutex, otherwise returns false
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    bool try_lock();

    /// \brief Unlocks the mutex associated with this object
    /// \throw std::runtime_error if internal POSIX Threads error occurs
    void unlock();
private:
    pthread_mutex_t m_mutex;
    pthread_mutexattr_t m_attr;
};


#endif
