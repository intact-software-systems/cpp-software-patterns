#include"RxObserver/FlowControlStrategy.h"

namespace BaseLib {
namespace Strategy {

Concurrent::EventResults PlayEventsToObservers::Perform(
    const Concurrent::Observers::Ptr& observers,
    const Concurrent::Events& events,
    const Policy::Computation& computation,
    const Policy::Timeout& timeout
) const
{
    if(observers->IsEmpty())
    {
        ICRITICAL() << "No observers to submit to!";
        return Concurrent::EventResults();
    }

    Concurrent::EventResults results;

    for(Concurrent::Event::Ptr event : events)
    {
        Concurrent::Observers::Ptr observerGroup(new Concurrent::Observers(observers->state().Obs()));

        Concurrent::EventResult::Ptr eventResult(new Concurrent::EventResult(event, observerGroup));
        results.push_back(eventResult);

        for(std::shared_ptr<Concurrent::ObserverAction> observer : observerGroup->state().Obs())
        {
            Concurrent::ObserverEvent::Ptr observerEvent(new Concurrent::ObserverEvent(eventResult, observer));
            observer->OnNext(observerEvent);
        }

        if(computation.IsSequential())
        {
            eventResult->WaitFor(timeout.InMillis());
        }
    }

    return results;
}

}
}
