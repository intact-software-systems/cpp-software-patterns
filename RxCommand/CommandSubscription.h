#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandControllerBase.h"

namespace Reactor {

// --------------------------------------------------------
// TODO: Add CommandsHandle to be able to find command later in CommandController
// --------------------------------------------------------

template <typename Return, typename Collection = IList<Return>>
class CommandSubscription
    : public CommandSubscriptionBase
      , public Templates::ResultMethod1<std::pair<Status::FutureStatus, Collection>, int64>
      , public RxObserverNew<Return>
      , public Templates::NotifyableShared<CommandSubscription<Return>>
{
private:
    typedef MutexTypeLocker<CommandSubscription<Return, Collection>>   Locker;
    typedef Templates::WakeAllTypeLocker<CommandSubscription<Return, Collection>> WakeAllLocker;

public:
    CommandSubscription(ScheduledFutureTask<bool> task)
        : task_(task)
        , status_(new Status::ExecutionStatus())
        , values_(new Collection())
        , exception_("")
    {
        status_->Start();
    }

    virtual ~CommandSubscription()
    { }

    // -----------------------------------------------------------
    // CommandSubscriptionBase
    // -----------------------------------------------------------

    virtual bool IsSuccess() const
    {
        return task_.IsSuccess();
    }

    virtual bool Cancel()
    {
        return task_.Cancel();
    }

    virtual bool IsCancelled() const
    {
        return task_.IsCancelled();
    }

    virtual bool IsDone() const
    {
        return task_.IsDone();
    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        return task_.WaitFor(msecs);
    }

    virtual typename Templates::FutureResult<Collection>::ResultHolder Result(int64 msecs = LONG_MAX) const
    {
        Locker locker(this);

        this->template waitForDone<Status::ExecutionStatus>(status_.get(), msecs);
        return pull();
    }

    // -----------------------------------------------------------
    // Interface RxObserver<T>
    // -----------------------------------------------------------

    virtual bool OnComplete()
    {
        WakeAllLocker locker(this);
        status_->Success();
        return true;
    }

    virtual bool OnNext(Return value)
    {
        Locker locker(this);
        push(value);
        return true;
    }

    virtual bool OnError(GeneralException exception)
    {
        WakeAllLocker locker(this);
        status_->Failure();
        exception_ = exception;
        return true;
    }

    // -----------------------------------------------------------
    // Factory functions
    // -----------------------------------------------------------

    static CommandSubscription<Return> NoOp()
    {
        return CommandSubscription<Return>(ScheduledFutureTask<bool>(ScheduledFutureTaskNoOp<bool>::InstancePtr()));
    }

private:

    // -----------------------------------------------------------
    // private implementation
    // -----------------------------------------------------------

    void push(const Return& value)
    {
        MutexTypeLocker<Collection> lockAdd(values_.get());
        values_->put(value);
    }

    virtual typename Templates::FutureResult<Collection>::ResultHolder pull() const
    {
        MutexTypeLocker<Collection> lockAdd(values_.get());

        if(status_->IsSuccess())
        {
            return typename Templates::FutureResult<Collection>::ResultHolder(Status::FutureStatus::Ready(), values_.operator*());
        }
        else if(status_->IsFailed())
        {
            return typename Templates::FutureResult<Collection>::ResultHolder(Status::FutureStatus::Failed(exception_), values_.operator*());
        }

        return typename Templates::FutureResult<Collection>::ResultHolder(Status::FutureStatus::Deferred(), values_.operator*());
    }

private:
    ScheduledFutureTask<bool>    task_;
    Status::ExecutionStatus::Ptr status_;
    std::shared_ptr<Collection>  values_;
    GeneralException             exception_;
};

}
