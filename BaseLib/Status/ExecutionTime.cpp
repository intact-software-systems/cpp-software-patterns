#include"BaseLib/Status/ExecutionTime.h"

namespace BaseLib { namespace Status {

ExecutionTime::ExecutionTime()
    : lastExecutionTime_(Timestamp::min())
    , lastFailedExecutionTime_(Timestamp::min())
    , currentExecutionTime_()
{ }

ExecutionTime::~ExecutionTime()
{ }

Timestamp ExecutionTime::LastSuccessfulExecutionTime() const
{
    Locker locker(this);
    return lastExecutionTime_;
}

Timestamp ExecutionTime::LastFailedExecutionTime() const
{
    Locker locker(this);
    return lastFailedExecutionTime_;
}

Timestamp ExecutionTime::CurrentExecutionTime() const
{
    Locker locker(this);
    return currentExecutionTime_;
}

Duration ExecutionTime::TimeSinceLastExecutionTime() const
{
    Locker locker(this);
    return Timestamp::Now() - currentExecutionTime_;
}

Duration ExecutionTime::TimeSinceLastFalseStart() const
{
    Locker locker(this);
    return Timestamp::Now() - lastFailedExecutionTime_;
}

bool ExecutionTime::DidLastExecutionFail() const
{
    Locker locker(this);
    return lastFailedExecutionTime_ > lastExecutionTime_;
}

void ExecutionTime::Start()
{
    Locker locker(this);
    currentExecutionTime_ = Timestamp::Now();
}

void ExecutionTime::Suspend()
{
    Locker locker(this);
    suspendTime_ = Timestamp::Now();
}

void ExecutionTime::Resume()
{
    // Nothing to do?
}

void ExecutionTime::Success()
{
    Locker locker(this);
    lastExecutionTime_ = currentExecutionTime_;
}

void ExecutionTime::Failure()
{
    Locker locker(this);
    lastFailedExecutionTime_ = currentExecutionTime_;
}

void ExecutionTime::FalseStart()
{
    Locker locker(this);
    currentExecutionTime_ = lastExecutionTime_;

    lastFalseStart_ = Timestamp::Now();
}

void ExecutionTime::Interrupt()
{
    Locker locker(this);
    interruptedTime_ = Timestamp::Now();
}

}}
