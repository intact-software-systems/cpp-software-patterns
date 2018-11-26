/***************************************************************************
                          Mutex.cpp  -  description
                             -------------------
    begin                : Sat Mar 12 2011
    copyright            : (C) 2011 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#include "BaseLib/Mutex.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/Thread.h"
#include "BaseLib/ThreadManager.h"

#include "BaseLib/Debug.h"
#include "BaseLib/Exception.h"

namespace BaseLib
{

// --------------------------------------------------------------
// MutexVariableHolder
// --------------------------------------------------------------

MutexVariableHolder::MutexVariableHolder()
{
#ifdef USE_GCC
    if (pthread_mutex_init(&mutex_, NULL))
        throw std::runtime_error("Mutex::Mutex(): Could not create mutex");
#else
    InitializeCriticalSection(&criticalSection_);
#endif
}

MutexVariableHolder::~MutexVariableHolder()
{
#ifdef USE_GCC
    if(pthread_mutex_destroy(&mutex_))
    {
        //ASSERT(0);
        ICRITICAL() << "Mutex::~Mutex(): Could not destroy mutex";
        throw std::runtime_error("Mutex::~Mutex(): Could not destroy mutex");
    }
#else
    DeleteCriticalSection(&criticalSection_);
#endif
}

// --------------------------------------------------------------
// Mutex
// --------------------------------------------------------------

Mutex::Mutex(MutexPolicy policy)
    : policy_(policy)
    , mutexOwnerThreadId_(0)
    , mutexHolder_(new MutexVariableHolder())
{ }

Mutex::Mutex(const Mutex &other)
    : policy_(other.policy_)
    , mutexOwnerThreadId_(0)
    , mutexHolder_(new MutexVariableHolder())
{
    if(other.policy_.IsShared()) {
        mutexHolder_ = other.mutexHolder_->shared_from_this();
    }
}

Mutex::~Mutex()
{ }

Mutex &Mutex::operator=(const Mutex &)
{
    return *this;
}

void Mutex::lock()
{
    Thread *self = debugMutexLockEntry();

#ifdef USE_GCC
    int status = pthread_mutex_lock(&mutexHolder_->GetMutex());
    if (status != 0)
    {
        ASSERT(status == 0);
        throw std::runtime_error("Mutex::lock(): Could not lock mutex");
    }
#else
    EnterCriticalSection(&mutexHolder_->GetMutex());
#endif

    debugMutexLockExit(self);

    if(Thread::isInterrupted())
    {
        throw InterruptedException("Thread was interrupted");
    }
}

void Mutex::unlock()
{
    Thread* self = debugMutexUnlockEntry();

#ifdef USE_GCC
    int status = pthread_mutex_unlock(&mutexHolder_->GetMutex());
    if (status != 0)
    {
        ASSERT(status == 0);
        throw std::runtime_error("Mutex::unlock(): Could not unlock mutex");
    }
#else
    LeaveCriticalSection(&mutexHolder_->GetMutex());
#endif

    debugMutexUnlockExit(self);
}

/**
 * TODO: Waiting is not yet implemented!
 */
bool Mutex::tryLock(int64 milliseconds)
{
    if(Thread::isInterrupted())
    {
        throw InterruptedException("Thread was interrupted");
    }

    if(milliseconds != 0)
    {
        ElapsedTimer timer(milliseconds);

        bool isLocked = false;
        do
        {
            isLocked = tryLockPrivate();

            if(!isLocked)
            {
                if(Thread::isInterrupted())
                {
                    throw InterruptedException("Thread was interrupted");
                }

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
            Thread *owner = ThreadManager::GetInstance().GetThread(this->mutexOwnerThreadId_);
            if(owner) ICRITICAL() << "Lock timeout, waiting for owner thread : " << owner;

            ICRITICAL() << "Timed out waiting for lock!";
        }
    }

    return tryLockPrivate();
}

bool Mutex::tryLockPrivate()
{
    // -- debug --
    Thread *self = debugMutexTrylockEntry();
    // -- debug --

#ifdef USE_GCC
    int status = pthread_mutex_trylock(&mutexHolder_->GetMutex());
    if(status == 0)
    {
        debugMutexLockExit(self);
        return true;
    }
    else if(status == EBUSY)
    {
        debugMutexTrylockFailedExit(self);
        return false;
    }
    else
    {
        std::cerr << "returned unknown error code: " << status;
        throw std::runtime_error("Mutex::tryLock(): ERROR! returned unknown error code: "); // + string(status));
    }
#else
    int isAcquired = TryEnterCriticalSection(&mutexHolder_->GetMutex());
    if(isAcquired != 0)
    {
        debugMutexLockExit(self);
        return true;
    }
    else
    {
        debugMutexTrylockFailedExit(self);
        return false;
    }
#endif

    return false;
}

Thread* Mutex::debugMutexLockEntry()
{
    ASSERT(mutexHolder_);

    // -- deadlock detection --
    Thread *self = NULL;
    if(policy_.IsDebug())
    {
        self = Thread::currentThread();
        if(self)
        {
            self->setThreadState(ThreadDetails::LockingState);
            if(mutexOwnerThreadId_ == self->threadId())
            {
                std::stringstream str;
                str << "Deadlock detected in thread id"
                    << self->threadId() << " name " << self->threadName()
                    << " owner is " << mutexOwnerThreadId_;

                throw DeadlockException(str.str());
            }
        }
    }
    // -- end --
    return self;
}

void Mutex::debugMutexLockExit(Thread *self)
{
    ASSERT(mutexHolder_);

    // -- deadlock detection --
    if(policy_.IsDebug() && self)
    {
        ASSERT(self);
        mutexOwnerThreadId_ = self->threadId();
        self->setThreadState(ThreadDetails::RunningMutexedState);
    }
    // -- end --

    // -- deadlock detection --
    /*if(debug_ && self)
    {
        if(mutexOwnerThreadId_ == self->threadId())
        {
            std::stringstream str;
            str << "Deadlock detected in thread id"
                << self->threadId() << " name " << self->threadName()
                << " owner is " << mutexOwnerThreadId_;

            throw DeadlockException(str.str());
        }

        self->setThreadState(ThreadDetails::RunningMutexedState);
        mutexOwnerThreadId_ = self->threadId();
    }*/
    // -- end --
}

Thread* Mutex::debugMutexUnlockEntry()
{
    ASSERT(mutexHolder_);

    // -- deadlock detection --
    Thread *self = NULL;
    if(policy_.IsDebug())
    {
        self = Thread::currentThread();
        if(self)
        {
            self->setThreadState(ThreadDetails::UnlockingState);
            if(mutexOwnerThreadId_ != self->threadId())
            {
                ICRITICAL() << "Unlocking mutex which is not owned! Current thread id "
                            << self->threadId() << " name " << self->threadName()
                            << " owner is " << mutexOwnerThreadId_;
            }
        }

        mutexOwnerThreadId_ = 0;
    }
    // -- end --

    return self;
}

void Mutex::debugMutexUnlockExit(Thread *self)
{
    ASSERT(mutexHolder_);

    // -- deadlock detection --
    if(policy_.IsDebug() && self)
    {
        self->setThreadState(ThreadDetails::RunningState);
    }
    // -- end --
}

Thread* Mutex::debugMutexTrylockEntry()
{
    // -- deadlock detection --
    Thread *self = NULL;
    if(policy_.IsDebug())
    {
        self = Thread::currentThread();
        if(self)
        {
            self->setThreadState(ThreadDetails::LockingState);
            if(mutexOwnerThreadId_ == self->threadId())
                IWARNING() << "Attempting to lock owned mutex! Thread id " << self->threadId() << " name " << self->threadName();
        }
    }
    // -- end --
    return self;
}

void Mutex::debugMutexTrylockFailedExit(Thread *self)
{
    // -- deadlock detection --
    if(policy_.IsDebug() && self)
    {
        self->setThreadState(ThreadDetails::RunningState);
    }
    // -- end --
}

// --------------------------------------------------------------
// Null operation mutex
// --------------------------------------------------------------

NullMutex::NullMutex(MutexPolicy)
{
}

NullMutex::~NullMutex()
{
}

void NullMutex::lock() const
{
//    if(Thread::isInterrupted())
//    {
//        throw InterruptedException("Thread was interrupted");
//    }
}

void NullMutex::unlock() const
{
//    if(Thread::isInterrupted())
//    {
//        throw InterruptedException("Thread was interrupted");
//    }
}

bool NullMutex::tryLock(int64) const
{
//    if(Thread::isInterrupted())
//    {
//        throw InterruptedException("Thread was interrupted");
//    }
    return true;
}

}

