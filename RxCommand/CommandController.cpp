#include"RxCommand/CommandController.h"

namespace Reactor { namespace details
{

// ---------------------------------------------------------------
// CommandControllerState
// ---------------------------------------------------------------

CommandControllerState::CommandControllerState(RxThreadPool::Ptr threadPool)
    : controllerPool_(threadPool)
    , commandPool_(threadPool)
    , triggered_(false)
{}

CommandControllerState::~CommandControllerState()
{
    commands_.Clear();
}

const CommandsGroup &CommandControllerState::commands() const
{
    return commands_;
}

CommandsGroup &CommandControllerState::commands()
{
    return commands_;
}

const CommandsGroup &CommandControllerState::fallback() const
{
    return fallback_;
}

CommandsGroup &CommandControllerState::fallback()
{
    return fallback_;
}

bool CommandControllerState::SetCommands(const CommandsGroup &commands)
{
    commands_ = commands;
    return true;
}

bool CommandControllerState::SetFallback(const CommandsGroup &commands)
{
    fallback_ = commands;
    return true;
}

RxThreadPool::Ptr CommandControllerState::ControllerPool() const
{
    return controllerPool_;
}

RxThreadPool::Ptr CommandControllerState::CommandPool() const
{
    return commandPool_;
}

bool CommandControllerState::SetCommandPool(RxThreadPool::Ptr commandPool)
{
    commandPool_ = commandPool;
    return true;
}

const Templates::BooleanTrigger &CommandControllerState::Trigger() const
{
    return triggered_;
}

Templates::BooleanTrigger &CommandControllerState::Trigger()
{
    return triggered_;
}

// ---------------------------------------------------------------
// CommandController
// ---------------------------------------------------------------

CommandController::CommandController(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
    : Templates::ContextObjectShared<CommandControllerPolicy, CommandControllerState, Status::ExecutionStatus>(
        new CommandControllerPolicy(policy),
        new CommandControllerState(threadPool),
        new Status::ExecutionStatus()
    )
{ }

CommandController::~CommandController()
{ }

CommandController::Ptr CommandController::GetPtr()
{
    return shared_from_this();
}

// -----------------------------------------------------------
// Interface Runnable
// -----------------------------------------------------------

/**
 * Execute attached commands until they are done.
 * A command is done when its Attempt policy is met.
 */
void CommandController::run()
{
    try
    {
        if(this->data()->ControllerPool() == nullptr)
        {
            throw CriticalException("RxThreadPool is null for CommandController");
        }

        IList<CommandBase::Ptr> scheduledCommands = scheduleFromRun();

        // TODO: event OnScheduled(scheduledCommands);
        if(scheduledCommands.empty())
        {
            IWARNING() << "Scheduling nothing!";
        }
    }
    catch(CriticalException exception)
    {
        Locker locker(this);

        ICRITICAL() << "Error while scheduling commands in CommandController";
        finishedWithFatalError(exception);
    }
    catch(GeneralException exception)
    {
        // TODO: There is a risk that an infinite loop occurs here ... implement a loop breaker. UPDATE: One loop breaker is the timeout policy.
        // AlarmChecker.isInInfiniteLoop(state().getExecutionStatus(), state().getCommands());

        ICRITICAL() << "Error while scheduling commands in CommandController";
        this->data()->ControllerPool()->OnSchedule((Runnable*)this, computeMaxWaitTimeMs());
    }
}

std::string CommandController::GetName() const
{
    return std::string("CommandControllerInTest");
}

// ---------------------------------------------------------------
// Interface CommandControllerBase
// ---------------------------------------------------------------

/**
 * Set thread pool for commands to avoid CommandController starvation, which
 * occurs when commands use all threads in thread-pool.
 */
bool CommandController::SetCommandThreadPool(RxThreadPool::Ptr commandThreadPool)
{
    Locker locker(this);

    if(this->status()->IsExecuting()) return false;

    return this->data()->SetCommandPool(commandThreadPool);
}

bool CommandController::SetCommands(const CommandsGroup &commands)
{
    Locker locker(this);

    if(this->status()->IsExecuting()) return false;

    return this->data()->SetCommands(commands);
}

bool CommandController::SetFallback(const CommandsGroup &commands)
{
    Locker locker(this);

    if(this->status()->IsExecuting()) return false;

    return this->data()->SetFallback(commands);
}

bool CommandController::AddCommand(CommandBase::Ptr command)
{
    Locker locker(this);

    if(this->status()->IsExecuting()) return false;

    this->data()->commands().Add(command);
    return true;
}

bool CommandController::AddFallback(CommandBase::Ptr command)
{
    Locker locker(this);

    if(this->status()->IsExecuting()) return false;

    this->data()->fallback().Add(command);
    return true;
}

CommandsGroup CommandController::GetCommands() const
{
    Locker locker(this);
    return this->data()->commands();
}

CommandsGroup CommandController::GetFallback() const
{
    Locker locker(this);
    return this->data()->fallback();
}

bool CommandController::Shutdown()
{
    Locker locker(this);
    return shutdown();
}

bool CommandController::Cancel()
{
    Locker locker(this);
    return shutdown();
}

void CommandController::Resume()
{
    Locker locker(this);
    this->status()->Resume();
}

void CommandController::Suspend()
{
    Locker locker(this);
    this->status()->Suspend();
}

bool CommandController::Interrupt()
{
    //Locker locker(this);
    //this->status()->Interrupt();
    // TODO: How to interrupt? Interrupt scheduled future in progress on this->run
    return false;
}

void CommandController::Trigger()
{
    Locker locker(this);

    if(this->status()->IsExecuting() || this->data()->Trigger().IsTriggered())
    {
        return;
    }

    this->data()->Trigger().UpdateTrigger(true);
    this->data()->ControllerPool()->OnReady((Runnable*)this);
}

bool CommandController::Reset()
{
    Locker locker(this);

    if(this->status()->IsExecuting())
    {
        return false;
    }
    else
    {
        reset();
        return true;
    }
}

bool CommandController::IsSuccess() const
{
    Locker locker(this);
    return isSuccess();
}

bool CommandController::IsDone() const
{
    Locker locker(this);
    return isDoneForever();
}

bool CommandController::IsExecuting() const
{
    return this->status()->IsExecuting();
}

bool CommandController::IsCancelled() const
{
    return this->status()->IsCancelled();
}

bool CommandController::IsTimeout() const
{
    return isTimeout();
}

bool CommandController::IsSuspended() const
{
    return this->status()->IsSuspended();
}

bool CommandController::IsInterrupted() const
{
    return this->status()->IsInterrupted();
}

bool CommandController::IsPolicyViolated() const
{
    return BaseLib::Strategy::IsPolicyViolated::Perform(
            this->status().delegate(),
            this->config()->GetInterval(),
            this->config()->Timeout(),
            this->config()->GetAttempt());
}

bool CommandController::AreCommandsExecuting() const
{
    return this->data()->commands().AreCommandsExecuting();
}

int CommandController::NumCommandsExecuting() const
{
    return this->data()->commands().NumTasksExecuting();
}

Duration CommandController::TimeoutIn() const
{
    return Strategy::ComputeTimeUntilTimeout::Perform(
                status().delegate(),
                config()->Timeout()
    );
}

Status::ExecutionStatus &CommandController::Status()
{
    Locker locker(this);
    return this->status().operator *();
}

const Status::ExecutionStatus &CommandController::StatusConst() const
{
    Locker locker(this);
    return this->status().delegate();
}

IList<CommandBase::Ptr> CommandController::Schedule()
{
    Locker locker(this);
    return schedule();
}

// ---------------------------------------------------------------
// private functions
// ---------------------------------------------------------------

/**
* Tries to acquire a scheduling mutex, gives up within a give threshold.
*/
IList<CommandBase::Ptr> CommandController::scheduleFromRun()
{
    ScopedTimer t(IINFO());

    TryMutexTypeLocker<Mutex> lock(&(this->data()->schedulingMutex()), ACQUIRE_LOCK_TIMEOUT_IN_MS);

    if(!lock.isLocked())
    {
        IWARNING() << "Could not acquire scheduling lock within " << ACQUIRE_LOCK_TIMEOUT_IN_MS << " ms";
        return CollectionUtils::EmptyIList<CommandBase::Ptr>();
    }

    return schedule();
}

IList<CommandBase::Ptr> CommandController::schedule()
{
    // TODO: Support fallback use
    if(isDoneForever())
    {
        stopScheduling();
        return CollectionUtils::EmptyIList<CommandBase::Ptr>();
    }
    else
    {
        if(isCommandGroupDone())
        {
            bool hasNext = nextGroupExecution();
            if(!hasNext)
            {
                IWARNING() << "Ignoring prepare next execution!";
                //ASSERT(hasNext);
            }

            return CollectionUtils::EmptyIList<CommandBase::Ptr>();
        }
//        else if(isFallback())
//        {
//            // set CommandsGroup fallback as the current execution group.
//        }
        else
        {
            IList<CommandBase::Ptr> scheduledCommands = scheduleReady();

            // TODO: Move to run? Then Schedule can be called externally to mean "Schedule once"
            scheduleController();

            return scheduledCommands;
        }
    }
}

bool CommandController::nextGroupExecution()
{
    // ------------------------------------------
    // Update commands execution status
    // ------------------------------------------
    if(!this->data()->Trigger().IsTriggered())
    {
        if(this->data()->commands().IsSuccess())
        {
            this->data()->commands().status()->Success();
        }
        else
        {
            this->data()->commands().status()->Failure();
        }
    }

    // ------------------------------------------
    // Is there a next execution?
    // ------------------------------------------
    if(this->data()->Trigger().IsTriggered() || BaseLib::Strategy::IsInAttempt::Perform(this->data()->commands().status().delegate(), this->config()->GetAttempt()))
    {
        IINFO() << "Current is done, starting fresh execution. Max: " << this->config()->GetAttempt().MaxNumAttempts() << " done: " << this->status()->Count().NumSuccesses();
        IINFO() << "Commands status " << this->data()->commands().status().delegate();

        this->data()->commands().Reset();
        this->data()->Trigger().UpdateTrigger(false);
        this->data()->commands().status()->Start();

        // ---------------------------------
        // schedule next using controller interval policy -- Use commands status?
        // ---------------------------------
        Duration waitTimeMs = BaseLib::Strategy::ComputeTimeUntilNextExecution::Perform(this->status().delegate(), this->config()->GetInterval());
        if(!waitTimeMs.IsInfinite())
        {
            IINFO() << "Time until next command group execution " << waitTimeMs;
            this->data()->ControllerPool()->OnSchedule((Runnable*)this, waitTimeMs.InMillis());
        }
        else
        {
            IINFO() << "Ignoring scheduling time " << waitTimeMs;
        }
        return true;
    }
    else
    {
        ASSERT(!data()->Trigger().IsTriggered() && !Strategy::IsInAttempt::Perform(
                this->data()->commands().status().delegate(),
                this->config()->GetAttempt()
        ));

        IWARNING() << "No more executions are possible";
        return false;
    }
}

/**
* Next schedule time. NB! waitTime == 0 should not be filtered out.
*/
void CommandController::scheduleController()
{
    Duration waitTimeMs = computeMaxWaitTimeMs();
    if(waitTimeMs < Duration::Infinite())
    {
        IINFO() << "Time until next command group execution " << waitTimeMs;
        this->data()->ControllerPool()->OnSchedule((Runnable*)this, waitTimeMs);
    }
    else
    {
        IINFO() << "Ignoring scheduling time " << waitTimeMs;
    }
}

void CommandController::stopScheduling()
{
    if(status()->IsExecuting())
    {
        if(isSuccess())
        {
            finished();
        }
        else
        {
            if(isTimeout())
            {
                doTimeout();
            }

            finishedWithError(Exception("CommandController finished with error!"));
        }
    }
    else
    {
        IWARNING() << "CommandController is already done";
    }
}

// -----------------------------------------------------------
// Private implementation
// -----------------------------------------------------------

void CommandController::reset()
{
    this->status().Set(new Status::ExecutionStatus());
    this->data()->commands().Reset();
}

IList<CommandBase::Ptr> CommandController::scheduleReady()
{
    // --------------------------------------
    // Algorithm: command.doTimeout() leads to callback to this.processOnError(command, ..) which cancels command through its future.
    // ---------------------------------------
    this->data()->commands().StopTimeoutCommands();

    int maxNumCommandsToExecute = std::max(0, this->config()->MaxConcurrentCommands().Limit() - this->data()->commands().NumTasksExecuting());

    IList<CommandBase::Ptr> readyCommands = this->config()->findReadyCommands()->Perform(maxNumCommandsToExecute, this->data()->commands());

    return executeCommands(readyCommands);
}

IList<CommandBase::Ptr> CommandController::executeCommands(const IList<CommandBase::Ptr> &commands)
{
    IList<CommandBase::Ptr> scheduledCommands;

    for(CommandBase::Ptr command : commands)
    {
        if(!command->IsExecuting() && command->IsReady())
        {
            bool isExecuted = executeCommand(command);

            ASSERT(isExecuted);

            scheduledCommands.push_back(command);
        }
    }

    return scheduledCommands;
}

// ---------------------------------------------------------------
// private functions
// ---------------------------------------------------------------

Duration CommandController::computeMaxWaitTimeMs() const
{
    return this->data()->commands().EarliestReadyIn();
}

bool CommandController::isTimeout() const
{
    return BaseLib::Strategy::IsTimeout::Perform(
                this->status().delegate(),
                this->config()->Timeout()
    );
}

// TODO: Rename to "is current command group execution done"
bool CommandController::isCommandGroupDone() const
{
    if(this->status()->IsCancelled())
    {
        IINFO() << "CommandController is cancelled";
        return true;
    }

    if(isTimeout())
    {
        IINFO() << "CommandController timed out";
        return true;
    }

    IINFO() << "Command group status: " << this->data()->commands().status().delegate();

    return this->data()->commands().IsDone();
}

bool CommandController::isDoneForever() const
{
    if(this->data()->Trigger().IsTriggered())
    {
        IINFO() << "CommandController is triggered";
        return false;
    }

    if(this->status()->IsCancelled())
    {
        IINFO() << "CommandController is cancelled";
        return true;
    }

    if(isTimeout())
    {
        IINFO() << "CommandController timed out";
        return true;
    }

    bool isDone = this->data()->commands().IsDone();

    bool isInAttempt = Strategy::IsInAttempt::Perform(
                this->data()->commands().status().delegate(),
                this->config()->GetAttempt()
                );

    IINFO() << "Command group status: " << this->data()->commands().status().delegate();

    if(isDone && !isInAttempt)
        IINFO() << "Controller Done! inAttempt?" << isInAttempt << " done?" << isDone;

    return isDone && !isInAttempt;
}

bool CommandController::isSuccess() const
{
    return this->data()->commands().IsSuccess();
}

void CommandController::doTimeout()
{
    IINFO() << "CommandController is being timed out";

    this->data()->commands().Timeout();
    this->status()->Cancel();
}

/**
 * Cancel all attached commands and set execution state.
 */
bool CommandController::shutdown()
{
    this->data()->commands().Shutdown();
    this->status()->Cancel();
    return true;
}

}}

