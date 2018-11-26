#include "RxConcurrent/FlowControllerPolicy.h"

namespace BaseLib { namespace Concurrent
{

FlowControllerPolicy::FlowControllerPolicy(TaskPolicy policy)
    : taskPolicy_(policy)
    , throughput_(Policy::Throughput::PerSecond(1000))
    , history_(Policy::History::KeepLast(5))
    , deadline_(Policy::Deadline::FromSeconds(5))
    , maxQueueSize_(Policy::MaxLimit<int>::LimitTo(Policy::LimitKind::INCLUSIVE, 100))
    , congestion_(Policy::MaxLimit<float>::InclusiveLimitTo(1), Policy::MaxLimit<float>::InclusiveLimitTo(0.8f))
    , minSeparation_(Policy::TimeBasedFilter::WithMinSeparation(Duration::FromMilliseconds(100)))
{}

FlowControllerPolicy::~FlowControllerPolicy()
{}

const TaskPolicy &FlowControllerPolicy::Task() const
{
    return taskPolicy_;
}

Policy::Throughput FlowControllerPolicy::Throughput() const
{
    return throughput_;
}

Policy::History FlowControllerPolicy::History() const
{
    return history_;
}

Policy::Deadline FlowControllerPolicy::Deadline() const
{
    return deadline_;
}

Policy::MaxLimit<int> FlowControllerPolicy::MaxQueueSize() const
{
    return maxQueueSize_;
}

Policy::Congestion FlowControllerPolicy::Congestion() const
{
    return congestion_;
}

Policy::TimeBasedFilter FlowControllerPolicy::MinSeparation() const
{
    return minSeparation_;
}

FlowControllerPolicy FlowControllerPolicy::Default()
{
    return FlowControllerPolicy();
}

}}
