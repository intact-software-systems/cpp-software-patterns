#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexPolicy.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

class DLL_STATE MutexVariableHolderBare
        : public ENABLE_SHARED_FROM_THIS(MutexVariableHolderBare)
{
public:
    MutexVariableHolderBare();
    ~MutexVariableHolderBare();

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
    MutexVariableHolderBare(const MutexVariableHolderBare &) { }
    MutexVariableHolderBare& operator=(const MutexVariableHolderBare&) { return *this; }
};

class DLL_STATE MutexBare
{
public:
    explicit MutexBare(MutexPolicy policy = MutexPolicy::No());
    MutexBare(const MutexBare &other);
    ~MutexBare();

    MutexBare& operator=(const MutexBare&);

    friend class WaitCondition;

    void lock();
    void unlock();
    bool tryLock(int64 milliseconds = 0);

    bool isDebug() const;

private:

    bool tryLockPrivate();

private:
    MutexPolicy policy_;
    std::shared_ptr<MutexVariableHolderBare> mutexHolder_;
};

}
