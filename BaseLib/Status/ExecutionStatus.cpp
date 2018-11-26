#include "BaseLib/Status/ExecutionStatus.h"

namespace BaseLib { namespace Status {

// ------------------------------------------------------
// Operators
// ------------------------------------------------------

std::ostream& operator<<(std::ostream& ostr, const ExecutionStatusKind::Type& kind)
{
    switch(kind)
    {
#define CASE(t) case t: ostr << #t; break;
        CASE(ExecutionStatusKind::NO)
        CASE(ExecutionStatusKind::STARTING)
        CASE(ExecutionStatusKind::STARTED)
        CASE(ExecutionStatusKind::SUSPENDED)
        CASE(ExecutionStatusKind::RESUMED)
        CASE(ExecutionStatusKind::CANCELLED)
        CASE(ExecutionStatusKind::FAILURE)
        CASE(ExecutionStatusKind::STOPPED)
        CASE(ExecutionStatusKind::INTERRUPTED)
#undef CASE
        default:
            std::cerr << "Missing operator<< case for ExecutionStatusKind::Type" << std::endl;
            break;
    }
    return ostr;
}

// ------------------------------------------------------
// Constructor and destructor
// ------------------------------------------------------

ExecutionStatus::ExecutionStatus()
    : executionCount_()
    , executionTime_()
    , state_(ExecutionStatusKind::NO)
{ }

ExecutionStatus::~ExecutionStatus()
{ }

// ------------------------------------------------------
// Simple getters to access state
// ------------------------------------------------------

const ExecutionCount& ExecutionStatus::Count() const
{
    return executionCount_;
}

const ExecutionTime& ExecutionStatus::Time() const
{
    return executionTime_;
}

ExecutionStatusKind::Type ExecutionStatus::State() const
{
    return state_;
}

void ExecutionStatus::Reset()
{
    IINFO() << "Resetting execution status!";

    Locker locker(this);
    executionCount_ = ExecutionCount();
    executionTime_  = ExecutionTime();
    state_          = ExecutionStatusKind::NO;
}

// ------------------------------------------------------
// Using interfaces: Simple checkers
// ------------------------------------------------------

bool ExecutionStatus::IsExecuting() const
{
    Locker locker(this);
    return isExecuting();
}

bool ExecutionStatus::isExecuting() const
{
    return ExecutionStatusKind::STARTED == state_ || ExecutionStatusKind::STARTING == state_ || ExecutionStatusKind::INTERRUPTED == state_;
}

bool ExecutionStatus::IsStarting() const
{
    Locker locker(this);
    return ExecutionStatusKind::STARTING == state_;
}

bool ExecutionStatus::IsStarted() const
{
    Locker locker(this);
    return ExecutionStatusKind::STARTED == state_;
}

bool ExecutionStatus::IsSuccess() const
{
    Locker locker(this);
    return ExecutionStatusKind::STOPPED == state_;
}

bool ExecutionStatus::IsCancelled() const
{
    Locker locker(this);
    return ExecutionStatusKind::CANCELLED == state_;
}

bool ExecutionStatus::IsInterrupted() const
{
    Locker locker(this);
    return ExecutionStatusKind::INTERRUPTED == state_;
}

bool ExecutionStatus::IsEverStarted() const
{
    Locker locker(this);
    return executionCount_.TotalNumAttempts() > 0;
}

bool ExecutionStatus::IsSuspended() const
{
    Locker locker(this);
    return ExecutionStatusKind::SUSPENDED == state_;
}

bool ExecutionStatus::IsResumed() const
{
    Locker locker(this);
    return ExecutionStatusKind::RESUMED == state_;
}

bool ExecutionStatus::IsFailed() const
{
    Locker locker(this);
    return ExecutionStatusKind::FAILURE == state_;
}

bool ExecutionStatus::IsDone() const
{
    return IsSuccess() || IsFailed() || IsCancelled();
}

bool ExecutionStatus::IsLastExecutionFailure() const
{
    Locker locker(this);
    return executionTime_.LastFailedExecutionTime() > executionTime_.LastSuccessfulExecutionTime();
}

bool ExecutionStatus::IsLastExecutionSuccess() const
{
    Locker locker(this);
    return executionTime_.LastFailedExecutionTime() < executionTime_.LastSuccessfulExecutionTime();
}

bool ExecutionStatus::IsNeverExecuted() const
{
    Locker locker(this);
    return executionCount_.TotalNumAttempts() <= 0;
}

bool ExecutionStatus::IsFalseStart() const
{
    Locker locker(this);
    return state_ == ExecutionStatusKind::NO && executionTime_.TimeSinceLastFalseStart().InMillis() < 20;
}

// --------------------------------------
// Using interfaces
// --------------------------------------

bool ExecutionStatus::SetIffNotExecuting(ExecutionStatusKind::Type kind)
{
    Locker locker(this);

    if(!isExecuting())
    {
        state_ = kind;
        return true;
    }
    return false;
}

void ExecutionStatus::Starting()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::STARTING;
}

void ExecutionStatus::Start()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::STARTED;
    executionCount_.Start();
    executionTime_.Start();
}

void ExecutionStatus::Suspend()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::SUSPENDED;
    executionCount_.Suspend();
    executionTime_.Suspend();
}

void ExecutionStatus::Resume()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::RESUMED;
    executionCount_.Resume();
    executionTime_.Resume();
}

void ExecutionStatus::Success()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::STOPPED;
    executionCount_.Success();
    executionTime_.Success();
}

void ExecutionStatus::Failure()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::FAILURE;
    executionCount_.Failure();
    executionTime_.Failure();
}

bool ExecutionStatus::Cancel()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::CANCELLED;
    executionCount_.Failure();
    executionTime_.Failure();
    return true;
}

bool ExecutionStatus::Interrupt()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::INTERRUPTED;
    executionCount_.Interrupt();
    executionTime_.Interrupt();
    return true;
}

void ExecutionStatus::FalseStart()
{
    Locker locker(this);
    state_ = ExecutionStatusKind::NO;
    executionCount_.FalseStart();
    executionTime_.FalseStart();
}

}}

