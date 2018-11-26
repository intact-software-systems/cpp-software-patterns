#include"BaseLib/Strategy/ExecutionChecker.h"

namespace BaseLib { namespace Strategy {

// ------------------------------------------------------------
// Strategies to check if state and policy allows execution
// ------------------------------------------------------------

int ComputeNumPacketsToDequeue::Perform(const Status::FlowStatus& , const Policy::Throughput& )
{
    //IINFO() << " dequeue max(0," << throughput.Rate().PerSecond() << " - " << status.Dequeued().SumAsTimeRate().PerSecond() << ")";
    //return std::max(0, int(status.Dequeued().SumAsTimeRate().PerSecond() - throughput.Rate().PerSecond()));
    return INT_MAX; //std::max(0, throughput.Rate().PerSecond() - status.Dequeued().SumAsTimeRate().PerSecond());
}

Status::TimeRate CalculateTimeRateAIMD::Perform(const Status::FlowStatus& , const Policy::Throughput& /*maxThroughput*/, const Policy::Congestion& /*congestion*/)
{
    // ---------------------------------------------------------
    // Calculate rate based on current flow status throughput
    // and congestion policy, throughput policy represents maximum.
    // ---------------------------------------------------------

    // TODO: How to detect congestion?
    // Queue is full?

    return Status::TimeRate(1, Duration::FromSeconds(1));
}
// ------------------------------------------------------------
// Strategies to check if state and policy allows execution
// ------------------------------------------------------------

bool IsInLifetime::Perform(const Status::AccessStatus& status, const Policy::Lifespan& lifespan)
{
    return !status.IsDeleted() && status.Time().TimeSinceStarted() <= lifespan.delegate();
}

// ------------------------------------------------------------
// Strategies to check if state and policy allows execution
// ------------------------------------------------------------

bool IsInLifespan::Perform(const Status::ExecutionStatus& status, const Policy::Lifespan& lifespan)
{
    return status.Time().TimeSinceLastExecutionTime() > lifespan.AsDuration();
}

bool IsInInterval::Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval)
{
    if(status.IsSuspended())
    {
        return false;
    }

    // ------------------------------------------------------------------------------------------------
    // if "never run before" and "time since command creation" > "initial delayed start"
    // ------------------------------------------------------------------------------------------------
    if(status.IsNeverExecuted() && status.Time().TimeSinceLastExecutionTime() >= interval.Initial())
    {
        return true;
    }
    // ------------------------------------------------------------------------------------------------
    // (else if implied "run before") and if "time since last execution" >= "configured minimum interval period"
    // ------------------------------------------------------------------------------------------------
    else if(status.Time().TimeSinceLastExecutionTime() >= interval.Period())
    {
        return true;
    }

    // ------------------------------------------------------------------------------------------------
    // if "time since last execution" < "configured minimum interval period"
    // ------------------------------------------------------------------------------------------------
    //else if(executionState.getExecutionTime().getTimeSinceLastExecutionTimeMs() < interval.getPeriodMs()) {
    //    return false;
    //}
    //ASSERT(status.Time().TimeSinceLastExecutionTime() < interval.Period());

    return false;
}

bool IsInRetryInterval::Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval)
{
    if(status.IsLastExecutionFailure())
    {
        return IsInInterval::Perform(status, interval);
    }

    return false;
}

bool IsInAttempt::Perform(const BaseLib::Status::ExecutionStatus& status, const Policy::Attempt& attempt)
{
    bool executeCommand = false;

    // ---------------------------------------
    // If command is cancelled it is done
    // --------------------------------------
    if(status.IsCancelled())
    {
        return false;
    }

    // ---------------------------------------
    // Check execution counts
    // ---------------------------------------
    switch(attempt.Kind())
    {
        case Policy::AttemptKind::FOREVER:
        {
            executeCommand = true;
            break;
        }
        case Policy::AttemptKind::NUM_SUCCESSFUL_TIMES:
        {
            // ------------------------------------------------------------------------------------------------
            // if "number of successful executions" < "required number of successful executions" then doExecute
            // ------------------------------------------------------------------------------------------------
            if(status.Count().NumSuccesses() < attempt.MinNumSuccesses())
            {
                executeCommand = true;
            }

            // ------------------------------------------------------------------------------------------------
            // if "total number of attempts" >= "maximum number of attempts" then doNotExecute
            // ------------------------------------------------------------------------------------------------
            if(status.Count().TotalNumAttempts() >= attempt.MaxNumAttempts())
            {
                executeCommand = false;
            }
            break;
        }
        case Policy::AttemptKind::UNTIL_SUCCESS:
        {
            // ------------------------------------------------------------------------------------------------
            // if "number of successful executions" == 0 then doExecute
            // ------------------------------------------------------------------------------------------------
            if(status.Count().NumSuccesses() == 0)
            {
                executeCommand = true;
            }

            // ------------------------------------------------------------------------------------------------
            // if "total number of attempts" >= "maximum number of attempts" then doNotExecute
            // ------------------------------------------------------------------------------------------------
            if(status.Count().TotalNumAttempts() >= attempt.MaxNumAttempts())
            {
                executeCommand = false;
            }

            break;
        }
        default:
            IWARNING() << "Unrecognized command policy Lifetime.kind";
            break;
    }

    return executeCommand;
}

bool IsTimeout::Perform(const BaseLib::Status::ExecutionStatus& status, const Policy::Timeout& timeout)
{
    //IINFO() << status.Time().TimeSinceLastExecutionTime().InMillis() << " >= " << timeout.AsDuration().TimeSinceEpoch();
    return status.IsExecuting() && status.Time().TimeSinceLastExecutionTime() >= timeout.delegate();
}

bool IsIdle::Perform(const BaseLib::Status::ExecutionStatus& status, const Policy::Timeout& timeout)
{
    return !status.IsExecuting() && status.Time().TimeSinceLastExecutionTime() >= timeout.delegate();
}

bool IsReadyToExecute::Perform(
    const Status::ExecutionStatus& status,
    const Policy::Attempt& attempt,
    const Policy::Interval& interval,
    const Policy::Interval& retryInterval
)
{
    bool inLifetime      = IsInAttempt::Perform(status, attempt);
    bool inInterval      = IsInInterval::Perform(status, interval);
    bool inRetryInterval = IsInRetryInterval::Perform(status, retryInterval);

    // ---------------------------------------
    // Q: Is Timeout policy relevant here?
    // A: No, it is handled in controller
    // ---------------------------------------

    return (inLifetime && inInterval) || (inLifetime && inRetryInterval);
}

Duration ComputeTimeUntilNextExecution::Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval)
{
    if(status.IsSuspended())
    {
        return Duration::Infinite();
    }
    else if(status.IsNeverExecuted())
    {
        return std::max<Duration>(Duration::Zero(), interval.Initial() - status.Time().TimeSinceLastExecutionTime());

        //IDEBUG() << "Time to execute according to initialDelayMs " << initialDelayMs;
        //ASSERT(!status.IsStarted());
    }
    else // executionStatus.Count().TotalNumAttempts() > 0
    {
        return std::max<Duration>(Duration::Zero(), interval.Period() - status.Time().TimeSinceLastExecutionTime());

        //IDEBUG() << "Time to execute according to periodMs " << intervalMs;
    }
}

Duration ComputeTimeUntilNextRetry::Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval)
{
    if(IsInRetryInterval::Perform(status, interval))
    {
        return ComputeTimeUntilNextExecution::Perform(status, interval);
    }

    return Duration::Infinite();
}

Duration ComputeTimeUntilTimeout::Perform(const Status::ExecutionStatus& status, const Policy::Timeout& timeout)
{
    if(!status.IsExecuting() || status.IsSuspended()) return Duration::Infinite();

    return std::max<Duration>(Duration::Zero(), timeout.delegate() - status.Time().TimeSinceLastExecutionTime());
}

Duration ComputeEarliestReadyIn::Perform(
    const Status::ExecutionStatus& status,
    const Policy::Attempt& attempt,
    const Policy::Interval& interval,
    const Policy::Interval& retryInterval,
    const Policy::Timeout& timeout
)
{
    Duration nextExecution = Duration::Infinite();

    if(IsInAttempt::Perform(status, attempt) && !status.IsExecuting())
    {
        nextExecution = ComputeTimeUntilNextExecution::Perform(status, interval);
    }
    else if(status.IsExecuting())
    {
        nextExecution = interval.Period();
    }

    Duration nextRetry = ComputeTimeUntilNextRetry::Perform(status, retryInterval);

    Duration nextTimeout = Duration::Infinite();
    if(!status.IsInterrupted())
    {
        nextTimeout = ComputeTimeUntilTimeout::Perform(status, timeout);
    }
    else
        IINFO() << "Was interrupted " << status;

    // -----------------------------------------
    // Compute earliest ready time based on policies and status
    // -----------------------------------------
    Duration earliestReadyIn = Duration::Infinite();
    earliestReadyIn = std::min<Duration>(earliestReadyIn, nextExecution);
    earliestReadyIn = std::min<Duration>(earliestReadyIn, nextRetry);
    earliestReadyIn = std::min<Duration>(earliestReadyIn, nextTimeout);

//    if(earliestReadyIn == Duration::Infinite())
//    {
//        IWARNING() << "Earliest ready time FOREVER. isExecuting = " << status.IsExecuting()
//        << ", nextExecution = " << nextExecution << ", nextRetry = " << nextRetry << ", nextTimeout = " << nextTimeout;
//    }

    {
        //IDEBUG() << "earliestReadyIn = " << earliestReadyIn << ". isExecuting = " << status.IsExecuting()
        //<< ", nextExecution = " << nextExecution << ", nextRetry = " << nextRetry << ", nextTimeout = " << nextTimeout;
    }

    if(status.IsFalseStart() && earliestReadyIn <= Duration::Zero())
    {
        earliestReadyIn = ComputeExponentialBackoff::Perform(status, retryInterval);
        //IWARNING() << "False start - backing off and making new attempt in " << earliestReadyIn << " ms";
    }

    // --------------------------------------------
    // Post conditions
    // --------------------------------------------
    if(earliestReadyIn <= Duration::Zero())
    {
        ASSERT(!status.IsSuspended());

        if(nextTimeout <= Duration::zero())
        {
            ASSERT(status.IsExecuting());
        }
        else
        {
            ASSERT(!status.IsExecuting());
            ASSERT(IsReadyToExecute::Perform(status, attempt, interval, retryInterval));
        }
        //IDEBUG() << "Ready NOW! earliestReadyIn = " << earliestReadyIn << ". isExecuting = " << status.IsExecuting() << ", nextExecution = " << nextExecution << ", nextRetry = " << nextRetry << ", nextTimeout = " << nextTimeout;
    }

    return earliestReadyIn;
}

Duration ComputeExponentialBackoff::Perform(const Status::ExecutionStatus& status, const Policy::Interval& retryInterval)
{
    return std::max(status.Count().NumConsecutiveFailures().Clone() * (int) retryInterval.Period().InMillis(), status.Count().NumFalseStarts().Clone() * 1000);
}

bool IsLifetimePolicyViolated::Perform(const Status::ExecutionStatus& status, const Policy::Attempt& attempt)
{
    if(status.Count().TotalNumAttempts() > attempt.MaxNumAttempts())
    {
        return true;
    }
    else if(status.Count().NumSuccesses() > attempt.MinNumSuccesses())
    {
        return true;
    }

    return false;
}

bool IsIntervalPolicyViolated::Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval)
{
    if(status.IsSuspended())
    {
        return false;
    }
    else if(status.IsNeverExecuted())
    {
        if(status.Time().TimeSinceLastExecutionTime() > interval.Initial())
        {
            return true;
        }
    }
    else
    {
        if(status.Time().TimeSinceLastExecutionTime() > interval.Period())
        {
            return true;
        }
    }

    return false;
}

bool IsTimeoutPolicyViolated::Perform(const Status::ExecutionStatus& status, const Policy::Timeout& timeout)
{
    return status.IsExecuting() && status.Time().TimeSinceLastExecutionTime() > timeout.delegate();
}

bool IsPolicyViolated::Perform(
    const Status::ExecutionStatus& status,
    const Policy::Interval& interval,
    const Policy::Timeout& timeout,
    const Policy::Attempt& attempt
)
{
    if(IsIntervalPolicyViolated::Perform(status, interval))
    {
        return true;
    }
    else if(IsTimeoutPolicyViolated::Perform(status, timeout))
    {
        return true;
    }
    else if(IsLifetimePolicyViolated::Perform(status, attempt))
    {
        return true;
    }

    return false;
}

bool IsCriterionMet::Perform(const Status::ExecutionStatus& status, const Policy::Criterion& successCriterion)
{
    if(successCriterion.IsAll())
    {
        if(status.Count().NumFailures() > 0)
        {
            return false;
        }
    }
    else if(successCriterion.IsMinimum())
    {
        if(status.Count().NumFailures() > successCriterion.Limit().Limit())
        {
            return false;
        }
    }

    // Note! Unconditional criterion is always true
    return true;
}

}}
