#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/ThreadPool/TaskPolicy.h"

namespace BaseLib { namespace Concurrent
{

template <typename T>
class ScheduledFuture
        : public Templates::ScheduledFutureResult<T, TaskPolicy>
{
public:
    ScheduledFuture(typename Templates::ScheduledFutureResult<T, TaskPolicy>::Ptr future)
        : future_(future)
    {}

    virtual ~ScheduledFuture()
    {}

    CLASS_TRAITS(ScheduledFuture)

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

    virtual bool Interrupt()
    {
        return future_->Interrupt();
    }

    bool IsAttached() const
    {
        return future_->IsAttached();
    }

    bool Detach()
    {
        return future_->Detach();
    }

    virtual bool IsInterrupted() const
    {
        return future_->IsInterrupted();
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
        return future_->Result(msecs);
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
        return future_->Policy();
    }

private:
    virtual void run()
    {
        future_->run();
    }

private:
    typename Templates::ScheduledFutureResult<T, TaskPolicy>::Ptr future_;
};

template <typename T>
class ScheduledFutureTaskNoOp
        : public Templates::ScheduledFutureResult<T, TaskPolicy>
        , public StaticSingletonPtr<ScheduledFutureTaskNoOp<T>>
{
public:
    ScheduledFutureTaskNoOp()
    {}
    virtual ~ScheduledFutureTaskNoOp()
    {}

    CLASS_TRAITS(ScheduledFutureTaskNoOp)

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool WaitFor(int64 = LONG_MAX) const
    {
        return false;
    }

    virtual bool IsCancelled() const
    {
        return false;
    }

    virtual const Status::ExecutionStatus &StatusConst() const
    {
        static Status::ExecutionStatus status;
        return status;
    }

    virtual Status::ExecutionStatus &Status()
    {
        static Status::ExecutionStatus status;
        return status;
    }

    virtual TaskPolicy Policy() const
    {
        static TaskPolicy policy = TaskPolicy::Default();
        return policy;
    }

    virtual void run()
    { }

    virtual void Trigger()
    { }

    virtual void Resume()
    { }

    virtual void Suspend()
    { }

    bool IsAttached() const
    {
        return false;
    }

    bool Detach()
    {
        return false;
    }

    virtual bool Interrupt()
    {
        return false;
    }

    virtual bool IsInterrupted() const
    {
        return false;
    }

    virtual bool IsTimeout() const
    {
        return false;
    }

    virtual bool IsExecuting() const
    {
        return false;
    }

    virtual bool IsSuspended() const
    {
        return false;
    }

    virtual bool IsReady() const
    {
        return false;
    }

    virtual typename Templates::FutureResult<T>::ResultHolder Result(int64 = LONG_MAX) const
    {
        return typename Templates::FutureResult<T>::ResultHolder(Status::FutureStatus::Deferred(), T());
    }

    virtual Duration ReadyIn() const
    {
        return Duration::zero();
    }

    virtual Duration TimeoutIn() const
    {
        return Duration::zero();
    }

};

template <typename T>
class ScheduledFutureTask
        : public Templates::ScheduledFutureResult<T, TaskPolicy>
{
public:
    ScheduledFutureTask(typename Templates::ScheduledFutureResult<T, TaskPolicy>::Ptr future)
        : future_(future)
    {}

    ScheduledFutureTask()
        : future_(ScheduledFutureTaskNoOp<T>::InstancePtr())
    {}

    virtual ~ScheduledFutureTask()
    {}

    CLASS_TRAITS(ScheduledFutureTask)

    virtual void run()
    {
        future_->run();
    }

    virtual std::string GetName() const override
    {
        return future_->GetName();
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

    virtual bool Interrupt()
    {
        return future_->Interrupt();
    }

    bool IsAttached() const
    {
        return future_->IsAttached();
    }

    bool Detach()
    {
        return future_->Detach();
    }

    virtual bool IsInterrupted() const
    {
        return future_->IsInterrupted();
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
        future_->Resume();
    }

    virtual void Suspend()
    {
        future_->Suspend();
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
        return future_->Result(msecs);
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
        return future_->Policy();
    }

private:
    typename Templates::ScheduledFutureResult<T, TaskPolicy>::Ptr future_;
};

}}
