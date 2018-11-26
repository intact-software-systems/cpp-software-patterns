#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandController.h"
#include"RxCommand/CommandControllerSubscription.h"

namespace Reactor
{

template <typename Return>
class CommandControllerAction
        : public Reactor::details::CommandController
        , public CommandObserver<Return>
        , public Templates::ExecuteMethod0<CommandControllerSubscription<Return>>
{
public:
    CommandControllerAction(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
        : Reactor::details::CommandController(policy, threadPool)
        , CommandObserver<Return>()
        , subscription_()
        , rxSubject_()
    { }
    virtual ~CommandControllerAction()
    {
        reset();
        this->rxSubject_.state().Unsubscribe();
    }

    CLASS_TRAITS(CommandControllerAction)

    // -----------------------------------------------------------
    // Add typed commands
    // -----------------------------------------------------------

    bool Add(typename CommandActions<Return>::Ptr command)
    {
        CommandBase::Ptr commandBase = std::dynamic_pointer_cast<CommandBase>(command);

        ASSERT(commandBase != nullptr);

        return CommandController::AddCommand(commandBase);
    }

    // -----------------------------------------------------------
    // Start controller's execution of commands
    // -----------------------------------------------------------

    virtual CommandControllerSubscription<Return> Execute()
    {
        Locker locker(this);

        if(this->status()->IsExecuting())
        {
            IWARNING() << "Controller already executing.";
            return subscription_;
        }

        return resetAndExecute();
    }

    BaseLib::RxObserverSubjectNew<Return>& RxSubject()
    {
        return rxSubject_;
    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        return subscription_.WaitFor(msecs);
    }

    // -----------------------------------------------------------
    // Interface CommandObserver <T>
    // -----------------------------------------------------------

    virtual bool OnComplete(Command<Return>* command)
    {
        IINFO() << "Completed command " << command->GetName();
        processOnComplete(command);
        return true;
    }

    virtual bool OnNext(Command<Return>* command, Return value)
    {
        IINFO() << "Command " << command->GetName() << " value: " << value;
        processOnNext(command, value);
        return true;
    }

    virtual bool OnError(Command<Return>* command, BaseLib::GeneralException exception)
    {
        IINFO() << "Command " << command->GetName() << " exception: " << exception.what();
        processOnError(command, exception);
        return true;
    }

protected:

    // -----------------------------------------------------------
    // Private implementaiton
    // -----------------------------------------------------------

    virtual bool executeCommand(CommandBase::Ptr command)
    {
        typename CommandActions<Return>::Ptr commandType = std::dynamic_pointer_cast<CommandActions<Return>>(command);

        ASSERT(commandType != nullptr);

        commandType->Execute(std::dynamic_pointer_cast<CommandObserver<Return>>(this->GetPtr()), this->data()->CommandPool());
        return true;
    }

    CommandControllerSubscription<Return> resetAndExecute()
    {
        reset();

        subscription_ = CommandControllerSubscription<Return>(this->GetPtr());

        this->data()->commands().status()->Start();
        this->status()->Start();

        this->data()->ControllerPool()->OnReady((Runnable*)this);

        return subscription_;
    }

    // -----------------------------------------------------------
    // private functions to process callbacks onError and onSuccess and trigger this.run()
    // -----------------------------------------------------------

    void processOnComplete(Command<Return> *command)
    {
        if(command->IsDone())
        {
            this->data()->CommandPool()->OnDispose(command);
        }

        if(isDoneForever())
        {
            this->data()->ControllerPool()->OnDispose((Runnable*)this);
        }
    }

    void processOnNext(Command<Return> *, Return values)
    {
        // -----------------------------------------------
        // Note: It is important that CommandResult is notified last because
        // other threads/observers may be using CommandResult to know when data
        // is available in cache, etc.
        // -----------------------------------------------
        rxSubject_.event().Next(values);
        subscription_.OnNext(values);
    }

    void processOnError(Command<Return> *command, GeneralException exception)
    {
        if(Utility::instanceof<FatalException, GeneralException>(&exception))
        {
            ICRITICAL()<< "Stopping command controller! Fatal exception thrown";

            finishedWithFatalError(exception);
        }
        else
        {
            ICRITICAL() << "Error encountered while executing command";

            // ----------------------------------------------------------
            // Q: Should this be called when not all commands are failed?
            // A: Probably not!
            // TODO: Conditional callbacks!
            // - Use Criterion policy in CommandController!
            // - If criterion is violated then call-back failure.
            // - Use it in IsSuccess() and when triggering commands in run().
            // ----------------------------------------------------------

            if(isDoneForever()) {
                this->data()->ControllerPool()->OnDispose((Runnable*)this);
            }

            this->data()->CommandPool()->OnAbort(command, exception);
        }
    }


    // -----------------------------------------------------------
    // private functions to set state, perform callbacks, etc
    // -----------------------------------------------------------

    virtual void finished()
    {
        this->data()->commands().status()->Success();
        this->status()->Success();

        this->data()->ControllerPool()->OnDispose((Runnable*)this);

        rxSubject_.event().Complete();
        subscription_.OnComplete();

        IINFO() << "Completed controller successful in " << this->status()->Time().TimeSinceLastExecutionTime();
    }

    virtual void finishedWithError(GeneralException exception)
    {
        ICRITICAL() << "CommandController finished with errors for commands ";

        this->data()->commands().status()->Failure();
        this->status()->Failure();

        this->data()->ControllerPool()->OnDispose((Runnable*)this);

        rxSubject_.event().Error(exception);
        subscription_.OnError(exception);
    }

    virtual void finishedWithFatalError(GeneralException exception)
    {
        for(CommandBase::Ptr command : this->data()->commands().GetCommands())
        {
            command->Shutdown();
            this->data()->CommandPool()->OnAbort(command.get(), exception);
        }

        finishedWithError(exception);
    }

private:
    CommandControllerSubscription<Return> subscription_;
    BaseLib::RxObserverSubjectNew<Return> rxSubject_;
};

// ---------------------------------------------------------------
// CommandControllerKeyValue
// ---------------------------------------------------------------

template <typename K, typename V>
class CommandControllerKeyValue
        : public CommandControllerAction<IMap<K,V>>
{
public:
    CommandControllerKeyValue(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
        : CommandControllerAction<IMap<K,V>>(policy, threadPool)
    { }
    virtual ~CommandControllerKeyValue()
    { }

    CLASS_TRAITS(CommandControllerKeyValue)
};

}
