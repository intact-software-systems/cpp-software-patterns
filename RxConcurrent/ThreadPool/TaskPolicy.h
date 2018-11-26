#pragma once

#include"RxConcurrent/IncludeExtLibs.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

/**

 Deadline - execute task within some time, otherwise future<T>.waitFor() throws exception and ThreadPool?? disregards the execution.

 This may enable mechanisms as: Fast forward over tasks that are out-dated because of Lifespan or Deadline

 Deadline may be "Execute a task N times within X msecs", "Lifespan may be don't execute anything more after X minutes".

 In the context of TaskPolicy it may seem that Deadline == Lifespan, except for:

 Lifespan - Continue to be alive even after scheduling is done. Helps to support Trigger(Task *task).
 Lease - Renew lease when triggered (if suspended for a long time then renew lifespan or not)

 TODO: Interval should include (Interval::FromEndOfTask, Interval::FromBeginningOfTask),

*/
class DLL_STATE TaskPolicy
{
public:
    TaskPolicy();

    TaskPolicy(
        Policy::Attempt attempt,
        Policy::Interval interval,
        Policy::Interval retryInterval,
        Policy::Timeout timeout,
        Policy::Lifespan lifespan,
        Policy::Deadline deadline,
        Policy::LeasePlan lease
    );

    virtual ~TaskPolicy();

    const Policy::Attempt  & Attempt() const;
    const Policy::Interval & Interval() const;
    const Policy::Interval & RetryInterval() const;
    const Policy::Lifespan & Lifespan() const;
    const Policy::Timeout  & Timeout() const;
    const Policy::Deadline & Deadline() const;
    const Policy::LeasePlan& Lease() const;

    static TaskPolicy RunOnceImmediately();
    static TaskPolicy RunOnceIn(Duration duration);
    static TaskPolicy RunForever();
    static TaskPolicy RunForever(Duration interval);
    static TaskPolicy Default();

    friend std::ostream& operator<<(std::ostream& ostr, const TaskPolicy& policy)
    {
        ostr << "TaskPolicy: " << policy.attempt_ << ", " << policy.interval_ << ", " << policy.retryInterval_ << ", " << policy.timeout_;
        return ostr;
    }

private:
    Policy::Attempt     attempt_;
    Policy::Interval    interval_;
    Policy::Interval    retryInterval_;
    Policy::Timeout     timeout_;
    Policy::Computation computation_;
    Policy::Lifespan    lifespan_;      // Continue to be alive even after scheduling is done. Helps to support Trigger(Task *task).
    Policy::Deadline    deadline_;      // Execute a task N times within X msecs
    Policy::LeasePlan   lease_;
};

}}
