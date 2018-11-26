#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Export.h"

namespace Reactor
{

// ---------------------------------------------------------------
// CommandSubscriptionBase
// ---------------------------------------------------------------

class DLL_STATE CommandSubscriptionBase
        : public Templates::FutureBase
{
public:
    virtual ~CommandSubscriptionBase() {}

    CLASS_TRAITS(CommandSubscriptionBase)
};

// ---------------------------------------------------------------
// CommandBase
// ---------------------------------------------------------------

class DLL_STATE CommandBase
        : public Templates::ScheduledRunnableFuture
        , public Templates::ResetMethod<bool>
        , public Templates::TimeoutMethod
        , public Templates::ShutdownMethod
        , public Templates::IsPolicyViolatedMethod
{
public:
    virtual ~CommandBase() {}

    CLASS_TRAITS(CommandBase)
};

// ---------------------------------------------------------------
// Command
// ---------------------------------------------------------------

template <typename Return>
class Command
        : public CommandBase
        , public RxObserverNew<Return>
        , public KeyValueRxObserver<Templates::Action0<Return>*, Return>
        , public ENABLE_SHARED_FROM_THIS_T1(Command, Return)
{
public:
    Command()
        : RxObserverNew<Return>(SubjectDescription("RxObserver.Command@InstanceAddress"))
        , KeyValueRxObserver<Templates::Action0<Return>*, Return>()
    {}
    virtual ~Command() {}

    CLASS_TRAITS(Command)
};

// ---------------------------------------------------------------
// CommandObserver
// ---------------------------------------------------------------

template <typename Return>
class CommandObserver
        : public KeyValueRxObserver<Command<Return>*, Return>
{
public:
    CLASS_TRAITS(CommandObserver)

public:
    CommandObserver(SubjectDescription description = SubjectDescription("CommanObserverRxEvents"))
        : KeyValueRxObserver<Command<Return>*, Return>(description)
    {}
    virtual ~CommandObserver()
    {}
};

// ---------------------------------------------------------------
// CommandSubject
// ---------------------------------------------------------------

template <typename Return>
class CommandSubject
        : public KeyValueRxEventsSubject<Command<Return>*, Return>
{
public:
    CommandSubject(SubjectDescription description = SubjectDescription("CommanObserverRxEvents"))
        : KeyValueRxEventsSubject<Command<Return>*, Return>(description)
    {}
    virtual ~CommandSubject()
    {}

    Subscriptions::Ptr Subscribe(typename CommandObserver<Return>::Ptr observer, ObserverPolicy policy = ObserverPolicy::Default())
    {
        return this->state().Subscribe(observer, policy);
    }
};

}
