#include"RxConcurrent/ThreadPool/ThreadPoolPolicy.h"

namespace BaseLib { namespace Concurrent {

ThreadPoolPolicy::ThreadPoolPolicy(
    Policy::Attempt attempt,
    Policy::Interval interval,
    Policy::Computation computation,
    Policy::Ordering ordering,
    Policy::MaxLimit<int64> maxThreadCount
)
    : attempt_(attempt)
    , interval_(interval)
    , computation_(computation)
    , ordering_(ordering)
    , maxThreadCount_(maxThreadCount)
{ }

ThreadPoolPolicy::~ThreadPoolPolicy()
{ }

const Policy::Attempt& ThreadPoolPolicy::Attempt() const
{
    return this->attempt_;
}

const Policy::Interval& ThreadPoolPolicy::Interval() const
{
    return interval_;
}

const Policy::Computation& ThreadPoolPolicy::Computation() const
{
    return computation_;
}

const Policy::Ordering& ThreadPoolPolicy::Ordering() const
{
    return ordering_;
}

const Policy::MaxLimit<int64>& ThreadPoolPolicy::MaxThreadCount() const
{
    return maxThreadCount_;
}

ThreadPoolPolicy ThreadPoolPolicy::Default()
{
    return ThreadPoolPolicy(
        Policy::Attempt::Default(),
        Policy::Interval::Default(),
        Policy::Computation::Sequential(),
        Policy::Ordering::Random(),
        Policy::MaxLimit<int64>::LimitTo(Policy::LimitKind::INCLUSIVE, Thread::idealThreadCount())
    );
}

ThreadPoolPolicy ThreadPoolPolicy::Custom(Policy::MaxLimit<int64> threadCount)
{
    return ThreadPoolPolicy(
        Policy::Attempt::Default(),
        Policy::Interval::Default(),
        Policy::Computation::Sequential(),
        Policy::Ordering::Random(),
        threadCount
    );
}

}}
