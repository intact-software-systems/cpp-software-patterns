#pragma once

#include <limits.h>
#include <unordered_set>

#include "BaseLib/Mutex.h"
#include "BaseLib/Export.h"

namespace BaseLib {

class Mutex;

class ReadWriteLock;

class DLL_STATE InterruptibleWaitCondition
{
public:
    InterruptibleWaitCondition();
    virtual ~InterruptibleWaitCondition();

    bool wait(Mutex* mutex, int64 milliseconds = LONG_MAX);
    bool wait(ReadWriteLock* readWriteLock, int64 milliseconds = LONG_MAX);

    void wakeAll();
    void wakeOne();

    size_t numWaiters() const;

private:
    void insertWaiting();
    void removeWaiting();

private:
    std::unordered_set<int64> waiting_;
    mutable Mutex             mutex_;
};

}
