#pragma once

#include <BaseLib/Status/AccessStatus.h>
#include <BaseLib/Status/FlowStatus.h>
#include <BaseLib/Status/ExecutionStatus.h>
#include <BaseLib/Policy/IncludeLibs.h>

#include"BaseLib/Export.h"

namespace BaseLib { namespace Strategy {

// ------------------------------------------------------------
// Strategies to check if state and policy allows execution
// ------------------------------------------------------------

struct DLL_STATE ComputeNumPacketsToDequeue
{
    static int Perform(const Status::FlowStatus& status, const Policy::Throughput& throughput);
};

/**
 * @brief The AIMDStrategy class implements the additive-increase/multiplicative-decrease (AIMD) algorithm
 * is a feedback control algorithm best known for its use in TCP Congestion Avoidance.
 *
 * Let w(t) be the sending rate (e.g. the congestion window) during time slot t, a (a > 0) be the additive
 * increase parameter, and b (0 < b < 1) be the multiplicative decrease factor.
 *
 * w(t + 1) = w(t) + a  \\ if congestion is not detected
 * w(t + 1) = w(t) x b  \\ if congestion is detected
 *
 * In TCP, after slow start, the additive increase parameter a is typically one MSS (maximum segment size)
 * per round-trip time, and the multiplicative decrease factor b is typically 1/2.
 *
 * Ref: http://en.wikipedia.org/wiki/Additive_increase/multiplicative_decrease
 */
struct DLL_STATE CalculateTimeRateAIMD
{
    static Status::TimeRate Perform(const Status::FlowStatus& status, const Policy::Throughput& maxThroughput, const Policy::Congestion& congestion);
};

// ------------------------------------------------------------
// Strategies to check if state and policy allows execution
// ------------------------------------------------------------

struct DLL_STATE IsInLifetime
{
    static bool Perform(const Status::AccessStatus& status, const Policy::Lifespan& lifespan);
};

// ------------------------------------------------------------
// Strategies to check if state and policy allows execution
// ------------------------------------------------------------

struct DLL_STATE IsInLifespan
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Lifespan& lifespan);
};

struct DLL_STATE IsInInterval
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval);
};

struct DLL_STATE IsInRetryInterval
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval);
};

struct DLL_STATE IsInAttempt
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Attempt& attempt);
};

struct DLL_STATE IsTimeout
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Timeout& timeout);
};

struct DLL_STATE IsIdle
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Timeout& timeout);
};

struct DLL_STATE IsReadyToExecute
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Attempt& attempt, const Policy::Interval& interval, const Policy::Interval& retryInterval);
};

// ------------------------------------------------------------
// Strategies to compute next execution time
// ------------------------------------------------------------

struct DLL_STATE ComputeTimeUntilNextExecution
{
    static Duration Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval);
};

struct DLL_STATE ComputeTimeUntilNextRetry
{
    static Duration Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval);
};

struct DLL_STATE ComputeTimeUntilTimeout
{
    static Duration Perform(const Status::ExecutionStatus& status, const Policy::Timeout& timeout);
};

struct DLL_STATE ComputeEarliestReadyIn
{
    static Duration Perform
                        (
                            const Status::ExecutionStatus& status,
                            const Policy::Attempt& attempt,
                            const Policy::Interval& interval,
                            const Policy::Interval& retryInterval,
                            const Policy::Timeout& timeout
                        );
};

struct DLL_STATE ComputeExponentialBackoff
{
    static Duration Perform(const Status::ExecutionStatus& t, const Policy::Interval& retryInterval);
};

// ------------------------------------------------------------
// Strategies to check for policy violations
// ------------------------------------------------------------

struct DLL_STATE IsLifetimePolicyViolated
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Attempt& attempt);
};

struct DLL_STATE IsIntervalPolicyViolated
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval);
};

struct DLL_STATE IsTimeoutPolicyViolated
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Timeout& timeout);
};

struct DLL_STATE IsPolicyViolated
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Interval& interval, const Policy::Timeout& timeout, const Policy::Attempt& attempt);
};

struct DLL_STATE IsCriterionMet
{
    static bool Perform(const Status::ExecutionStatus& status, const Policy::Criterion& successCriterion);
};

}}

