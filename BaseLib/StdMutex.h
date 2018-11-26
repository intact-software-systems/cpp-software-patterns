#pragma once

#include"CommonDefines.h"
#include"MutexPolicy.h"
#include"Duration.h"
#include"Timestamp.h"

#include"BaseLib/Export.h"

namespace BaseLib {

class DLL_STATE StdMutex
{
public:
    explicit StdMutex(MutexPolicy policy = MutexPolicy::No());

    StdMutex(const StdMutex& other);
    ~StdMutex();

    StdMutex& operator=(const StdMutex&);

    friend class StdWaitCondition;

    void lock();
    void unlock();

    bool tryLock(int64 milliseconds = 0);
    bool tryLock(Duration duration);

    bool tryLockUntil(Timestamp timestamp);

    bool isDebug() const
    {
        return false;
    }

private:
    std::shared_ptr<std::timed_mutex> mutex_;
};

}
