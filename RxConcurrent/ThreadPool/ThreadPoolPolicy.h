#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * @brief The ThreadPoolPolicy class
 *
 * TODO: Allow interval to control throughput, i.e., "maximum allowed".
 */
class DLL_STATE ThreadPoolPolicy
{
public:
    ThreadPoolPolicy(
        Policy::Attempt attempt,
        Policy::Interval interval,
        Policy::Computation computation,
        Policy::Ordering ordering,
        Policy::MaxLimit<int64> maxThreadCount
    );

    virtual ~ThreadPoolPolicy();

    const Policy::Attempt        & Attempt() const;
    const Policy::Interval       & Interval() const;
    const Policy::Computation    & Computation() const;
    const Policy::Ordering       & Ordering() const;
    const Policy::MaxLimit<int64>& MaxThreadCount() const;

    static ThreadPoolPolicy Default();

    static ThreadPoolPolicy Custom(Policy::MaxLimit<int64> threadCount);

private:
    Policy::Attempt     attempt_;
    Policy::Interval    interval_;
    Policy::Computation computation_;
    Policy::Ordering    ordering_;

    Policy::MaxLimit<int64> maxThreadCount_;
};

}}
