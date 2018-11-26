#include"RxConcurrent/ThreadPool/ScheduledTasks.h"
#include"RxConcurrent/Strategy/ExecutionStrategy.h"

namespace BaseLib { namespace Concurrent {

// ----------------------------------------------------
// ScheduledTasksState
// ----------------------------------------------------

ScheduledTasksState::ScheduledTasksState()
{ }

ScheduledTasksState::~ScheduledTasksState()
{ }

ScheduledTasksState::ListTask& ScheduledTasksState::Tasks()
{
    return tasks_;
}

const ScheduledTasksState::ListTask& ScheduledTasksState::Tasks() const
{
    return tasks_;
}

ScheduledTasksState::ListTaskBase& ScheduledTasksState::TasksBase()
{
    return tasksBase_;
}

void ScheduledTasksState::Add(ScheduledRunnableTask::Ptr future)
{
    tasks_.push_back(future->GetPtr());
    tasksBase_.push_back(future->GetPtr());
}

void ScheduledTasksState::Remove(ScheduledRunnableTask::Ptr future)
{
    tasks_.remove(future);

    size_t prev = tasksBase_.size();
    tasksBase_.remove(future);

    ASSERT(tasksBase_.size() == (prev - 1));
}

// ----------------------------------------------------
// ScheduledTasks
// ----------------------------------------------------

ScheduledTasks::ScheduledTasks()
    : Templates::LockableType<ScheduledTasks, Mutex>()
    , Templates::ContextData<ScheduledTasksState>()
{ }

ScheduledTasks::~ScheduledTasks()
{ }

ScheduledTasks::Ptr ScheduledTasks::GetPtr()
{
    return shared_from_this();
}

bool ScheduledTasks::IsEmpty() const
{
    Locker lock(this);

    return this->context().Tasks().empty();
}

size_t ScheduledTasks::Size() const
{
    Locker lock(this);

    return this->context().Tasks().size();
}

// ----------------------------------------------------
// Task management
// ----------------------------------------------------

ScheduledRunnableTask::Ptr ScheduledTasks::Add(const Templates::RunnableFuture::Ptr& task, const TaskPolicy& taskPolicy, ScheduleFunction scheduleFunction)
{
    Locker lock(this);

    ScheduledRunnableTask::Ptr future(new ScheduledRunnableTask(task, taskPolicy, scheduleFunction));

    this->context().Add(future->GetPtr());

    return future->GetPtr();
}

ScheduledRunnableTask::Ptr ScheduledTasks::Remove(const Templates::RunnableFuture::Ptr& task)
{
    Locker lock(this);

    if(!task) return ScheduledRunnableTask::Ptr();

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(future->Task().get() == task.get())
        {
            this->context().Remove(future);
            return future->GetPtr();
        }
    }

    return ScheduledRunnableTask::Ptr();
}

ScheduledRunnableTask::Ptr ScheduledTasks::Find(const Templates::RunnableFuture::Ptr& task)
{
    Locker lock(this);

    ASSERT(task);
    if(!task) return ScheduledRunnableTask::Ptr();

    for(const ScheduledRunnableTask::Ptr& future : this->context().Tasks())
    {
        if(future->Task().get() == task.get())
        {
            return future;
        }
    }

    return ScheduledRunnableTask::Ptr();
}

std::list<ScheduledRunnableTask::Ptr> ScheduledTasks::RemoveDone()
{
    Locker lock(this);

    std::list<ScheduledRunnableTask::Ptr> doneTasks;

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        ASSERT(future != nullptr);

        if(future->IsDone())
        {
            doneTasks.push_back(future->GetPtr());
        }
    }

    for(ScheduledRunnableTask::Ptr future : doneTasks)
    {
        future->Detach();
        this->context().Remove(future);
    }

    return doneTasks;
}


std::list<ScheduledRunnableTask::Ptr> ScheduledTasks::InterruptTimeoutTasks()
{
    std::list<ScheduledRunnableTask::Ptr> timedOut;

    for(auto future : this->context().Tasks())
    {
        if(future->IsTimeout())
        {
            IINFO() << "Timeout: " << future;

            future->Interrupt();

            timedOut.push_back(future);
        }
    }

    return timedOut;
}


bool ScheduledTasks::IsDone() const
{
    Locker lock(this);

    for(const ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(!future->IsDone())
        {
            return false;
        }
    }
    return true;
}

bool ScheduledTasks::IsSuspended() const
{
    Locker lock(this);

    for(const ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(!future->IsSuspended())
        {
            return false;
        }
    }
    return true;
}

bool ScheduledTasks::IsSuccess() const
{
    Locker lock(this);

    for(const ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(!future->IsSuccess())
        {
            return false;
        }
    }
    return true;
}

bool ScheduledTasks::Cancel()
{
    Locker lock(this);

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        future->Cancel();
    }
    return true;
}

void ScheduledTasks::Clear(WaitSet::Ptr executorWaitSet)
{
    Locker lock(this);

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        bool isDetached = executorWaitSet->Detach(future->state().Cond());

        ASSERT(isDetached);

        future->Finalize();

        //IDEBUG() << "Task use count Condition is " << future->state().Cond().use_count();
    }

    this->context().Tasks().clear();
}

int ScheduledTasks::NumTasksExecuting() const
{
    Locker lock(this);

    int numExecuting = 0;

    for(const ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(future->IsExecuting())
        {
            ++numExecuting;
        }
    }

    return numExecuting;
}

int ScheduledTasks::NumTasksPolicyViolated() const
{
    Locker lock(this);

    int policyViolationCounter = 0;

    for(const ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(future->IsPolicyViolated())
        {
            ++policyViolationCounter;
        }
    }

    return policyViolationCounter;
}

int ScheduledTasks::NumTasksNeverStarted() const
{
    Locker lock(this);

    int neverExecutedCounter = 0;

    for(const ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        if(future->StatusConst().IsNeverExecuted())
        {
            ++neverExecutedCounter;
        }
    }

    return neverExecutedCounter;
}

Duration ScheduledTasks::EarliestReadyIn() const
{
    Locker lock(this);

    Duration maxWaitTimeMs = Duration::Infinite();

    for(const ScheduledRunnableTask::Ptr& future : this->context().Tasks())
    {
        maxWaitTimeMs = std::min<Duration>(maxWaitTimeMs, future->ReadyIn());
    }

    return maxWaitTimeMs;
}

Duration ScheduledTasks::FindReady(std::list<Templates::ScheduledRunnableFuture::Ptr>& readyTasks)
{
    Locker lock(this);

    return Strategy::DefaultFindReady::Perform(this->context().TasksBase(), readyTasks);
}

int ScheduledTasks::TriggerReady()
{
    return Strategy::DefaultTaskTrigger::Perform(this);
}

void ScheduledTasks::Trigger()
{
    Locker lock(this);

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        future->Trigger();
    }
}

void ScheduledTasks::Resume()
{
    Locker lock(this);

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        future->Resume();
    }
}

void ScheduledTasks::Suspend()
{
    Locker lock(this);

    for(ScheduledRunnableTask::Ptr future : this->context().Tasks())
    {
        future->Suspend();
    }
}

// ----------------------------------------------------
// Status
// ----------------------------------------------------

void ScheduledTasks::Report(std::ostream& ostr) const
{
    for(const ScheduledRunnableTask::Ptr& future : this->context().Tasks())
    {
        ostr << "Future: " << future->StatusConst() << std::endl;
    }
}

}}
