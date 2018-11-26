#include "Observer.h"
#include"RxObserver/SubjectBase.h"

namespace BaseLib { namespace Concurrent { namespace details
{

// ----------------------------------------------------
// ObserverState
// ----------------------------------------------------

ObserverState::ObserverState(Templates::Any delegate, Templates::Any event, FlowControllerPolicy policy)
    : flowController_(new FlowControllerAction<ObserverEvent::Ptr>(policy))
    , status_()
    , filters_()
    , delegate_(delegate)
    , event_(event)
{ }

ObserverState::~ObserverState()
{ }

Status::ObserverStatus &ObserverState::Status()
{
    return status_;
}

const Status::ObserverStatus &ObserverState::Status() const
{
    return status_;
}

FilterList &ObserverState::Filters()
{
    return filters_;
}

const FilterList &ObserverState::Filters() const
{
    return filters_;
}

Concurrent::FlowController<ObserverEvent::Ptr>::Ptr ObserverState::Controller() const
{
    return flowController_;
}

Templates::Any ObserverState::Delegate() const
{
    return delegate_;
}

Templates::Any ObserverState::EventId() const
{
    return event_;
}

bool ObserverState::IsInitialized() const
{
    return status_.IsSubscribed();
}

void ObserverState::Initialize(Templates::Action0<bool>::Ptr action)
{
    bool isAttached = flowController_->Set(action);
    if(isAttached)
        status_.Subscribe();
}

void ObserverState::SetSubject(SubjectPtr subject)
{
    subject_ = subject;
}

SubjectPtr ObserverState::GetSubject() const
{
    return subject_.lock();
}

// ----------------------------------------------------
// Observer
// ----------------------------------------------------

Observer::~Observer()
{}

Observer::Ptr Observer::GetPtr()
{
    return shared_from_this();
}

void Observer::OnNext(ObserverEvent::Ptr event)
{
    bool initialize = Templates::DoubleCheckedLocking::Initialize<Observer>(this);
    if(!initialize) {
        IFATAL() << "Failed to initialize";
        return;
    }

    // -------------------------------------------
    // flow controller may reject if queue is full
    // TODO: With replay policy OnComplete, the flow-controller cannot be enabled until OnComplete is called.
    // -------------------------------------------
    bool isAdded = this->state().Controller()->Next(event);

    if(!isAdded)
    {
        // TODO: Use return value to deduce event ignore/filter/reject, etc.
        event->Reject();
        OnFilteredOut(event);
    }
}

void Observer::OnComplete()
{
    // TODO: Basically the flow-controller cannot be enabled until subject is done
    // if(this->config().policy().Replay().IsOnUnsubscribe())
    // TODO: What to replay? History or entire queue?

    this->state().Status().Unsubscribe();
    this->state().Controller()->Complete();
}

void Observer::OnError(BaseLib::Exception exception)
{
    this->state().Status().Unsubscribe();
    this->state().Controller()->Error(exception);
}

// ---------------------------------------------------------
// Support pull model interaction
// ---------------------------------------------------------

ObserverEvents Observer::History() const
{
    return this->state().Controller()->History();
}

// ---------------------------------------------------------
// Action interface
// ---------------------------------------------------------

bool Observer::Invoke()
{
    if(!IsSubscribed()) return false;

    ObserverEvents events = this->state().Controller()->Pull();
    if(events.empty()) return false;

    // -----------------------------
    // Apply timebased filter
    // -----------------------------
    ObserverEvents filteredEvents = Strategy::FilterObserverEventsOnTime::Instance().Perform(events, this->config().MinSeparation());

    if(!filteredEvents.empty())
    {
        for(Concurrent::ObserverEvent::Ptr event : filteredEvents)
            event->Filter();

        OnFilteredOut(filteredEvents);
    }

    // --------------------------------------
    // Submit event's data to observer if not blocked
    // TODO: With replay policy OnUnsubscribe, submit last n events
    // TODO: What to replay? History or entire queue?
    // --------------------------------------
    for(Concurrent::ObserverEvent::Ptr event : events)
    {
        ASSERT(event != nullptr);

        if(Filter(event))
        {
            event->Filter();

            OnFilteredOut(event);
        }
        else if(!IsBlocked())
        {
            Submit(event);

            event->Accept();
        }
    }

    return true;
}

bool Observer::Cancel()
{
    return this->state().Status().Unsubscribe();
}

bool Observer::IsDone() const
{
    return !this->state().Status().IsSubscribed();
}

bool Observer::IsSuccess() const
{
    return !this->state().Status().IsSubscribed();
}

// ---------------------------------------------------------
// observer interface
// ---------------------------------------------------------

const Status::ObserverStatus &Observer::StatusConst() const
{
    return this->state().Status();
}

Status::ObserverStatus &Observer::Status()
{
    return this->state().Status();
}

bool Observer::IsBlocked() const
{
    return this->state().Status().IsBlocked();
}

bool Observer::Unblock()
{
    return this->state().Status().Unblock();
}

bool Observer::Block()
{
    return this->state().Status().Block();
}

bool Observer::IsSubscribed() const
{
    return this->state().Status().IsSubscribed();
}

bool Observer::Unsubscribe()
{
    // TODO: Support Subscribe again, what to do with subscription?
    // TODO: What to replay? History or entire queue?
    //if(this->config().policy().Replay().IsOnUnsubscribe())
    //replayHistoryToObserver(observer, this->state().Controller()->History());
    Subject::Ptr subject = this->state().GetSubject();
    if(subject != nullptr)
    {
        subject->Unsubscribe(this->GetPtr());
    }

    return this->state().Status().Unsubscribe();
}

DelegateIdentification Observer::Delegate() const
{
    return this->state().Delegate();
}

EventIdentification Observer::EventId() const
{
    return std::make_pair(this->state().Delegate(), this->state().EventId());
}

ObserverPolicy Observer::Policy() const
{
    return this->config();
}

void Observer::SetSubject(SubjectPtr subject)
{
    this->state().SetSubject(subject);
}

// ---------------------------------------------------------
// Initialize methods
// ---------------------------------------------------------

void Observer::Initialize()
{
    this->state().Initialize(this->GetPtr());
}

bool Observer::IsInitialized() const
{
    return this->state().IsInitialized();
}

}}}
