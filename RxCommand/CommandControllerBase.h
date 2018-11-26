#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Commands.h"
#include"RxCommand/Export.h"

namespace Reactor
{

class DLL_STATE CommandController
        : public Templates::RunnableFuture
        , public Templates::ShutdownMethod
        , public Templates::ResumeMethod
        , public Templates::SuspendMethod
        , public Templates::TriggerMethod
        , public Templates::ResetMethod<bool>
        , public Templates::TimeoutInMethod<Duration>
        , public Templates::IsTimeoutMethod
        , public Templates::IsExecutingMethod
        , public Templates::IsSuspendedMethod
        , public Templates::IsPolicyViolatedMethod
{
public:
    virtual ~CommandController() {}

    CLASS_TRAITS(CommandController)

    virtual bool SetCommandThreadPool(RxThreadPool::Ptr commandThreadPool) = 0;

    virtual bool SetCommands(const CommandsGroup &commands) = 0;
    virtual bool SetFallback(const CommandsGroup &commands) = 0;

    virtual bool AddCommand(CommandBase::Ptr command) = 0;
    virtual bool AddFallback(CommandBase::Ptr command) = 0;

    virtual CommandsGroup GetCommands() const = 0;
    virtual CommandsGroup GetFallback() const = 0;

    virtual bool AreCommandsExecuting() const = 0;
    virtual int NumCommandsExecuting() const = 0;
};

}
