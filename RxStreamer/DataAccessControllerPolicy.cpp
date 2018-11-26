#include "RxStreamer/DataAccessControllerPolicy.h"

namespace Reactor
{

DataAccessControllerPolicy::DataAccessControllerPolicy(Policy::Attempt attempt, Policy::Interval interval, Policy::Interval retryInterval, Policy::Timeout timeout)
    : attempt_(attempt)
    , interval_(interval)
    , retryInterval_(retryInterval)
    , timeout_(timeout)
{ }

DataAccessControllerPolicy::~DataAccessControllerPolicy()
{ }

Policy::Attempt DataAccessControllerPolicy::attempt() const
{
    return attempt_;
}

Policy::Interval DataAccessControllerPolicy::interval() const
{
    return interval_;
}

Policy::Interval DataAccessControllerPolicy::retryInterval() const
{
    return retryInterval_;
}

Policy::Timeout DataAccessControllerPolicy::timeout() const
{
    return timeout_;
}

DataAccessControllerPolicy DataAccessControllerPolicy::Default()
{
    return DataAccessControllerPolicy(Policy::Attempt::UntilSuccess(5), Policy::Interval::RunEveryMs(30000), Policy::Interval::FromMinutes(2), Policy::Timeout::FromMinutes(1));
}

}
