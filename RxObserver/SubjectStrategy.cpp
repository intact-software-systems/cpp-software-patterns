#include "RxObserver/SubjectStrategy.h"

namespace BaseLib { namespace Strategy
{

Concurrent::Events FilterEventsOnTime::Perform(
        Concurrent::Events &,
        Policy::TimeBasedFilter )
{
    return Concurrent::Events();
}


Concurrent::ObserverEvents FilterObserverEventsOnTime::Perform(
        Concurrent::ObserverEvents &,
        Policy::TimeBasedFilter )
{
    return Concurrent::ObserverEvents();
}


}}
