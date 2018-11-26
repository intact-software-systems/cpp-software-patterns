#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/ThreadPool/Workers.h"
#include"RxConcurrent/ThreadPool/ThreadPool.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Strategy {

struct DLL_STATE LoadBalanceWorkers
{
    static bool Perform(Concurrent::ThreadPool* threadPool);

    static bool optimizeWorkers(Concurrent::ThreadPool* threadPool, Concurrent::ScheduledTasks::Ptr tasks, Concurrent::Workers::Ptr workers);

    static int stopIdleWorkers(Concurrent::Workers::Ptr workers, int numWorkers);
};

}}
