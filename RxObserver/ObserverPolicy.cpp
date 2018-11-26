#include "RxObserver/ObserverPolicy.h"

namespace BaseLib { namespace Concurrent
{

ObserverPolicy::ObserverPolicy()
{ }

ObserverPolicy::~ObserverPolicy()
{ }

Policy::Replay ObserverPolicy::Replay() const
{
    return replay_;
}

Policy::Deadline ObserverPolicy::Deadline() const
{
    return deadline_;
}

Policy::TimeBasedFilter ObserverPolicy::MinSeparation() const
{
    return minSeparation_;
}

FlowControllerPolicy ObserverPolicy::FlowPolicy() const
{
    return flowPolicy_;
}

}}
