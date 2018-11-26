#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include "RxObserver/ObserverEvent.h"
#include"RxObserver/EventFilterCriterion.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Strategy
{

/**
 * @brief The FilterEventOnContent class
 */
template <typename EventArg, typename EventFilterArg>
class FilterEventOnContent
        : public Templates::StrategyConst2<bool, Concurrent::Event::Ptr, Concurrent::FilterList>
        , public StaticSingleton<FilterEventOnContent<EventArg, EventFilterArg>>
{
public:
    virtual bool Perform(
            const Concurrent::Event::Ptr &event,
            const Concurrent::FilterList &filters) const
    {
        typename EventArg::Ptr eventArg = std::dynamic_pointer_cast< EventArg > (event->Data());
        if(!eventArg)
        {
            IWARNING() << "Could not cast event data! Filtering out";
            return true;
        }

        bool isFiltered = false;
        for(Concurrent::FilterCriterion::Ptr filter : filters)
        {
            typename EventFilterArg::Ptr filterType = std::dynamic_pointer_cast< EventFilterArg >(filter);
            if(!filterType)
            {
                IWARNING() << "Could not cast event filter. Ignoring filter.";
                continue;
            }

            isFiltered = filterType->Filter(event, eventArg);

            if(isFiltered) {
                IDEBUG() << "Filtered out!";
                break;
            }
        }

        return isFiltered;
    }
};

/**
 * @brief The FilterEventOnTime class
 */
class DLL_STATE FilterEventsOnTime
        : public Templates::Strategy2<Concurrent::Events, Concurrent::Events &, Policy::TimeBasedFilter>
        , public StaticSingleton<FilterEventsOnTime>
{
public:
    virtual Concurrent::Events Perform(Concurrent::Events &, Policy::TimeBasedFilter);
};


/**
 * @brief The FilterEventOnTime class
 */
class DLL_STATE FilterObserverEventsOnTime
        : public Templates::Strategy2<Concurrent::ObserverEvents, Concurrent::ObserverEvents &, Policy::TimeBasedFilter>
        , public StaticSingleton<FilterObserverEventsOnTime>
{
public:
    virtual Concurrent::ObserverEvents Perform(Concurrent::ObserverEvents &, Policy::TimeBasedFilter);
};

}}
