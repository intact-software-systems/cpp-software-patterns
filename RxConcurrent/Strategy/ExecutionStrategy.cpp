#include"RxConcurrent/Strategy/ExecutionStrategy.h"

namespace BaseLib { namespace Strategy {

bool WaitSetExecutionStrategy::Perform(Concurrent::WaitSet& waitSet, Status::ExecutionStatus& status)
{
    try
    {
        Concurrent::AbstractCondition::Ptr condition = waitSet.WaitGetNext();

        if(!condition)
        {
            IINFO() << "Null condition returned. Might be shutting down";
            return false;
        }

        status.Start();

        bool isDispatched = Concurrent::WaitSet::DispatchTriggeredCondition(condition); // triggers call-back, for example, ThreadPool::operator()(Task)

        if(isDispatched)
        {
            status.Success();
        }
        else
        {
            status.FalseStart();
            IDEBUG() << "No task to trigger. Ignoring.";
        }

        return isDispatched;
    }
    catch(BaseLib::GeneralException e)
    {
        IWARNING() << "Caught exception " << e;
        status.Failure();
    }
    catch(...)
    {
        ICRITICAL() << "Caught undefined exception!";
        status.Failure();
    }

    return false;
}


int TaskTriggerAlgorithm::triggerTasks(std::list<Templates::ScheduledRunnableFuture::Ptr>& scheduledTasks)
{
    int numTasksTriggered = 0;

    for(Templates::ScheduledRunnableFuture::Ptr& future : scheduledTasks)
    {
        bool triggered = TaskTriggerAlgorithm::triggerTask(future.get());
        if(triggered) ++numTasksTriggered;
    }

    return numTasksTriggered;
}

// TODO: Support for execution in "parallel"/"overlapping" policy
bool TaskTriggerAlgorithm::triggerTask(Templates::ScheduledRunnableFuture* future)
{
    if(future->IsExecuting())
    {
        ICRITICAL() << "Future still executing: " << future->StatusConst();
        return false;
    }

    if(!future->IsReady())
    {
        ICRITICAL() << "Future triggered before ready: " << future->StatusConst();
        return false;
    }

    if(future->StatusConst().IsStarting())
    {
        IWARNING() << "Future is already being started: " << future->StatusConst();
        return false;
    }

    if(future->IsSuspended())
    {
        IWARNING() << "Future is suspended " << future->StatusConst();
        return false;
    }

    future->Trigger();
    return true;
}

std::list<Concurrent::ScheduledRunnableTask::Ptr> TriggerTaskByInstance::Perform(const std::list<Templates::RunnableFuture::Ptr>& tasks, Concurrent::ScheduledTasks& scheduledTasks)
{
    std::list<Concurrent::ScheduledRunnableTask::Ptr> triggered;

    if(scheduledTasks.IsEmpty()) return triggered;

    for(Templates::RunnableFuture::Ptr task : tasks)
    {
        Concurrent::ScheduledRunnableTask::Ptr future = scheduledTasks.Find(task);

        if(!future)
        {
            IWARNING() << "Task not found from task type. Ignoring!";
            continue;
        }

        bool isTriggered = TaskTriggerAlgorithm::triggerTask(future.get());

        if(isTriggered)
        {
            triggered.push_back(future->GetPtr());
        }
    }

    return triggered;
}

int DefaultTaskTrigger::Perform(Concurrent::ScheduledTasksBase* tasks)
{
    std::list<Templates::ScheduledRunnableFuture::Ptr> scheduledTasks;

    //int nextScheduleTime =
    tasks->FindReady(scheduledTasks);

    //IINFO() << "Find ready found next schedule time " << nextScheduleTime << " earliest ready at found " << tasks->EarliestReadyAtMs();

    // ----------------------------------------
    // Measure time it takes to trigger tasks
    // and substract from nextScheduleTime.
    // ----------------------------------------

    int numTasksTriggered = TaskTriggerAlgorithm::triggerTasks(scheduledTasks);

    // -- debug --
    //if(numTasksTriggered == 0) IWARNING() << "Schedule miss! No tasks triggered!";
    // -- debug --

    return numTasksTriggered;
}

Duration DefaultFindReady::Perform(std::list<Templates::ScheduledRunnableFuture::Ptr>& allTasks, std::list<Templates::ScheduledRunnableFuture::Ptr>& scheduledTasks)
{
    Duration nextScheduleTime = Duration::Infinite();

    for(const auto& task : allTasks)
    {
        Duration scheduleTime = task->ReadyIn();

        ASSERT(scheduleTime >= Duration::zero());

        // ---------------------------------------
        // If due to be scheduled then add to list
        // ---------------------------------------
        if(scheduleTime <= Duration::zero())
        {
            ASSERT(!task->IsSuspended());
            ASSERT(!task->IsInterrupted());
            ASSERT(!task->IsExecuting());
            ASSERT(task->IsReady());

            scheduledTasks.push_back(task);
        }
        else if(scheduleTime < nextScheduleTime)
        {
            // To find the minimum next schedule time
            nextScheduleTime = scheduleTime;
        }
    }

    return nextScheduleTime;
}


}}
