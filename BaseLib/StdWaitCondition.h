#pragma once

#include <limits.h>
#include <unordered_set>
#include <condition_variable>

#include "BaseLib/Mutex.h"
#include "BaseLib/Export.h"
#include "Duration.h"
#include "Timestamp.h"

namespace BaseLib {

class StdMutex;

class StdWaitCondition
{
public:
    StdWaitCondition();
    ~StdWaitCondition();

    bool wait(StdMutex* mutex, int64 milliseconds = LONG_MAX);

    bool wait(StdMutex* mutex, Duration duration);
    bool wait(StdMutex* mutex, Duration duration, std::function<bool ()> && function);

    bool waitUntil(StdMutex* mutex, Timestamp timestamp);
    bool waitUntil(StdMutex* mutex, Timestamp timestamp, std::function<bool ()> && function);

    void wakeAll();
    void wakeOne();

private:
    std::shared_ptr<std::condition_variable_any> condition_;
};

}


