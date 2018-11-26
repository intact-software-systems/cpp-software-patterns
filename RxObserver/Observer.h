#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include "RxObserver/SubjectStrategy.h"
#include "RxObserver/Subscription.h"
#include "RxObserver/SubjectPolicy.h"
#include "RxObserver/SubjectId.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent { namespace details
{

// ----------------------------------------------------
// ObserverState
// ----------------------------------------------------

class DLL_STATE ObserverState
{
public:
    ObserverState(Templates::Any delegate = Templates::Any::Empty(),
                  Templates::Any event = Templates::Any::Empty(),
                  FlowControllerPolicy policy = FlowControllerPolicy::Default());

    virtual ~ObserverState();

    Status::ObserverStatus&         Status();
    const Status::ObserverStatus&   Status() const;

    FilterList& Filters();
    const FilterList& Filters() const;

    Concurrent::FlowController<ObserverEvent::Ptr>::Ptr Controller() const;

    Templates::Any Delegate() const;
    Templates::Any EventId() const;

    bool IsInitialized() const;
    void Initialize(Templates::Action0<bool>::Ptr action);

    void SetSubject(SubjectPtr subject);

    SubjectPtr GetSubject() const;

private:
    Concurrent::FlowController<ObserverEvent::Ptr>::Ptr flowController_;

    Status::ObserverStatus  status_;
    FilterList              filters_;
    Templates::Any          delegate_;
    Templates::Any          event_;
    SubjectWeakPtr          subject_;
};

// ----------------------------------------------------
// Observer a sort of "future" subscriber that is "Triggered" or executed with new arguments from publisher/subject.
// scheduled to run periodically based on interval policy
// triggered by subject to notify observer
// One flow controller for each observer because the interval policies may differ between observers
// Observer must be able to pull events through reactor
// ----------------------------------------------------

class DLL_STATE Observer
        : public Concurrent::ObserverAction
        , public Templates::LockableType<Observer>
        , public Templates::ContextObject<ObserverPolicy, ObserverState>
        , public ENABLE_SHARED_FROM_THIS(Observer)
{
public:
    template <typename Class, typename FunctionPointer>
    Observer(Class delegate, FunctionPointer func, ObserverPolicy policy = ObserverPolicy::Default())
        : Templates::ContextObject<ObserverPolicy, ObserverState>
          (
              policy,
              ObserverState(Templates::Any(delegate), Templates::Any(func), policy.FlowPolicy())
          )
    {}

    template <typename Class>
    Observer(Class delegate, Templates::Any func, ObserverPolicy policy = ObserverPolicy::Default())
        : Templates::ContextObject<ObserverPolicy, ObserverState>
          (
              policy,
              ObserverState(Templates::Any(delegate), func, policy.FlowPolicy())
          )
    {}

    virtual ~Observer();

    CLASS_TRAITS(Observer)

    Observer::Ptr GetPtr();

    friend class Templates::DoubleCheckedLocking;

    // ---------------------------------------------------------
    // Reactor methods
    // ---------------------------------------------------------

    virtual void OnNext(ObserverEvent::Ptr event);
    virtual void OnComplete();
    virtual void OnError(BaseLib::Exception exception);

    // ---------------------------------------------------------
    // Support pull model interaction
    // ---------------------------------------------------------

    virtual ObserverEvents History() const;

    // ---------------------------------------------------------
    // Action interface
    // ---------------------------------------------------------

    virtual bool Invoke();
    virtual bool Cancel();
    virtual bool IsDone() const;
    virtual bool IsSuccess() const;

    // ---------------------------------------------------------
    // observer interface
    // ---------------------------------------------------------

    virtual const Status::ObserverStatus &StatusConst() const;
    virtual Status::ObserverStatus &Status();

    virtual bool IsBlocked() const;
    virtual bool Unblock();
    virtual bool Block();

    virtual bool IsSubscribed() const;
    virtual bool Unsubscribe();

    virtual DelegateIdentification  Delegate() const;
    virtual EventIdentification     EventId() const;

    virtual ObserverPolicy Policy() const;

    virtual void SetSubject(SubjectPtr subject);

    // ---------------------------------------------------------
    // Implement by specialized observer
    // ---------------------------------------------------------

    virtual void Submit(ObserverEvent::Ptr event) = 0;
    virtual bool Filter(ObserverEvent::Ptr event) = 0;

protected:

    template <typename FilterType>
    bool addFilter(FilterCriterion::Ptr filter)
    {
        typename FilterType::Ptr filtertype = FilterCriterionFactory::To<FilterType>(filter);
        if(!filtertype) return false;

        this->state().Filters().push_back(filter->GetPtr());
        return true;
    }

private:

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    void Initialize();
    bool IsInitialized() const;
};


// ----------------------------------------------------
// Observer0 represents one event type, aka. function pointer
// ----------------------------------------------------

template <typename FunctionPointer, typename Return>
class Observer0 : public details::Observer
{
public:
    template <typename Class>
    Observer0(Class delegate, FunctionPointer func, ObserverPolicy policy = ObserverPolicy::Default())
        : details::Observer(delegate, func, policy)
        , eventFunction_(func)
    {}
    virtual ~Observer0()
    {}

    CLASS_TRAITS(Observer0)

    virtual void Submit(ObserverEvent::Ptr event)
    {
        Return val = eventFunction_();
        event->SetResult(Templates::Any(val));
    }

    virtual bool Filter(ObserverEvent::Ptr event)
    {
        return Strategy::FilterEventOnContent<Event0, FilterCriterion0>::Instance().Perform(event->GetPtr(), this->state().Filters());
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        return addFilter<FilterCriterion0>(filter);
    }

private:
    FunctionPointer eventFunction_;
};

// ----------------------------------------------------
// Observer1 represents one event type, aka. function pointer
// ----------------------------------------------------

template <typename FunctionPointer, typename Return, typename Arg1>
class Observer1 : public details::Observer
{
public:
    template <typename Class>
    Observer1(Class delegate, FunctionPointer func, ObserverPolicy policy = ObserverPolicy::Default())
        : details::Observer(delegate, func, policy)
        , eventFunction_(func)
    {}
    virtual ~Observer1()
    {}

    CLASS_TRAITS(Observer1)

    virtual void Submit(ObserverEvent::Ptr event)
    {
        typename Event1<Arg1>::Ptr eventData = std::dynamic_pointer_cast< Event1<Arg1> > (event->Data());

        if(eventData)
        {
            Return val = eventFunction_(eventData->Arg1());
            event->SetResult(Templates::Any(val));
        }
        else
        {
            IWARNING() << "Could not cast event data";
        }
    }

    virtual bool Filter(ObserverEvent::Ptr event)
    {
        return Strategy::FilterEventOnContent<Event1<Arg1>, FilterCriterion1<Arg1>>::Instance().Perform(event->GetPtr(), this->state().Filters());
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        return addFilter<FilterCriterion1<Arg1>>(filter);
    }

private:
    FunctionPointer eventFunction_;
};

// ----------------------------------------------------
// Observer2 represents one event type, aka. function pointer
// ----------------------------------------------------

template <typename FunctionPointer, typename Return, typename Arg1, typename Arg2>
class Observer2 : public details::Observer
{
public:
    template <typename Class>
    Observer2(Class delegate, FunctionPointer func, ObserverPolicy policy = ObserverPolicy::Default())
        : details::Observer(delegate, func, policy)
        , eventFunction_(func)
    {}
    virtual ~Observer2()
    {}

    CLASS_TRAITS(Observer2)

    virtual void Submit(ObserverEvent::Ptr event)
    {
        typename Event2<Arg1, Arg2>::Ptr eventData = std::dynamic_pointer_cast< Event2<Arg1, Arg2> > (event->Data());

        if(eventData)
        {
            Return val = eventFunction_(eventData->Arg1(), eventData->Arg2());
            event->SetResult(Templates::Any(val));
        }
        else
        {
            IWARNING() << "Could not cast event data";
        }
    }

    virtual bool Filter(ObserverEvent::Ptr event)
    {
        return Strategy::FilterEventOnContent<Event2<Arg1, Arg2>, FilterCriterion2<Arg1, Arg2>>::Instance().Perform(event->GetPtr(), this->state().Filters());
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        return addFilter<FilterCriterion2<Arg1, Arg2>>(filter);
    }

private:
    FunctionPointer eventFunction_;
};

}}}
