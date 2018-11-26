#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/GuardCondition.h"
#include"RxConcurrent/WaitSet.h"

#include"RxConcurrent/ThreadPool/ScheduledTask.h"
#include"RxConcurrent/ThreadPool/ThreadPoolBase.h"

#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

// ----------------------------------------------------
// ScheduledTasksState
// ----------------------------------------------------

class DLL_STATE ScheduledTasksState
{
public:
    typedef std::list<ScheduledRunnableTask::Ptr>              ListTask;
    typedef std::list<Templates::ScheduledRunnableFuture::Ptr> ListTaskBase;

public:
    ScheduledTasksState();
    virtual ~ScheduledTasksState();

    ListTask      & Tasks();
    const ListTask& Tasks() const;

    ListTaskBase& TasksBase();

    void Add(ScheduledRunnableTask::Ptr future);
    void Remove(ScheduledRunnableTask::Ptr future);

private:
    /**
     * @brief taks_ is a collection of all ScheduledTasks instances which are triggered by task
     * scheduler using WaitSet/Condition
     */
    ListTask tasks_;

    /**
     * @brief tasksBase_ for convenience
     */
    ListTaskBase tasksBase_;
};

// ----------------------------------------------------
// ScheduledTasks
// ----------------------------------------------------

class DLL_STATE ScheduledTasks
    : public ScheduledTasksBase
      , public Templates::LockableType<ScheduledTasks, Mutex>
      , public Templates::ContextData<ScheduledTasksState>
      , public ENABLE_SHARED_FROM_THIS(ScheduledTasks)
{
public:
    ScheduledTasks();
    virtual ~ScheduledTasks();

    CLASS_TRAITS(ScheduledTasks)

    ScheduledTasks::Ptr GetPtr();

    virtual bool IsEmpty() const;

    virtual size_t Size() const;

    // ----------------------------------------------------
    // Task management
    // ----------------------------------------------------

    ScheduledRunnableTask::Ptr Add(const Templates::RunnableFuture::Ptr& task, const TaskPolicy& taskPolicy, ScheduleFunction scheduleFunction);
    ScheduledRunnableTask::Ptr Remove(const Templates::RunnableFuture::Ptr& task);
    ScheduledRunnableTask::Ptr Find(const Templates::RunnableFuture::Ptr& task);

    std::list<ScheduledRunnableTask::Ptr> RemoveDone();

    std::list<ScheduledRunnableTask::Ptr> InterruptTimeoutTasks();

    // ----------------------------------------------------
    // Action interface
    // ----------------------------------------------------

    virtual bool IsDone() const;
    virtual bool IsSuspended() const;
    virtual bool IsSuccess() const;
    virtual bool Cancel();

    // ----------------------------------------------------
    // Public functions
    // ----------------------------------------------------

    virtual void Clear(WaitSet::Ptr executorWaitSet);

    virtual int NumTasksExecuting() const;
    virtual int NumTasksPolicyViolated() const;
    virtual int NumTasksNeverStarted() const;

    virtual Duration EarliestReadyIn() const;

    //Status::ExecutionStatuses Snapshot() const;

    // ----------------------------------------------------
    // Trigger interfaces
    // ----------------------------------------------------

    virtual Duration FindReady(std::list<Templates::ScheduledRunnableFuture::Ptr>& readyTasks);
    virtual int      TriggerReady();

    virtual void Trigger();
    virtual void Resume();
    virtual void Suspend();

    // ----------------------------------------------------
    // Status printing
    // ----------------------------------------------------

    void Report(std::ostream& ostr) const;
};

}}
