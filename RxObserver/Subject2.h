#pragma once

#include"RxObserver/Subject.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// class Subject2
// TODO: use (delegate, function) to identify one observers instance and function
// I need specialized subject with (Return, Arg1) to filter on content
// Use subjects to group and distribute events to correct subject
// Note: Subscribe and Next functions are not really necessary to be available, they are basically specialized "proxy/factory" functions and can be done by specialized subject.
// Add event id as function identifier in constructor then it is not necessary in other functions
// TODO: Remove Return values for all subjects, since it is a Pub/Sub implementation and no return values are necessary.
// ----------------------------------------------------

template <typename Return, typename Arg1, typename Arg2>
class Subject2 : public details::SubjectBase
{
public:
    template <typename EventFunction>
    Subject2(EventFunction id, SubjectPolicy policy = SubjectPolicy::Default())
        : details::SubjectBase(SubjectId(id), policy)
    {}
    virtual ~Subject2()
    {}

    CLASS_TRAITS(Subject2)

    // ---------------------------------------------------------
    // Subscribe functions
    // ---------------------------------------------------------

    template <typename Delegate>
    typename Subscription::Ptr SubscribeType(Delegate delegate, ObserverPolicy policy = ObserverPolicy::Default())
    {
        typedef Return (Delegate::*EventFunction)(Arg1, Arg2);
        typedef Templates::Function2<Delegate, Return, Arg1, Arg2>      FunctionType;
        typedef details::Observer2<FunctionType, Return, Arg1, Arg2>    ObserverType;

        SubjectId id = this->state().Id();
        Templates::Any any = id.Id();

        // Use default callback through "SubjectId"
        EventFunction *member = Templates::AnyCast<EventFunction>(&any);
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
    Event::Ptr Next(Arg1 arg1, Arg2 arg2, Source *source)
    {
        return details::SubjectBase::next<Source>(new Event2<Arg1, Arg2>(this->state().Id(), arg1, arg2), source);
    }

    // ---------------------------------------------------------
    // Implement by specific subject
    // ---------------------------------------------------------

    virtual bool Filter(Event::Ptr event)
    {
        return Strategy::FilterEventOnContent<Event2<Arg1, Arg2>, FilterCriterion2<Arg1, Arg2>>::Instance().Perform(event, this->state().Filters());
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        return addFilter<FilterCriterion2<Arg1, Arg2>>(filter);
    }
};

}}
