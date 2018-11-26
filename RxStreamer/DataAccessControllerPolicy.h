#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessPolicy.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE DataAccessControllerPolicy
{
public:
    DataAccessControllerPolicy(Policy::Attempt attempt, Policy::Interval interval, Policy::Interval retryInterval, Policy::Timeout timeout);
    virtual ~DataAccessControllerPolicy();

    Policy::Attempt attempt() const;
    Policy::Interval interval() const;
    Policy::Interval retryInterval() const;
    Policy::Timeout timeout() const;

    static DataAccessControllerPolicy Default();

private:
    // TODO: Replace with TaskPolicy
    Policy::Attempt        attempt_;
    Policy::Interval       interval_;
    Policy::Interval       retryInterval_;
    Policy::Timeout        timeout_;
};

}
