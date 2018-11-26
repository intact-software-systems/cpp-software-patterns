#include "BaseLib/Status/ExecutionCount.h"

namespace BaseLib { namespace Status {

ExecutionCount::ExecutionCount()
    : executionCount_(0)
    , totalExecutionCount_(0)
    , numConsecutiveSuccesses_(0)
    , numConsecutiveFailures_(0)
{ }

ExecutionCount::~ExecutionCount()
{ }

Count ExecutionCount::NumFailures() const
{
    Locker locker(this);

    Count totalCompletedExecutions = totalExecutionCount_ - numFalseStarts_;

    ASSERT(totalCompletedExecutions >= 0);
    ASSERT(totalCompletedExecutions - executionCount_ >= 0);

    return totalCompletedExecutions - executionCount_;
}

Count ExecutionCount::NumSuccesses() const
{
    Locker locker(this);
    return executionCount_;
}

Count ExecutionCount::TotalNumAttempts() const
{
    Locker locker(this);

    ASSERT(totalExecutionCount_ - numFalseStarts_ >= 0);
    return totalExecutionCount_ - numFalseStarts_;
}

Count ExecutionCount::NumConsecutiveSuccesses() const
{
    Locker locker(this);
    return numConsecutiveSuccesses_;
}

Count ExecutionCount::NumConsecutiveFailures() const
{
    Locker locker(this);
    return numConsecutiveFailures_;
}

Count ExecutionCount::NumFalseStarts() const
{
    Locker locker(this);
    return numFalseStarts_;
}

void ExecutionCount::Start()
{
    Locker locker(this);
    ++totalExecutionCount_;
}

void ExecutionCount::Suspend()
{
    Locker locker(this);
    ++numSuspended_;
}

void ExecutionCount::Resume()
{
    // TODO: Count?
}

void ExecutionCount::Success()
{
    Locker locker(this);
    numConsecutiveFailures_ = 0;
    ++numConsecutiveSuccesses_;
    ++executionCount_;

    ASSERT(totalExecutionCount_ >= executionCount_);
}

void ExecutionCount::Failure()
{
    Locker locker(this);
    ++numConsecutiveFailures_;
    numConsecutiveSuccesses_ = 0;
}

void ExecutionCount::FalseStart()
{
    Locker locker(this);
    ++totalExecutionCount_;
    ++numFalseStarts_;
}

void ExecutionCount::Interrupt()
{
    Locker locker(this);
    ++totalInterruptedCount_;
}

}}
