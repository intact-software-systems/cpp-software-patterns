#include"RxConcurrent/Strategy/MonitorWorkers.h"

namespace BaseLib { namespace Strategy {

bool LoadBalanceWorkers::Perform(Concurrent::ThreadPool* threadPool)
{
    return optimizeWorkers(threadPool, threadPool->state().Tasks(), threadPool->state().Executors());
}

bool LoadBalanceWorkers::optimizeWorkers(Concurrent::ThreadPool* threadPool, Concurrent::ScheduledTasks::Ptr tasks, Concurrent::Workers::Ptr workers)
{
    int maxNumWorkersToStop = workers->NumWorkers() - int(tasks->Size());
    if(maxNumWorkersToStop > 0)
    {
        stopIdleWorkers(workers, maxNumWorkersToStop);

        Concurrent::WorkersState::ListWorkers doneWorkers = workers->RemoveDone();

        if(!doneWorkers.empty())
            IINFO() << "Removed " << doneWorkers.size() << " workers that are not running and expired lifetime.";
    }
    else if(workers->IsSpace())
    {
        int maxNumWorkersToStart = std::max<int>(0, int(tasks->Size()) - workers->NumWorkers());
        if(maxNumWorkersToStart > 0)
        {
            //int numWorkersStarted =
            threadPool->StartWorkers(maxNumWorkersToStart);

            //if(numWorkersStarted > 0)
            //    IINFO() << "Started " << numWorkersStarted << " workers";
        }
    }

    return true;
}

int LoadBalanceWorkers::stopIdleWorkers(Concurrent::Workers::Ptr workers, int maxNumWorkersToStop)
{
    int numWorkersIdled = 0;

    for(Concurrent::Worker::Ptr worker : workers->state().Workers())
    {
        if(numWorkersIdled >= maxNumWorkersToStop)
        {
            break;
        }

        if(worker->IsIdle())
        {
            IDEBUG() << "Stopping idle worker: " << worker.get();
            worker->Stop();
            numWorkersIdled++;
        }
    }

    return numWorkersIdled;
}

}}
