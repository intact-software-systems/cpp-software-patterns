#include"RxConcurrent/ThreadPool/TaskPolicy.h"

namespace BaseLib { namespace Concurrent {

TaskPolicy::TaskPolicy()
    : attempt_(Policy::Attempt::Default())
    , interval_(Policy::Interval::Default())
    , retryInterval_(Policy::Interval::Default())
    , timeout_(Policy::Timeout::FromMilliseconds(10000))
    , computation_(Policy::Computation::Sequential())
    , lifespan_(Policy::Lifespan::Infinite())
    , deadline_(Policy::Deadline::FromMilliseconds(10000))
    , lease_(Policy::LeasePlan::NoRenew())
{ }

TaskPolicy::TaskPolicy(Policy::Attempt attempt, Policy::Interval interval, Policy::Interval retryInterval, Policy::Timeout timeout, Policy::Lifespan lifespan, Policy::Deadline deadline, Policy::LeasePlan lease)
    : attempt_(attempt)
    , interval_(interval)
    , retryInterval_(retryInterval)
    , timeout_(timeout)
    , computation_(Policy::Computation::Sequential())
    , lifespan_(lifespan)
    , deadline_(deadline)
    , lease_(lease)
{ }

TaskPolicy::~TaskPolicy()
{ }

const Policy::Attempt &TaskPolicy::Attempt() const
{
    return attempt_;
}

const Policy::Interval &TaskPolicy::Interval() const
{
    return interval_;
}

const Policy::Interval &TaskPolicy::RetryInterval() const
{
    return retryInterval_;
}

const Policy::Lifespan &TaskPolicy::Lifespan() const
{
    return lifespan_;
}

const Policy::Timeout &TaskPolicy::Timeout() const
{
    return timeout_;
}

const Policy::Deadline &TaskPolicy::Deadline() const
{
    return deadline_;
}

const Policy::LeasePlan &TaskPolicy::Lease() const
{
    return lease_;
}

TaskPolicy TaskPolicy::RunOnceImmediately()
{
    return TaskPolicy(
                Policy::Attempt::UntilSuccess(1),
                Policy::Interval::RunImmediately(),
                Policy::Interval::Default(),
                Policy::Timeout::FromMilliseconds(60000),
                Policy::Lifespan::Infinite(),
                Policy::Deadline::FromDuration(Duration::Infinite()),
                Policy::LeasePlan::NoRenew());
}

TaskPolicy TaskPolicy::RunOnceIn(Duration duration)
{
    return TaskPolicy(
                Policy::Attempt::UntilSuccess(1),
                Policy::Interval::DelayedRunAndThenEvery(duration, duration),
                Policy::Interval::Default(),
                Policy::Timeout::FromMilliseconds(60000),
                Policy::Lifespan::Infinite(),
                Policy::Deadline::FromDuration(Duration::Infinite()),
                Policy::LeasePlan::NoRenew());
}

TaskPolicy TaskPolicy::RunForever()
{
    return TaskPolicy(
                Policy::Attempt::Forever(),
                Policy::Interval::RunImmediatelyAndThenEvery(1000),
                Policy::Interval::Default(),
                Policy::Timeout::FromMilliseconds(60000),
                Policy::Lifespan::Infinite(),
                Policy::Deadline::FromDuration(Duration::Infinite()),
                Policy::LeasePlan::NoRenew());
}

TaskPolicy TaskPolicy::RunForever(Duration interval)
{
    return TaskPolicy(
                Policy::Attempt::Forever(),
                Policy::Interval::DelayedRunAndThenEvery(Duration::Zero(), interval),
                Policy::Interval::Default(),
                Policy::Timeout::FromMilliseconds(60000),
                Policy::Lifespan::Infinite(),
                Policy::Deadline::FromDuration(Duration::Infinite()),
                Policy::LeasePlan::NoRenew());
}

TaskPolicy TaskPolicy::Default()
{
    return TaskPolicy(
                Policy::Attempt::Default(),
                Policy::Interval::Default(),
                Policy::Interval::Default(),
                Policy::Timeout::FromMilliseconds(60000),
                Policy::Lifespan::Infinite(),
                Policy::Deadline::FromDuration(Duration::Infinite()),
                Policy::LeasePlan::NoRenew());
}

}}
