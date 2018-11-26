/***************************************************************************
                          Mutex.h  -  description
                             -------------------
    begin                : Sat Mar 12 2011
    copyright            : (C) 2011 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexPolicy.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

class Thread;

// --------------------------------------------------------------
// MutexVariableHolder
// --------------------------------------------------------------

class DLL_STATE MutexVariableHolder
        : public ENABLE_SHARED_FROM_THIS(MutexVariableHolder)
{
public:
    MutexVariableHolder();
    virtual ~MutexVariableHolder();

#ifdef USE_GCC
    pthread_mutex_t&    GetMutex() { return mutex_; }
#else
    CRITICAL_SECTION&   GetMutex() { return criticalSection_; }
#endif

private:

#ifdef USE_GCC
    pthread_mutex_t 	mutex_;
#else
    CRITICAL_SECTION	criticalSection_;
#endif

private:
    MutexVariableHolder(const MutexVariableHolder &) { }
    MutexVariableHolder& operator=(const MutexVariableHolder&) { return *this; }
};

// --------------------------------------------------------------
// Mutex
// --------------------------------------------------------------

class DLL_STATE Mutex
{
public:
    explicit Mutex(MutexPolicy policy = MutexPolicy::Debug());
    Mutex(const Mutex &other);
    ~Mutex();

    Mutex& operator=(const Mutex&);

    friend class WaitCondition;

    void lock();
    void unlock();
    bool tryLock(int64 milliseconds = 0);

    bool isDebug() const
    {
        return policy_.IsDebug();
    }

private:
    bool tryLockPrivate();

private:
    Thread* debugMutexLockEntry();
    void    debugMutexLockExit(Thread *self);

private:
    Thread* debugMutexUnlockEntry();
    void    debugMutexUnlockExit(Thread *self);

private:
    Thread* debugMutexTrylockEntry();
    void    debugMutexTrylockFailedExit(Thread *self);

private:
    const MutexPolicy policy_;

private:
    int64 mutexOwnerThreadId_;

private:
    std::shared_ptr<MutexVariableHolder> mutexHolder_;
};

/**
 * @brief The NullMutex class is useful when used as template argument
 * into an object with varying needs of synchronization.
 *
 * This can be used to create an unsynchronized object and maintain portability.
 */
class DLL_STATE NullMutex
{
public:
    explicit NullMutex(MutexPolicy = MutexPolicy::No());

    ~NullMutex();

    friend class WaitCondition;

    void lock() const;
    void unlock() const;
    bool tryLock(int64 milliseconds = 0) const;
};

}
