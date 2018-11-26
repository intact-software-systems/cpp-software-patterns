#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandSubscription.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/CommandPolicy.h"

namespace Reactor {

// ---------------------------------------------------------------
// Class CommandState
// ---------------------------------------------------------------

template <typename Return>
class CommandState
{
    typedef IList<Templates::Action0<Return>*> ActionList;

public:
    CommandState(std::shared_ptr<ScheduledRunnableTask> task)
        : subscription_(CommandSubscription<Return>::NoOp())
        , task_(task)
    {
        ASSERT(task_);
    }

    virtual ~CommandState()
    { }

    void ResetSubscription()
    {
        subscription_ = CommandSubscription<Return>::NoOp();
    }

    void SetSubscription(const CommandSubscription<Return>& subscription)
    {
        subscription_ = subscription;
    }

    CommandSubscription<Return>& Subscription()
    {
        return subscription_;
    }

    const CommandSubscription<Return>& Subscription() const
    {
        return subscription_;
    }

    RxObserverSubject<Return>& RxSubject()
    {
        return subject_;
    }

    CommandSubject<Return>& commandSubject()
    {
        return commandSubject_;
    }

    ScheduledRunnableTask::Ptr Task() const
    {
        return task_->GetPtr();
    }

    ActionList& Actions()
    {
        return actions_;
    }

private:
    CommandSubscription<Return> subscription_;
    RxObserverSubject<Return>   subject_;
    CommandSubject<Return>      commandSubject_;

    ActionList actions_;

    ScheduledRunnableTask::Ptr task_;
};

// ---------------------------------------------------------------
// Class CommandActions
// ---------------------------------------------------------------

template <typename Return>
class CommandActions
    : public Command<Return>
      , public Templates::ContextObjectShared<CommandPolicy, CommandState<Return>>
      , public Templates::LockableType<CommandActions<Return>, Mutex>
{
    typedef MutexTypeLocker<CommandActions<Return>> Locker;

public:
    CommandActions(CommandPolicy policy = CommandPolicy::Default())
        : Command<Return>()
        , Templates::ContextObjectShared<CommandPolicy, CommandState<Return>>
            (
                new CommandPolicy(policy),
                new CommandState<Return>(ScheduledRunnableTask::Default(FutureFactory::Create(this)->GetPtr(), policy.GetTaskPolicy()))
            )
    { }

    virtual ~CommandActions()
    {
        this->data()->RxSubject().DisconnectAll();
        this->data()->commandSubject().state().Unsubscribe();

        for(Templates::Action* action : this->data()->Actions())
        {
            delete action;
        }

        this->data()->Actions().clear();
    }

    CLASS_TRAITS(CommandActions)

    // -----------------------------------------------------------
    // Interface Runnable
    // -----------------------------------------------------------

    virtual void run()
    {
        try
        {
            this->config()->template GetCompositionStrategy<Return>()->Perform(
                this->config()->SuccessCriterion(),
                this->data()->Actions(),
                this
            );
        }
        catch(...)
        {
            processOnError(Exception("Caught exception while executing command"));
        }
    }

    // -----------------------------------------------------------
    // Interface Command<T>
    // -----------------------------------------------------------

    virtual CommandPolicy Policy() const
    {
        return this->config().Clone();
    }

    CommandSubscription<Return> Result() const
    {
        Locker locker(this);
        return this->data()->Subscription();
    }

    void Add(typename Templates::Action0<Return>::Ptr action)
    {
        Locker locker(this);
        this->data()->Actions().push_back(new Templates::Action0Invoker<Return>(action));
    }

    template <typename Arg1>
    void Add(typename Templates::Action1<Return, Arg1>::Ptr action, Arg1 arg1)
    {
        Locker locker(this);
        this->data()->Actions().push_back(new Templates::Action1Invoker<Return, Arg1>(action, arg1));
    }

    template <typename Arg1, typename Arg2>
    void Add(typename Templates::Action2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        Locker locker(this);
        this->data()->Actions().push_back(new Templates::Action2Invoker<Return, Arg1, Arg2>(action, arg1, arg2));
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    void Add(typename Templates::Action3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        Locker locker(this);
        this->data()->Actions().push_back(new Templates::Action3Invoker<Return, Arg1, Arg2, Arg3>(action, arg1, arg2, arg3));
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    void Add(typename Templates::Action4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        Locker locker(this);
        this->data()->Actions().push_back(new Templates::Action4Invoker<Return, Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4));
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    void Add(typename Templates::Action5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        Locker locker(this);
        this->data()->Actions().push_back(new Templates::Action5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5));
    }

    CommandSubscription<Return> Execute(typename CommandObserver<Return>::Ptr observer, RxThreadPool::Ptr threadPool)
    {
        Locker locker(this);
        return execute(threadPool, observer);
    }

    CommandSubscription<Return> Execute(RxThreadPool::Ptr threadPool)
    {
        Locker locker(this);
        return execute(threadPool, typename CommandObserver<Return>::Ptr());
    }

    // -----------------------------------------------------------
    // Interface CommandBase
    // -----------------------------------------------------------

    virtual bool Cancel()
    {
        return this->data()->Task()->Cancel();
    }

    virtual bool IsDone() const
    {
        return this->data()->Task()->IsDone();
    }

    virtual bool IsSuccess() const
    {
        return this->data()->Task()->IsSuccess();
    }

    virtual bool WaitFor(int64 msecs) const
    {
        return this->data()->Task()->WaitFor(msecs);
    }

    virtual bool IsCancelled() const
    {
        return this->data()->Task()->IsCancelled();
    }

    virtual const Status::ExecutionStatus& StatusConst() const
    {
        return this->data()->Task()->StatusConst();
    }

    virtual Status::ExecutionStatus& Status()
    {
        return this->data()->Task()->Status();
    }

    virtual bool IsSuspended() const
    {
        return this->data()->Task()->IsSuspended();
    }

    virtual void Resume()
    {
        this->data()->Task()->Resume();
    }

    virtual void Suspend()
    {
        this->data()->Task()->Suspend();
    }

    virtual void Trigger()
    {
        this->data()->Task()->Trigger();
    }

    virtual bool Interrupt()
    {
        return this->data()->Task()->Interrupt();
    }

    virtual bool Detach()
    {
        return this->data()->Task()->Detach();
    }

    virtual bool IsAttached() const
    {
        return this->data()->Task()->IsAttached();
    }

    virtual bool IsInterrupted() const
    {
        return this->data()->Task()->IsInterrupted();
    }

    virtual bool IsTimeout() const
    {
        return this->data()->Task()->IsTimeout();
    }

    virtual bool IsExecuting() const
    {
        return this->data()->Task()->IsExecuting();
    }

    virtual bool IsReady() const
    {
        return this->data()->Task()->IsReady();
    }

    virtual Duration ReadyIn() const
    {
        return this->data()->Task()->ReadyIn();
    }

    virtual Duration TimeoutIn() const
    {
        return this->data()->Task()->TimeoutIn();
    }

    virtual bool IsPolicyViolated() const
    {
        return this->data()->Task()->IsPolicyViolated();
    }

    virtual bool Shutdown()
    {
        Locker locker(this);

        bool unsubscribe = this->data()->Task()->Cancel();

        this->data()->Subscription().Cancel();

        this->data()->ResetSubscription();
        this->data()->commandSubject().state().Unsubscribe();

        return unsubscribe;
    }

    virtual void Timeout()
    {
        Locker locker(this);

        processOnError(Exception("Command timed out according to command policy"));

        this->data()->Subscription().Cancel();
    }

    virtual bool Reset()
    {
        Locker locker(this);

        this->data()->ResetSubscription();
        this->data()->Task()->Status().Reset();
        return true;
    }

    // -----------------------------------------------------------
    // Interface RxObserverNew
    // -----------------------------------------------------------

    virtual void OnFilteredOut(ObserverEvent::Ptr)
    {
        IINFO() << " filtered out";
        //ObserverEventType::OnFilteredOut(ptr);
    }

    // -----------------------------------------------------------
    // Interface RxObserver<T>
    // -----------------------------------------------------------

    virtual bool OnComplete()
    {
        return processOnComplete();
    }

    virtual bool OnNext(Return)
    {
        // Note: Currently, this is only used from this command's CompositionStrategy.
        return true;
    }

    virtual bool OnError(GeneralException throwable)
    {
        return processOnError(throwable);
    }

    // -----------------------------------------------------------
    // Interface KeyValueRxObserver<K, V>
    // -----------------------------------------------------------

    virtual bool OnComplete(Templates::Action0<Return>*)
    {
        IINFO() << "Last action completed in command chain";
        return true;
    }

    virtual bool OnNext(Templates::Action0<Return>*, Return value)
    {
        return processOnNext(value);
    }

    virtual bool OnError(Templates::Action0<Return>*, GeneralException)
    {
        IWARNING() << "Error encountered while executing action";
        return true;
    }

protected:

    // -----------------------------------------------------------
    // Private functions
    // -----------------------------------------------------------

    CommandSubscription<Return> execute(RxThreadPool::Ptr threadPool, typename CommandObserver<Return>::Ptr observer)
    {
        if(this->data()->Task()->Status().IsExecuting())
        {
            IWARNING() << "Command is already executing. Returning existing CommandSubscription!";
            return this->data()->Subscription();
        }

        // ---------------------------------------
        // Update command state
        // ---------------------------------------
        this->data()->Task()->Status().Start();

        // ---------------------------------------
        // Set command observer, for example, CommandController
        // ---------------------------------------
        this->data()->commandSubject().state().Unsubscribe();
        if(observer != nullptr)
        {
            this->data()->commandSubject().Subscribe(observer, ObserverPolicy::Default());
        }

        bool createAsSuspended = true;

        ScheduledFutureTask<bool> future = threadPool->Schedule((Runnable*) this, TaskPolicy::RunOnceImmediately(), createAsSuspended);

        // ---------------------------------------
        // Set command subscription
        // ---------------------------------------
        CommandSubscription<Return> subscription(future);
        this->data()->SetSubscription(subscription);

        future.Resume();

        return subscription;
    }

    bool processOnComplete()
    {
        // ---------------------------------------------
        // Update command state
        // ---------------------------------------------
        this->data()->Task()->Status().Success();

        IINFO() << "Completed with status: " << this->data()->Task()->StatusConst();

        // ---------------------------------------------
        // Notify observers
        // ---------------------------------------------
        this->data()->commandSubject().event().Complete(this);
        this->data()->RxSubject().OnComplete();

        // ---------------------------------------------
        // Set directly on CommandSubscription
        // ---------------------------------------------
        this->data()->Subscription().OnComplete();

        return true;
    }

    bool processOnError(GeneralException exception)
    {
        // ---------------------------------------------
        // Update command state
        // ---------------------------------------------
        if(Utility::instanceof<CancelException, GeneralException>(&exception))
        {
            this->data()->Task()->Cancel();
        }
        else
        {
            this->data()->Task()->Status().Failure();
        }

        // ---------------------------------------------
        // Notify observers
        // ---------------------------------------------
        this->data()->commandSubject().event().Error(this, exception);
        this->data()->RxSubject().OnError(exception);

        // ---------------------------------------------
        // Set directly on CommandSubscription
        // ---------------------------------------------
        this->data()->Subscription().OnError(exception);

        return true;
    }

    bool processOnNext(Return value)
    {
        // ---------------------------------------------
        // Notify observers
        // ---------------------------------------------
        this->data()->commandSubject().event().Next(this, value);
        this->data()->RxSubject().OnNext(value);

        // ---------------------------------------------
        // Set directly on CommandSubscription
        // ---------------------------------------------
        this->data()->Subscription().OnNext(value);

        return true;
    }
};

}
