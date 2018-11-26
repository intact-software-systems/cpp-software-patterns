#include "EventPolicy.h"

namespace BaseLib { namespace Concurrent
{

EventPolicy::EventPolicy(Policy::Timeout timeout)
    : timeout_(timeout)
{ }

EventPolicy::~EventPolicy()
{ }

Policy::Timeout EventPolicy::Timeout() const
{
    return timeout_;
}

EventPolicy EventPolicy::Default()
{
    return EventPolicy(Policy::Timeout::FromMilliseconds(5000));
}

}}
