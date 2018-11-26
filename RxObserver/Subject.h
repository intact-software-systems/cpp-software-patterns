#pragma once

#include"RxObserver/SubjectBase.h"
#include"RxObserver/SubjectId.h"

#include"RxObserver/Observers.h"
#include"RxObserver/Observer.h"
#include"RxObserver/Subscription.h"
#include"RxObserver/SubjectPolicy.h"
#include"RxObserver/SubjectStrategy.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent { namespace details
{

// ----------------------------------------------------
// SubjectState
// ----------------------------------------------------

class DLL_STATE SubjectState
{
public:
    SubjectState();
    SubjectState(SubjectId id, FlowControllerPolicy policy);
    virtual ~SubjectState();

    Concurrent::Observers::Ptr Observers() const;
    Concurrent::FlowController<Event::Ptr>::Ptr Controller() const;

    Status::SubjectStatus& Status();
    const Status::SubjectStatus& Status() const;

    FilterList& Filters();
    const FilterList& Filters() const;

    const SubjectId& Id() const;

    bool IsInitialized() const;
    void Initialize(Templates::Action0<bool>::Ptr action);

private:
    Concurrent::FlowController<Event::Ptr>::Ptr    flowController_;

    Concurrent::Observers::Ptr  observers_;
    Status::SubjectStatus       status_;
    FilterList                  filters_;
    SubjectId                   subjectId_;
};

/**
 * @brief The Signaller class, aka. Publisher, EventDispatcher, to be used by Subject.
 *
 * - support for replay of events
 * - support for scalable asynchronous dispatch of events.
 * - event meta data for monitoring: timestamp, etc
 *
 * TODO: Activate (start), Shutdown (stop)
 * TODO: When subject is activated or deactivated, notify the observers.
 */
class DLL_STATE SubjectBase
        : public Concurrent::Subject
        , public Templates::Action0<bool>
        , public Templates::LockableType<SubjectBase>
        , public Templates::ContextObject<SubjectPolicy, SubjectState>
        , public ENABLE_SHARED_FROM_THIS(SubjectBase)
{
public:
    SubjectBase(SubjectId id, SubjectPolicy policy = SubjectPolicy::Default());
    virtual ~SubjectBase();

    CLASS_TRAITS(SubjectBase)

    SubjectBase::Ptr GetPtr();

    friend class Templates::DoubleCheckedLocking;

    // ---------------------------------------------------------
    // Subject interface
    // ---------------------------------------------------------

    virtual Subscription::Ptr Subscribe(std::shared_ptr<ObserverAction> observer);
    virtual bool Unsubscribe(std::shared_ptr<ObserverAction> observer);

    virtual bool Block();
    virtual bool Unblock();

    virtual bool IsBlocked() const;
    virtual bool IsActive() const;
    virtual bool IsEmpty() const;

    virtual int Size() const;

    virtual bool UnsubscribeAll();

    virtual SubjectPolicy Policy() const;

    virtual const Status::SubjectStatus &StatusConst() const;

    virtual SubjectId Id() const;

    // ---------------------------------------------------------
    // Reactor methods
    // ---------------------------------------------------------

    virtual Event::Ptr Next(Event::Ptr event);
    virtual bool       Complete();
    virtual bool       Error(BaseLib::Exception exception);

    // ---------------------------------------------------------
    // Support pull model interaction
    // ---------------------------------------------------------

    virtual Events History() const;

    virtual bool Submit(Event::Ptr event);
    virtual bool Submit(Events events);

    // ---------------------------------------------------------
    // template function
    // ---------------------------------------------------------

    template <typename Delegate>
    bool UnsubscribeType(Delegate observer)
    {
        return this->state().Observers()->UnsubscribeType<Delegate>(observer);
    }

    template <typename Delegate>
    bool IsSubscribedType(Delegate delegate) const
    {
        return this->state().Observers()->IsSubscribedType<Delegate>(delegate);
    }

    // ---------------------------------------------------------
    // Action0 interface
    // TODO: Split subject base functionality and this action, which is basically a rector
    // ---------------------------------------------------------

    virtual bool Invoke();
    virtual bool Cancel();
    virtual bool IsDone() const;
    virtual bool IsSuccess() const;

protected:

    template <typename FilterType>
    bool addFilter(FilterCriterion::Ptr filter)
    {
        typename FilterType::Ptr filtertype = FilterCriterionFactory::To<FilterType>(filter);
        if(!filtertype) return false;

        this->state().Filters().push_back(filter->GetPtr());
        return true;
    }

    template <typename Delegate>
    Event::Ptr next(EventData *eventData, Delegate *source)
    {
        return Next(Event::Ptr(new Event(EventData::Ptr(eventData), this->state().Id(), source)));
    }

private:
    // ---------------------------------------------------------
    // Initialize methods called when invoking Next
    // ---------------------------------------------------------

    void Initialize();
    bool IsInitialized() const;

    // ---------------------------------------------------------
    // private implementation
    // ---------------------------------------------------------

    bool replayHistoryToObserver(std::shared_ptr<ObserverAction> observer, Events events);
};

}}}
