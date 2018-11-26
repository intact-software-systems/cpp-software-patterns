#include "BaseLib/MutexBare.h"
#include "BaseLib/Debug.h"
#include "BaseLib/ElapsedTimer.h"
#include "BaseLib/Thread.h"

namespace BaseLib
{

// --------------------------------------------------------------
// MutexVariableHolderBare
// --------------------------------------------------------------

MutexVariableHolderBare::MutexVariableHolderBare()
{
#ifdef USE_GCC
    if (pthread_mutex_init(&mutex_, NULL))
        throw std::runtime_error("MutexBare::MutexBare(): Could not create mutex");
#else
    InitializeCriticalSection(&criticalSection_);
#endif
}

MutexVariableHolderBare::~MutexVariableHolderBare()
{
#ifdef USE_GCC
    if(pthread_mutex_destroy(&mutex_))
    {
        //ASSERT(0);
        ICRITICAL() << "MutexBare::~MutexBare(): Could not destroy mutex";
        throw std::runtime_error("MutexBare::~MutexBare(): Could not destroy mutex");
    }
#else
    DeleteCriticalSection(&criticalSection_);
#endif
}

// --------------------------------------------------------------
// MutexBare
// --------------------------------------------------------------

MutexBare::MutexBare(MutexPolicy policy)
    : policy_(policy)
    , mutexHolder_(new MutexVariableHolderBare())
{ }

MutexBare::MutexBare(const MutexBare &other)
    : policy_(other.policy_)
    , mutexHolder_(new MutexVariableHolderBare())
{
    if(other.policy_.IsShared()) {
        mutexHolder_ = other.mutexHolder_->shared_from_this();
    }
}

MutexBare::~MutexBare()
{ }

MutexBare &MutexBare::operator=(const MutexBare &)
{
    return *this;
}

void MutexBare::lock()
{
#ifdef USE_GCC
    int status = pthread_mutex_lock(&mutexHolder_->GetMutex());
    if (status != 0)
    {
        ASSERT(status == 0);
        throw std::runtime_error("MutexBare::lock(): Could not lock mutex");
    }
#else
    EnterCriticalSection(&mutexHolder_->GetMutex());
#endif
}

void MutexBare::unlock()
{
#ifdef USE_GCC
    int status = pthread_mutex_unlock(&mutexHolder_->GetMutex());
    if (status != 0)
    {
        ASSERT(status == 0);
        throw std::runtime_error("MutexBare::unlock(): Could not unlock mutex");
    }
#else
    LeaveCriticalSection(&mutexHolder_->GetMutex());
#endif
}

/**
 * TODO: Waiting is not yet implemented!
 */
bool MutexBare::tryLock(int64 milliseconds)
{
    if(milliseconds != 0)
    {
        ElapsedTimer timer(milliseconds);

        bool isLocked = false;
        do
        {
            isLocked = tryLockPrivate();

            if(!isLocked)
            {
                int waitTimeMs = (int) std::min<int64>(300, timer.Remaining().InMillis());
                Thread::msleep(waitTimeMs);
            }
        }
        while(!isLocked && !timer.HasExpired());

        if(isLocked)
        {
            return isLocked;
        }
        else
        {
            ICRITICAL() << "Timed out waiting for lock!";
            return false;
        }
    }

    return tryLockPrivate();
}

bool MutexBare::isDebug() const
{
    return false;
}

bool MutexBare::tryLockPrivate()
{
#ifdef USE_GCC
    int status = pthread_mutex_trylock(&mutexHolder_->GetMutex());
    if(status == 0)
    {
        return true;
    }
    else if(status == EBUSY)
    {
        return false;
    }
    else
    {
        std::cerr << "returned unknown error code: " << status;
        throw std::runtime_error("MutexBare::tryLock(): ERROR! returned unknown error code: "); // + string(status));
    }
#else
    int isAcquired = TryEnterCriticalSection(&mutexHolder_->GetMutex());
    if(isAcquired != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
#endif

    return false;
}

}
