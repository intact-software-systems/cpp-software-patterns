#include"RxCommand/CommandControllerPolicy.h"
#include"RxCommand/Strategy/ParallelComputationStrategy.h"
#include"RxCommand/Strategy/SequentialComputationStrategy.h"

namespace Reactor
{

CommandControllerPolicy::CommandControllerPolicy()
    : lifetime_(Policy::Attempt::NumSuccessfulTimes(1, 5))
    , interval_(Policy::Interval::RunImmediatelyAndThenEvery(500))
    , undoRedo_(Policy::UndoRedo::UndoAndRedo())
    , computation_(Policy::Computation::Parallel())
    , composition_(Policy::Composition::Aggregate())
    , ordering_(Policy::Ordering::Fifo())
    , timeout_(Policy::Timeout::FromMilliseconds(60000))
    , successCriterion_(Policy::Criterion::All())
    , maxConcurrentCommands_(10)
{}

CommandControllerPolicy::CommandControllerPolicy(
        Policy::Attempt lifetime,
        Policy::Interval interval,
        Policy::UndoRedo undoRedo,
        Policy::Computation computation,
        Policy::Composition composition,
        Policy::Ordering ordering)
    : lifetime_(lifetime)
    , interval_(interval)
    , undoRedo_(undoRedo)
    , computation_(computation)
    , composition_(composition)
    , ordering_(ordering)
    , timeout_(Policy::Timeout::FromMilliseconds(60000))
    , successCriterion_(Policy::Criterion::All())
    , maxConcurrentCommands_(10)
{ }

CommandControllerPolicy::CommandControllerPolicy(
        Policy::Computation computation,
        Policy::Ordering ordering,
        Policy::Timeout timeout,
        Policy::Criterion criterion,
        Policy::MaxLimit<int> limit)
    : lifetime_(Policy::Attempt::NumSuccessfulTimes(1, 5))
    , interval_(Policy::Interval::RunImmediatelyAndThenEvery(500))
    , undoRedo_(Policy::UndoRedo::UndoAndRedo())
    , computation_(computation)
    , composition_(Policy::Composition::Aggregate())
    , ordering_(ordering)
    , timeout_(timeout)
    , successCriterion_(criterion)
    , maxConcurrentCommands_(limit)
{ }

CommandControllerPolicy::~CommandControllerPolicy()
{ }

Policy::Attempt CommandControllerPolicy::GetAttempt() const
{
    return lifetime_;
}

Policy::Timeout CommandControllerPolicy::Timeout() const
{
    return timeout_;
}

Policy::Interval CommandControllerPolicy::GetInterval() const
{
    return interval_;
}

Policy::UndoRedo CommandControllerPolicy::UndoRedo() const
{
    return undoRedo_;
}

Policy::Computation CommandControllerPolicy::Computation() const
{
    return computation_;
}

Policy::Composition CommandControllerPolicy::Composition() const
{
    return composition_;
}

Policy::Ordering CommandControllerPolicy::Ordering() const
{
    return ordering_;
}

Policy::MaxLimit<int> CommandControllerPolicy::MaxConcurrentCommands() const
{
    return maxConcurrentCommands_;
}

ComputationStrategy *CommandControllerPolicy::findReadyCommands()
{
    switch(computation_.Kind())
    {
    case Policy::ComputationKind::SEQUENTIAL:
        return SequentialComputationStrategy::InstancePtr();
    case Policy::ComputationKind::CONCURRENT:
    case Policy::ComputationKind::PARALLEL:
    default:
        return ParallelComputationStrategy::InstancePtr();
    }
}

CommandControllerPolicy CommandControllerPolicy::Default()
{
    return ParallelAnd(Policy::Ordering::Fifo(), Policy::Timeout::No());
}

CommandControllerPolicy CommandControllerPolicy::SequentialAnd(Policy::Ordering ordering, Policy::Timeout timeout)
{
    return CommandControllerPolicy(
                Policy::Computation::Sequential(),
                ordering,
                timeout,
                Policy::Criterion::Unconditional(),
                Policy::MaxLimit<int>::InclusiveLimitTo(20)
                );
}

CommandControllerPolicy CommandControllerPolicy::ParallelAnd(Policy::Ordering ordering, Policy::Timeout timeout)
{
    return CommandControllerPolicy(
                Policy::Computation::Parallel(),
                ordering,
                timeout,
                Policy::Criterion::Unconditional(),
                Policy::MaxLimit<int>::InclusiveLimitTo(20)
                );
}

CommandControllerPolicy CommandControllerPolicy::ConcurrentAnd(Policy::Ordering ordering, Policy::Timeout timeout)
{
    return CommandControllerPolicy(
                Policy::Computation::Concurrent(),
                ordering,
                timeout,
                Policy::Criterion::Unconditional(),
                Policy::MaxLimit<int>::InclusiveLimitTo(20)
                );
}

}
