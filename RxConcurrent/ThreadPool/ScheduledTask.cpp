#include"RxConcurrent/ThreadPool/ScheduledTask.h"

namespace BaseLib { namespace Concurrent {

// ----------------------------------------------------
// ScheduledTaskState
// ----------------------------------------------------

ScheduledRunnableTaskState::ScheduledRunnableTaskState()
    : task_()
    , condition_()
    , schedulerCondition_()
{ }

ScheduledRunnableTaskState::ScheduledRunnableTaskState(Templates::RunnableFuture::Ptr task, const ScheduleFunction& scheduleFunction_)
    : task_(task)
    , condition_(new GuardCondition())
    , schedulerCondition_(new GuardCondition())
    , scheduleFunction_(scheduleFunction_)
{
    ASSERT(task_);
}

ScheduledRunnableTaskState::~ScheduledRunnableTaskState()
{ }

Templates::RunnableFuture::Ptr ScheduledRunnableTaskState::Task() const
{
    return task_;
}

GuardCondition::Ptr ScheduledRunnableTaskState::Cond() const
{
    return condition_;
}

GuardCondition::Ptr ScheduledRunnableTaskState::SchedulerCond() const
{
    return schedulerCondition_;
}

const ScheduleFunction& ScheduledRunnableTaskState::scheduleFunction() const
{
    return scheduleFunction_;
}

// ----------------------------------------------------
// ScheduledTask
// ----------------------------------------------------

ScheduledRunnableTask::ScheduledRunnableTask(Templates::RunnableFuture::Ptr task, TaskPolicy policy, ScheduleFunction scheduleFunction)
    : Templates::ContextObject<TaskPolicy, ScheduledRunnableTaskState>
    (
        policy,
        ScheduledRunnableTaskState(task, scheduleFunction)
    )
{ }

ScheduledRunnableTask::~ScheduledRunnableTask()
{ }

ScheduledRunnableTask::Ptr ScheduledRunnableTask::GetPtr()
{
    return this->shared_from_this();
}

// ------------------------------------------------------
// Runnable called by worker
// ------------------------------------------------------

void ScheduledRunnableTask::run()
{
    this->state().Task()->run();
}

// ------------------------------------------------------
// Finalize, make ready for deletion
// ------------------------------------------------------

bool ScheduledRunnableTask::Finalize()
{
    this->state().Cond()->ResetHandler();
    return true;
}

// ------------------------------------------------------
// Access to future
// ------------------------------------------------------

Templates::RunnableFuture::Ptr ScheduledRunnableTask::Task() const
{
    return this->state().Task();
}

void ScheduledRunnableTask::Trigger()
{
    bool isStarting = this->state().Task()->Status().SetIffNotExecuting(Status::ExecutionStatusKind::STARTING);
    if(isStarting)
    {
        this->state().Cond()->SetTriggerValue(true);
    }
}

bool ScheduledRunnableTask::Cancel()
{
    return this->state().Task()->Cancel();
}

void ScheduledRunnableTask::Resume()
{
    bool isResumed = this->state().Task()->Status().SetIffNotExecuting(Status::ExecutionStatusKind::RESUMED);
    if(isResumed)
    {
        this->state().SchedulerCond()->SetTriggerValue(true);
    }
}

void ScheduledRunnableTask::Suspend()
{
    this->state().Task()->Status().Suspend();
}

bool ScheduledRunnableTask::Interrupt()
{
    return this->state().Task()->Interrupt();
}

bool ScheduledRunnableTask::Detach()
{
    // TODO: Reset handler? Call-back to parent to detach?
    return false;
}

// ------------------------------------------------------
// Check status and policy
// ------------------------------------------------------

TaskPolicy ScheduledRunnableTask::Policy() const
{
    return this->config();
}

const Status::ExecutionStatus& ScheduledRunnableTask::StatusConst() const
{
    return this->state().Task()->StatusConst();
}

Status::ExecutionStatus& ScheduledRunnableTask::Status()
{
    return this->state().Task()->Status();
}

bool ScheduledRunnableTask::WaitFor(int64 msecs) const
{
    return this->state().Task()->WaitFor(msecs);
}

Duration ScheduledRunnableTask::ReadyIn() const
{
    return this->state().scheduleFunction().operator()(this->state().Task()->StatusConst(), this->config());
}

Duration ScheduledRunnableTask::TimeoutIn() const
{
    return BaseLib::Strategy::ComputeTimeUntilTimeout::Perform(
        this->state().Task()->StatusConst(),
        this->config().Timeout());
}

// ------------------------------------------------------
// Using interfaces: Checking state
// ------------------------------------------------------

bool ScheduledRunnableTask::IsAttached() const
{
    return this->state().Cond()->HasHandler();
}

// TODO: Support for execution in "parallel"/"overlapping" policy
bool ScheduledRunnableTask::IsReady() const
{
    //return this->state().Cond()->GetTriggerValue() ||
    Duration untilReady = this->state().scheduleFunction().operator()(this->state().Task()->StatusConst(), this->config());
    if(untilReady.count() <= 0) {
        return true;
    }

    return !this->state().Task()->StatusConst().IsExecuting() &&
           BaseLib::Strategy::IsReadyToExecute::Perform(
               this->state().Task()->StatusConst(),
               this->config().Attempt(),
               this->config().Interval(),
               this->config().RetryInterval()
           );
}

bool ScheduledRunnableTask::IsSuccess() const
{
    // TODO: Use state and config to check if success
    return this->state().Task()->IsSuccess();
}

bool ScheduledRunnableTask::IsCancelled() const
{
    return this->state().Task()->IsCancelled();
}

bool ScheduledRunnableTask::IsExecuting() const
{
    return this->state().Task()->StatusConst().IsExecuting();
}

bool ScheduledRunnableTask::IsSuspended() const
{
    return this->state().Task()->StatusConst().IsSuspended();
}

bool ScheduledRunnableTask::IsInterrupted() const
{
    return this->state().Task()->IsInterrupted();
}

bool ScheduledRunnableTask::IsDone() const
{
    if(this->state().Task()->StatusConst().IsExecuting())
    {
        return false;
    }

    // TODO: Check task lifetime
    return !BaseLib::Strategy::IsInAttempt::Perform(
        this->state().Task()->StatusConst(),
        this->config().Attempt());
}

bool ScheduledRunnableTask::IsTimeout() const
{
    return BaseLib::Strategy::IsTimeout::Perform(
        this->state().Task()->StatusConst(),
        this->config().Timeout());
}

bool ScheduledRunnableTask::IsPolicyViolated() const
{
    return BaseLib::Strategy::IsPolicyViolated::Perform(
        this->state().Task()->StatusConst(),
        this->config().Interval(),
        this->config().Timeout(),
        this->config().Attempt());
}

std::string ScheduledRunnableTask::toString() const
{
    std::stringstream ostr;

    ostr << GetName() << ": Status(" << StatusConst() << "), Policy(" << Policy() << ")";
    return ostr.str();
}

std::shared_ptr<ScheduledRunnableTask> ScheduledRunnableTask::Default(std::shared_ptr<Templates::RunnableFuture> task, TaskPolicy policy)
{
    return std::make_shared<ScheduledRunnableTask>(task, policy, Function::defaultScheduleFunction);
}

}}
