#pragma once

#include"RxObserver/Subject.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// class Subject0
// TODO: use (delegate, function) to identify one observers instance and function
// I need specialized subject with (Return, Arg1) to filter on content
// Use subjects to group and distribute events to correct subject
// Note: Subscribe and Next functions are not really necessary to be available, they are basically specialized "proxy/factory" functions and can be done by specialized subject.
// Add event id as function identifier in constructor then it is not necessary in other functions
// TODO: Remove Return values for all subjects, since it is a Pub/Sub implementation and no return values are necessary.
// ----------------------------------------------------

template <typename Return>
class Subject0 : public details::SubjectBase
{
public:
    template <typename EventFunction>
    Subject0(EventFunction id, SubjectPolicy policy = SubjectPolicy::Default())
        : SubjectBase(SubjectId(id), policy)
    {}
    virtual ~Subject0()
    {}

    CLASS_TRAITS(Subject0)

    // ---------------------------------------------------------
    // Subscribe functions
    // ---------------------------------------------------------

    template <typename Delegate>
    typename Subscription::Ptr SubscribeType(Delegate delegate, ObserverPolicy policy = ObserverPolicy::Default())
    {
        typedef Return (Delegate::*EventFunction)();
        typedef Templates::Function0<Delegate, Return>      FunctionType;
        typedef details::Observer0<FunctionType, Return>    ObserverType;

        Templates::Any any = this->state().Id().Id();

        // Use default callback through "SubjectId"
        EventFunction *member = Templates::AnyCast<EventFunction>( &any );
        if(member == nullptr)
        {
            ICRITICAL() << "Could not cast to function!";
            return Subscription::Ptr();
        }

        return details::SubjectBase::Subscribe(new ObserverType(delegate, FunctionType(delegate, member), policy));
    }

    // ---------------------------------------------------------
    // Next event functions
    // ---------------------------------------------------------

    template <typename Source>
    Event::Ptr Next(Source *source)
    {
        return details::SubjectBase::next<Source>(new Event0(this->state().Id()), source);
    }

    // ---------------------------------------------------------
    // Implement by specific subject
    // ---------------------------------------------------------

    virtual bool Filter(Event::Ptr event)
    {
        return Strategy::FilterEventOnContent<Event0, FilterCriterion0>::Instance().Perform(event, this->state().Filters());
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        return addFilter<FilterCriterion0>(filter);
    }
};

}}
