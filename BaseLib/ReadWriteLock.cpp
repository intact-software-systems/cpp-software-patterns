#include "BaseLib/ReadWriteLock.h"
#include "BaseLib/Exception.h"
#include "BaseLib/Debug.h"
#include "BaseLib/Thread.h"

namespace BaseLib
{

#ifdef USE_GCC
/**
 * The pthread_rwlock_rdlock() and pthread_rwlock_tryrdlock() functions may fail if:
 *  [EINVAL]  - The value specified by rwlock does not refer to an initialised read-write lock object.
 *  [EDEADLK] - The current thread already owns the read-write lock for writing.
 *  [EAGAIN]  - The read lock could not be acquired because the maximum number of read locks for rwlock has been exceeded.
 */
std::string interpretPthreadReturn(int status)
{
    std::stringstream err;
    switch(status)
    {
        case EINVAL:
            err << "[EINVAL]  - The value specified by rwlock does not refer to an initialised read-write lock object.";
            break;
        case EDEADLK:
            err << "[EDEADLK] - The current thread already owns the read-write lock for writing.";
            break;
        case EAGAIN:
            err << "[EAGAIN]  - The read lock could not be acquired because the maximum number of read locks for rwlock has been exceeded.";
            break;
        default:
            err <<"Unknown error occurred!";
            break;
    }

    return err.str();
}
#endif

/**
 * @brief ReadWriteVariableHolder::ReadWriteVariableHolder
 */
ReadWriteVariableHolder::ReadWriteVariableHolder()
#ifdef USE_GCC
    : mutex_()
#endif
{
#ifdef USE_GCC
    if(pthread_rwlock_init(&mutex_, NULL))
        throw std::runtime_error("ReadWriteVariableHolder::ReadWriteVariableHolder(): Could not create mutex");
#else
    InitializeSRWLock(&criticalSection_);
#endif
}

/**
 * @brief ReadWriteVariableHolder::~ReadWriteVariableHolder
 */
ReadWriteVariableHolder::~ReadWriteVariableHolder()
{
#ifdef USE_GCC
    if(pthread_rwlock_destroy(&mutex_))
        throw std::runtime_error("ReadWriteVariableHolder::~ReadWriteVariableHolder(): Could not destroy mutex");
#else
    //Because SRWLs don't use any dynamically allocated events or memory internally, there is no need to delete them la ter on, and initialization ensures the right bit pattern is contained in memory.
#endif
}


/**
 * @brief ReadWriteLock::ReadWriteLock
 */
ReadWriteLock::ReadWriteLock(MutexPolicy policy)
    : policy_(policy)
    , mutexOwnerThreadId_(0)
    , accessCount_(0)
    , mutexHolder_(new ReadWriteVariableHolder())
{
}

/**
 * @brief ReadWriteLock::~ReadWriteLock
 */
ReadWriteLock::~ReadWriteLock()
{
}

/**
 * @brief ReadWriteLock::lockForRead
 */
void ReadWriteLock::lockForRead()
{
#ifdef USE_GCC
    int status = pthread_rwlock_rdlock(&mutexHolder_->GetMutex());
    if (status != 0)
    {
        std::stringstream err;
        err << "ReadWriteLock::lockForRead(): Could not lock mutex: " << interpretPthreadReturn(status);
        throw std::runtime_error(err.str());
    }
#else
    AcquireSRWLockShared(&mutexHolder_->GetMutex());
#endif

    ASSERT(accessCount_ != -1);
    ++accessCount_;

    if(Thread::isInterrupted())
    {
        throw InterruptedException("Thread was interrupted");
    }
}

/**
 * @brief ReadWriteLock::lockForWrite
 */
void ReadWriteLock::lockForWrite()
{
#ifdef USE_GCC
    int status = pthread_rwlock_wrlock(&mutexHolder_->GetMutex());
    if (status != 0)
    {
        std::stringstream err;
        err << "ReadWriteLock::lockForWrite(): Could not lock mutex: " << interpretPthreadReturn(status);
        throw std::runtime_error(err.str());
    }
#else
    AcquireSRWLockExclusive(&mutexHolder_->GetMutex());
#endif

    ASSERT(accessCount_ == 0);
    accessCount_ = -1;


    if(Thread::isInterrupted())
    {
        throw InterruptedException("Thread was interrupted");
    }
}

void ReadWriteLock::lock()
{
    return lockForWrite();
}

/**
 * @brief ReadWriteLock::tryLockForRead
 * @return
 */
bool ReadWriteLock::tryLockForRead()
{
    if(Thread::isInterrupted())
    {
        throw InterruptedException("Thread was interrupted");
    }

#ifdef USE_GCC
     int status = pthread_rwlock_tryrdlock(&mutexHolder_->GetMutex());
     if(status == 0)
     {
         ASSERT(accessCount_ != -1);
         ++accessCount_;

         return true;
     }
     else if(status == EBUSY)
     {
         return false;
     }
     else
     {
         ICRITICAL() << "returned unknown error code: " << status;
         throw std::runtime_error("ReadWriteLock::tryLockForRead(): ERROR! returned unknown error code!");
     }
 #else
     int status = TryAcquireSRWLockShared(&mutexHolder_->GetMutex());
     if(status != 0)
     {
         ASSERT(accessCount_ != -1);
         ++accessCount_;

         return true;
     }
     else
     {
         return false;
     }
 #endif
     return false;
}

/**
 * @brief ReadWriteLock::tryLockForRead
 * @param timeout
 * @return
 */
/*bool ReadWriteLock::tryLockForRead(int timeout)
{
     return false;
}*/

/**
 * @brief ReadWriteLock::tryLockForWrite
 * @return
 */
bool ReadWriteLock::tryLockForWrite()
{
    if(Thread::isInterrupted())
    {
        throw InterruptedException("Thread was interrupted");
    }

#ifdef USE_GCC
     int status = pthread_rwlock_trywrlock(&mutexHolder_->GetMutex());
     if(status == 0)
     {
         ASSERT(accessCount_ == 0);
         accessCount_ = -1;

         return true;
     }
     else if(status == EBUSY)
     {
         ASSERT(accessCount_ > 0);
         return false;
     }
     else
     {
         ICRITICAL() << "returned unknown error code: " << status;
         throw std::runtime_error("ReadWriteLock::tryLockForWrite(): ERROR! returned unknown error code!");
     }
 #else
     BOOLEAN status = TryAcquireSRWLockExclusive(&mutexHolder_->GetMutex());
     if(status != 0)
     {
         ASSERT(accessCount_ == 0);
         accessCount_ = -1;

         return true;
     }
     else
     {
         ASSERT(accessCount_ > 0);
         return false;
     }
 #endif
     return false;
}

/**
 * @brief ReadWriteLock::tryLockForWrite
 * @param timeout
 * @return
 */
/*bool ReadWriteLock::tryLockForWrite(int timeout)
{
    return false;
}*/

/**
 * @brief ReadWriteLock::unlock
 */
void ReadWriteLock::unlock()
{
#ifdef WIN32
    int prevAccessCount = accessCount_;
#endif

    if(accessCount_ == -1) accessCount_ = 0;
    else --accessCount_;

    ASSERT(accessCount_ >= 0);

#ifdef USE_GCC
    int status = pthread_rwlock_unlock(&mutexHolder_->GetMutex());
    if (status != 0)
        throw std::runtime_error("ReadWriteLock::unlock(): Could not unlock mutex");
#else
    if(prevAccessCount > 0)
        ReleaseSRWLockShared(&mutexHolder_->GetMutex());
    else if(prevAccessCount == -1)
        ReleaseSRWLockExclusive(&mutexHolder_->GetMutex());
    else // accessCount_ == 0 || accessCount_ < -1
        throw std::runtime_error("ReadWriteLock::unlock(): Could not unlock mutex. Access count was 0!");
#endif
}

/**
 * @brief ReadWriteLock::unlockRead
 */
/*void ReadWriteLock::unlockRead()
{
    ASSERT(accessCount_ > 0);

#ifdef USE_GCC
    int status = pthread_rwlock_unlock(&mutexHolder_->GetMutex());
    if (status != 0)
        throw std::runtime_error("ReadWriteLock::unlock(): Could not unlock mutex");
#else
    ReleaseSRWLockShared(&mutexHolder_->GetMutex());
#endif

    accessCount_--;

    ASSERT(accessCount_ > -1);
}*/

/**
 * @brief ReadWriteLock::unlockWrite
 */
/*void ReadWriteLock::unlockWrite()
{
    ASSERT(accessCount_ == -1);

#ifdef USE_GCC
    int status = pthread_rwlock_unlock(&mutexHolder_->GetMutex());
    if (status != 0)
        throw std::runtime_error("ReadWriteLock::unlock(): Could not unlock mutex");
#else
    ReleaseSRWLockExclusive(&mutexHolder_->GetMutex());
#endif

    accessCount_ = 0;
}*/

}
