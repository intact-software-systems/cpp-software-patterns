#pragma once

#include"RxCommand/IncludeExtLibs.h"

#include"RxCommand/CommandSubscription.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/CommandControllerBase.h"

#include"RxCommand/Commands.h"
#include"RxCommand/Command.h"

#include"RxCommand/Strategy/ComputationStrategy.h"

#include"RxCommand/CommandPolicy.h"
#include"RxCommand/CommandControllerPolicy.h"

#include"RxCommand/Export.h"

namespace Reactor { namespace details
{

// ---------------------------------------------------------------
// CommandControllerState
// ---------------------------------------------------------------

class DLL_STATE CommandControllerState
{
public:
    CommandControllerState() {}
    CommandControllerState(RxThreadPool::Ptr threadPool);
    virtual ~CommandControllerState();

    const CommandsGroup& commands() const;
    CommandsGroup&       commands();

    const CommandsGroup& fallback() const;
    CommandsGroup&       fallback();

    bool SetCommands(const CommandsGroup &commands);
    bool SetFallback(const CommandsGroup &commands);

    RxThreadPool::Ptr ControllerPool() const;
    RxThreadPool::Ptr CommandPool() const;

    bool SetCommandPool(RxThreadPool::Ptr commandPool);

    const Templates::BooleanTrigger&   Trigger() const;
    Templates::BooleanTrigger&         Trigger();

    Mutex &schedulingMutex() {
        return schedulingMutex_;
    }

private:
    RxThreadPool::Ptr controllerPool_;
    RxThreadPool::Ptr commandPool_;

    CommandsGroup commands_;
    CommandsGroup fallback_;

    Templates::BooleanTrigger triggered_;

    Mutex schedulingMutex_;
};

// ---------------------------------------------------------------
// CommandController
// ---------------------------------------------------------------

class DLL_STATE CommandController
        : public Reactor::CommandController
        , public Templates::LockableType<CommandController>
        , protected Templates::ContextObjectShared<CommandControllerPolicy, CommandControllerState, Status::ExecutionStatus>
        , public ENABLE_SHARED_FROM_THIS(CommandController)
{
private:
    static const int64 ACQUIRE_LOCK_TIMEOUT_IN_MS = 5000;

public:
    CommandController(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool);
    virtual ~CommandController();

    CLASS_TRAITS(CommandController)

    CommandController::Ptr GetPtr();

    // ---------------------------------------------------------------
    // Interface Runnable
    // ---------------------------------------------------------------

    virtual void run();

    virtual std::string GetName() const;

    // ---------------------------------------------------------------
    // Interface CommandControllerBase
    // ---------------------------------------------------------------

    virtual bool SetCommandThreadPool(RxThreadPool::Ptr commandThreadPool);

    virtual bool SetCommands(const CommandsGroup &commands);
    virtual bool SetFallback(const CommandsGroup &commands);

    virtual bool AddCommand(CommandBase::Ptr command);
    virtual bool AddFallback(CommandBase::Ptr command);

    virtual CommandsGroup GetCommands() const;
    virtual CommandsGroup GetFallback() const;

    virtual bool Shutdown();
    virtual bool Cancel();
    virtual void Resume();
    virtual void Suspend();
    virtual bool Interrupt();
    virtual void Trigger();
    virtual bool Reset();

    virtual bool IsDone() const;
    virtual bool IsSuccess() const;
    virtual bool IsExecuting() const;
    virtual bool IsCancelled() const;
    virtual bool IsTimeout() const;
    virtual bool IsInterrupted() const;
    virtual bool IsSuspended() const;
    virtual bool IsPolicyViolated() const;

    virtual bool AreCommandsExecuting() const;
    virtual int NumCommandsExecuting() const;
    virtual Duration TimeoutIn() const;

    virtual Status::ExecutionStatus& Status();
    virtual const Status::ExecutionStatus& StatusConst() const;

    virtual IList<CommandBase::Ptr> Schedule();

protected:

    // -----------------------------------------------------------
    // Private implementation
    // -----------------------------------------------------------

    void reset();
    void stopScheduling();
    void scheduleController();
    bool nextGroupExecution();

    IList<CommandBase::Ptr> scheduleFromRun();
    IList<CommandBase::Ptr> schedule();

    /**
     * Execute all commands that can be executed according to policies
     */
    IList<CommandBase::Ptr> scheduleReady();

    /**
     * Execute the commands if they are not currently executing
     * Note! "Not executing" should be enforced by computation strategies.
     */
    IList<CommandBase::Ptr> executeCommands(const IList<CommandBase::Ptr> &commands);

    /**
     * Execute templated command
     */
    virtual bool executeCommand(CommandBase::Ptr command) = 0;

    /**
     * Compute max wait time based on timeout and interval policy: "The time when a command should be done with the current execution and is ready for another execution".
     */
    Duration computeMaxWaitTimeMs() const;

    /**
     * Check if controller has timed out according to policy, i.e., no more time left to finish all commands.
     */
    bool isTimeout() const;

    /**
     * Check if commands group is done.
     */
    bool isCommandGroupDone() const;

    /**
     * Check if controller is cancelled, timed out, or if there are any more commands left executing.
     */
    bool isDoneForever() const;

    /**
     * Check if commands are successfully executed.
     */
    bool isSuccess() const;

    /**
     * Controller times out, stop all commands.
     */
    void doTimeout();

    /**
     * Cancel all attached commands and set execution state.
     */
    bool shutdown();

    // -----------------------------------------------------------
    // private functions to set state, perform callbacks, etc
    // -----------------------------------------------------------

    virtual void finished() = 0;
    virtual void finishedWithError(GeneralException exception) = 0;
    virtual void finishedWithFatalError(GeneralException exception) = 0;
};

}}
