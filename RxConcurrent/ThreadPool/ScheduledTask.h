#pragma once

#include"RxConcurrent/CommonDefines.h"

#include"RxConcurrent/GuardCondition.h"
#include"RxConcurrent/ThreadPool/TaskPolicy.h"
#include"RxConcurrent/ThreadPool/ThreadPoolBase.h"

#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

// ----------------------------------------------------
// ScheduledTaskState
// ----------------------------------------------------

class DLL_STATE ScheduledRunnableTaskState
{
public:
    ScheduledRunnableTaskState();
    ScheduledRunnableTaskState(Templates::RunnableFuture::Ptr task, const ScheduleFunction& scheduleFunction_);
    ~ScheduledRunnableTaskState();

    Templates::RunnableFuture::Ptr Task() const;
    GuardCondition::Ptr            Cond() const;
    GuardCondition::Ptr            SchedulerCond() const;

    const ScheduleFunction& scheduleFunction() const;

private:
    Templates::RunnableFuture::Ptr task_;
    GuardCondition::Ptr            condition_;
    GuardCondition::Ptr            schedulerCondition_;
    ScheduleFunction               scheduleFunction_;
};

// ----------------------------------------------------
// ScheduledRunnableTask
// ----------------------------------------------------

/**
 * @brief ScheduledRunnableTask is similar to a command/action. It decorates a Templates::ScheduledRunnableFuture.
 *
 * TODO: bool IsAttached() to thread pool function
 */
class DLL_STATE ScheduledRunnableTask
    : public Templates::ScheduledRunnablePolicyFuture<TaskPolicy>
      , public Templates::FinalizeMethod
      , public Templates::ContextObject<TaskPolicy, ScheduledRunnableTaskState>
      , public ENABLE_SHARED_FROM_THIS(ScheduledRunnableTask)
{
public:
    ScheduledRunnableTask(Templates::RunnableFuture::Ptr task, TaskPolicy policy, ScheduleFunction scheduleFunction);
    virtual ~ScheduledRunnableTask();

    CLASS_TRAITS(ScheduledRunnableTask)

    ScheduledRunnableTask::Ptr GetPtr();

    // ------------------------------------------------------
    // Runnable called by worker
    // ------------------------------------------------------

    virtual void run();

    // ------------------------------------------------------
    // Finalize, make ready for deletion
    // ------------------------------------------------------

    virtual bool Finalize();

    // ------------------------------------------------------
    // Access to future
    // ------------------------------------------------------

    Templates::RunnableFuture::Ptr Task() const;

    virtual void Trigger();
    virtual void Resume();
    virtual bool Interrupt();

    virtual void Suspend();
    virtual bool Cancel();
    virtual bool Detach();

    // ------------------------------------------------------
    // Check status and policy
    // ------------------------------------------------------

    virtual TaskPolicy Policy() const;

    virtual const Status::ExecutionStatus& StatusConst() const;
    virtual Status::ExecutionStatus      & Status();

    virtual bool WaitFor(int64 msecs = LONG_MAX) const;

    virtual Duration ReadyIn() const;
    virtual Duration TimeoutIn() const;

    // ------------------------------------------------------
    // Using interfaces: Checking state
    // ------------------------------------------------------

    virtual bool IsAttached() const;
    virtual bool IsReady() const;
    virtual bool IsSuccess() const;
    virtual bool IsCancelled() const;
    virtual bool IsExecuting() const;
    virtual bool IsSuspended() const;
    virtual bool IsInterrupted() const;
    virtual bool IsDone() const;
    virtual bool IsTimeout() const;
    virtual bool IsPolicyViolated() const;

    // ------------------------------------------------------
    // Printing of states
    // ------------------------------------------------------

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const ScheduledRunnableTask& task)
    {
        ostr << task.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<ScheduledRunnableTask> task)
    {
        ostr << task->toString();
        return ostr;
    }

    // ------------------------------------------------------
    // Factory function
    // ------------------------------------------------------

    static std::shared_ptr<ScheduledRunnableTask> Default(std::shared_ptr<Templates::RunnableFuture> task, TaskPolicy policy);
};

// ----------------------------------------------------
// ScheduledTask
// ----------------------------------------------------

/**
 * @brief The ScheduledTask class is almost identical to a command/action. It decorates a ScheduledFutureResult<T>.
 *
 * ScheduledTask<T> is returned from ThreadPool.
 */
template <typename T>
class ScheduledTask
    : public Templates::ScheduledFutureResult<T, TaskPolicy>
      , public ENABLE_SHARED_FROM_THIS_T1(ScheduledTask, T)
{
public:
    ScheduledTask(
        typename Templates::FutureResult<T>::Ptr task,
        ScheduledRunnableTask::Ptr future,
        TaskPolicy policy
    )
        : task_(task)
        , future_(future)
        , policy_(policy)
    { }

    virtual ~ScheduledTask()
    { }

    CLASS_TRAITS(ScheduledTask)

    typename ScheduledTask<T>::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual void run()
    {
        future_->run();
    }

    virtual std::string GetName() const override
    {
        return future_->GetName();
    }

    virtual bool Interrupt()
    {
        return future_->Interrupt();
    }

    virtual bool IsInterrupted() const
    {
        return future_->IsInterrupted();
    }

    virtual bool IsSuccess() const
    {
        return future_->IsSuccess();
    }

    virtual bool Cancel()
    {
        return future_->Cancel();
    }

    virtual bool IsCancelled() const
    {
        return future_->IsCancelled();
    }

    virtual bool IsDone() const
    {
        return future_->IsDone();
    }

    virtual void Trigger()
    {
        future_->Trigger();
    }

    virtual void Resume()
    {
        future_->Resume();;
    }

    virtual void Suspend()
    {
        future_->Suspend();
    }

    bool IsAttached() const
    {
        return future_->IsAttached();
    }

    bool Detach()
    {
        return future_->Detach();
    }

    virtual bool IsTimeout() const
    {
        return future_->IsTimeout();
    }

    virtual bool IsExecuting() const
    {
        return future_->IsExecuting();
    }

    virtual bool IsSuspended() const
    {
        return future_->IsSuspended();
    }

    virtual bool IsReady() const
    {
        return future_->IsReady();
    }

    virtual Duration ReadyIn() const
    {
        return future_->ReadyIn();
    }

    virtual Duration TimeoutIn() const
    {
        return future_->TimeoutIn();
    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        return future_->WaitFor(msecs);
    }

    virtual typename Templates::FutureResult<T>::ResultHolder Result(int64 msecs = LONG_MAX) const
    {
        return task_->Result(msecs);
    }

    virtual const Status::ExecutionStatus& StatusConst() const
    {
        return future_->StatusConst();
    }

    virtual Status::ExecutionStatus& Status()
    {
        return future_->Status();
    }

    virtual TaskPolicy Policy() const
    {
        return policy_;
    }

private:
    typename Templates::FutureResult<T>::Ptr task_;
    ScheduledRunnableTask::Ptr               future_;
    TaskPolicy                               policy_;
};

}}
