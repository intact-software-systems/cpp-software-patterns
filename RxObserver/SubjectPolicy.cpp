#include "RxObserver/SubjectPolicy.h"

namespace BaseLib { namespace Concurrent
{

SubjectPolicy::SubjectPolicy()
    : timeout_(Policy::Timeout::FromSeconds(30))
    , computation_(Policy::Computation::Concurrent())
    , invocation_(Policy::Invocation::Async())
    , maxNumSubscribers_(Policy::MaxLimit<int>::InclusiveLimitTo(std::numeric_limits<int>::max()))
    , maxNumPublishers_(Policy::MaxLimit<int>::InclusiveLimitTo(std::numeric_limits<int>::max()))
{ }

SubjectPolicy::~SubjectPolicy()
{ }

Policy::Timeout SubjectPolicy::Timeout() const
{
    return timeout_;
}

Policy::TimeBasedFilter SubjectPolicy::MinSeparation() const
{
    return filterInterval_;
}

Policy::Replay SubjectPolicy::Replay() const
{
    return replay_;
}

Policy::Ordering SubjectPolicy::Ordering() const
{
    return ordering_;
}

Policy::Invocation SubjectPolicy::Invocation() const
{
    return invocation_;
}

Policy::Computation SubjectPolicy::Computation() const
{
    return computation_;
}

int SubjectPolicy::MaxNumSubscribers() const
{
    return maxNumSubscribers_.Clone();
}

int SubjectPolicy::MaxNumPublishers() const
{
    return maxNumPublishers_.Clone();
}

FlowControllerPolicy SubjectPolicy::FlowPolicy() const
{
    return flowPolicy_;
}

}}
