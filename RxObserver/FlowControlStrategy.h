#pragma once

#include"RxObserver/ObserverEvent.h"
#include"RxObserver/EventResult.h"
#include"RxObserver/Observers.h"

#include"RxObserver/Export.h"

namespace BaseLib { namespace Strategy
{

class DLL_STATE PlayEventsToObservers
        : public Templates::StrategyConst4<Concurrent::EventResults, Concurrent::Observers::Ptr, Concurrent::Events, Policy::Computation, Policy::Timeout>
        , public Concurrent::StaticStrategyConstFutureFactory4<Concurrent::EventResults, Concurrent::Observers::Ptr, Concurrent::Events, Policy::Computation, Policy::Timeout>
        , public StaticSingletonPtr<PlayEventsToObservers>
{
public:
    virtual ~PlayEventsToObservers() {}

    virtual Concurrent::EventResults Perform(
            const Concurrent::Observers::Ptr &observers,
            const Concurrent::Events &events,
            const Policy::Computation &computation,
            const Policy::Timeout &timeout) const;
};

}}
