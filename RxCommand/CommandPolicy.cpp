#include"RxCommand/CommandPolicy.h"

namespace Reactor {

CommandPolicy::CommandPolicy()
    : policy_(TaskPolicy::Default())
    , undoRedo_(Policy::UndoRedo::UndoAndRedo())
    , successCriterion_(Policy::Criterion::All())
    , composition_(Policy::Composition::Individual())
{ }

CommandPolicy::CommandPolicy(TaskPolicy policy, Policy::Composition composition, Policy::Criterion successCriterion)
    : policy_(policy)
    , undoRedo_(Policy::UndoRedo::UndoAndRedo())
    , successCriterion_(successCriterion)
    , composition_(composition)
{ }

CommandPolicy::~CommandPolicy()
{ }

Policy::Criterion CommandPolicy::SuccessCriterion() const
{
    return successCriterion_;
}

TaskPolicy CommandPolicy::GetTaskPolicy() const
{
    return policy_;
}

CommandPolicy CommandPolicy::Default()
{
    return CommandPolicy(
        TaskPolicy(
            Policy::Attempt::Default(),
            Policy::Interval::Default(),
            Policy::Interval::Default(),
            Policy::Timeout::FromMilliseconds(60000),
            Policy::Lifespan::Infinite(),
            Policy::Deadline::FromDuration(Duration::Infinite()),
            Policy::LeasePlan::NoRenew()),
        Policy::Composition::Individual(),
        Policy::Criterion::All());
}

CommandPolicy CommandPolicy::OnceAggregate(Policy::Interval retryInterval, Policy::Timeout timeout, int maxNumberAttempts)
{
    return CommandPolicy(
        TaskPolicy(
            Policy::Attempt::UntilSuccess(maxNumberAttempts),
            Policy::Interval::RunImmediatelyAndThenEvery(1000),
            retryInterval,
            timeout,
            Policy::Lifespan::Infinite(),
            Policy::Deadline::FromDuration(Duration::Infinite()),
            Policy::LeasePlan::NoRenew()),
        Policy::Composition::Aggregate(),
        Policy::Criterion::All());
}

CommandPolicy CommandPolicy::OnceIndividual(Policy::Interval retryInterval, Policy::Timeout timeout, int maxNumberAttempts)
{
    return CommandPolicy(
        TaskPolicy(
            Policy::Attempt::UntilSuccess(maxNumberAttempts),
            Policy::Interval::RunImmediatelyAndThenEvery(1000),
            retryInterval,
            timeout,
            Policy::Lifespan::Infinite(),
            Policy::Deadline::FromDuration(Duration::Infinite()),
            Policy::LeasePlan::NoRenew()),
        Policy::Composition::Individual(),
        Policy::Criterion::All());
}

CommandPolicy CommandPolicy::OnceImmediatelyIndividual(Policy::Timeout timeout, int maxNumberAttempts)
{
    return CommandPolicy(
        TaskPolicy(
            Policy::Attempt::UntilSuccess(maxNumberAttempts),
            Policy::Interval::RunImmediatelyAndThenEvery(1000),
            Policy::Interval::DelayedRunAndThenEvery(1000, 1000),
            timeout,
            Policy::Lifespan::Infinite(),
            Policy::Deadline::FromDuration(Duration::Infinite()),
            Policy::LeasePlan::NoRenew()),
        Policy::Composition::Individual(),
        Policy::Criterion::All());
}

CommandPolicy CommandPolicy::ForeverIndividual(Policy::Interval interval, Policy::Interval retryInterval, Policy::Timeout timeout)
{
    return CommandPolicy(
        TaskPolicy(
            Policy::Attempt::Forever(),
            interval,
            retryInterval,
            timeout,
            Policy::Lifespan::Infinite(),
            Policy::Deadline::FromDuration(Duration::Infinite()),
            Policy::LeasePlan::NoRenew()),
        Policy::Composition::Individual(),
        Policy::Criterion::Unconditional());
}

}
