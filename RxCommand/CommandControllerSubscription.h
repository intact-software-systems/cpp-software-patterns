#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandControllerBase.h"

namespace Reactor {

template <typename Return, typename Collection = IList<Return>>
class CommandControllerSubscription
    : public CommandSubscriptionBase
      , public Templates::ResultMethod1<std::pair<Status::FutureStatus, Collection>, int64>
      , public RxObserverNew<Return>
      , public Templates::NotifyableShared<CommandControllerSubscription<Return>>
{
protected:
    typedef MutexTypeLocker<CommandControllerSubscription<Return, Collection>>   Locker;
    typedef Templates::WakeAllTypeLocker<CommandControllerSubscription<Return, Collection>> WakeAllLocker;
    //typedef WaitForNotificationTypeLocker<CommandControllerSubscription<Return, Collection>>  WaitForNotificationLocker;

public:
    CommandControllerSubscription(CommandController::Ptr controller)
        : status_(new Status::ExecutionStatus())
        , controller_(controller)
        , values_(new Collection())
        , exception_("")
    {
        status_->Start();
    }

    CommandControllerSubscription()
        : status_(new Status::ExecutionStatus())
        , controller_()
        , values_(new Collection())
        , exception_("")
    { }

    virtual ~CommandControllerSubscription()
    { }

    // -----------------------------------------------------------
    // CommandSubscriptionBase
    // -----------------------------------------------------------

    virtual bool IsSuccess() const
    {
        return status_->IsSuccess();
    }

    virtual bool Cancel()
    {
        status_->Cancel();

        CommandController::Ptr controllerPtr = controller_.lock();
        if(controllerPtr == nullptr) return false;

        return controllerPtr->Cancel();
    }

    virtual bool IsCancelled() const
    {
        return status_->IsCancelled();
    }

    virtual bool IsDone() const
    {
        return status_->IsDone();
    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        {
            CommandController::Ptr controllerPtr = controller_.lock();
            if(controllerPtr == nullptr && !status_->IsDone())
            {
                return false;
            }
        }

        Locker locker(this);
        this->template waitForDone<Status::ExecutionStatus>(status_.get(), msecs);

        return status_->IsDone();
    }

    virtual typename Templates::FutureResult<Collection>::ResultHolder Result(int64 msecs = LONG_MAX) const
    {
        {
            CommandController::Ptr controllerPtr = controller_.lock();
            if(controllerPtr == nullptr && !status_->IsDone())
            {
                MutexTypeLocker<Collection> lockAdd(values_.get());
                return typename Templates::FutureResult<Collection>::ResultHolder(Status::FutureStatus::Failed(exception_), values_.operator*());
            }
        }

        Locker locker(this);

        this->template waitForDone<Status::ExecutionStatus>(status_.get(), msecs);

        IINFO() << "Woke up! Done? " << status_->IsDone();

        return pull();
    }

    // -----------------------------------------------------------
    // Interface RxObserver<T>
    // -----------------------------------------------------------

    virtual bool OnComplete()
    {
        IINFO() << "Completed! Wakeup call!";
        WakeAllLocker locker(this);
        status_->Success();
        return true;
    }

    virtual bool OnNext(Return value)
    {
        IINFO() << "Received value" << value;
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

    static CommandControllerSubscription<Return> NoOp()
    {
        return CommandControllerSubscription<Return>();
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
    Status::ExecutionStatus::Ptr status_;
    CommandController::WeakPtr   controller_;
    std::shared_ptr<Collection>  values_;
    GeneralException             exception_;
};

}
