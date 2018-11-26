#include"RxObserver/Subject.h"
#include"RxObserver/FlowControlStrategy.h"

namespace BaseLib { namespace Concurrent { namespace details
{

// ----------------------------------------------------
// SubjectState
// ----------------------------------------------------

SubjectState::SubjectState()
    : flowController_(new FlowControllerAction<Event::Ptr>(FlowControllerPolicy::Default()))
    , observers_(new Concurrent::Observers())
    , status_()
    , filters_()
    , subjectId_("default-id")
{}

SubjectState::SubjectState(SubjectId id, FlowControllerPolicy policy)
    : flowController_(new FlowControllerAction<Event::Ptr>(policy))
    , observers_(new Concurrent::Observers())
    , status_()
    , filters_()
    , subjectId_(id)
{ }

SubjectState::~SubjectState()
{ }

Observers::Ptr SubjectState::Observers() const
{
    return observers_;
}

Concurrent::FlowController<Event::Ptr>::Ptr SubjectState::Controller() const
{
    return flowController_;
}

Status::SubjectStatus &SubjectState::Status()
{
    return status_;
}

const Status::SubjectStatus &SubjectState::Status() const
{
    return status_;
}

FilterList &SubjectState::Filters()
{
    return filters_;
}

const FilterList &SubjectState::Filters() const
{
    return filters_;
}

const SubjectId &SubjectState::Id() const
{
    return subjectId_;
}

bool SubjectState::IsInitialized() const
{
    return status_.IsActive();
}

void SubjectState::Initialize(Templates::Action0<bool>::Ptr action)
{
    bool isAttached = flowController_->Set(action);
    if(isAttached)
        status_.Activate();
}

// ----------------------------------------------------
// SubjectBase
// ----------------------------------------------------

SubjectBase::SubjectBase(SubjectId id, SubjectPolicy policy)
    : Templates::ContextObject<SubjectPolicy, SubjectState>
      (
          policy,
          SubjectState(id, policy.FlowPolicy())
      )
{ }

SubjectBase::~SubjectBase()
{ }

SubjectBase::Ptr SubjectBase::GetPtr()
{
    return shared_from_this();
}

// ---------------------------------------------------------
// Subject interface
// ---------------------------------------------------------
Subscription::Ptr SubjectBase::Subscribe(std::shared_ptr<ObserverAction> observer)
{
    this->state().Observers()->Add(observer);

    if(this->config().Replay().IsOnSubscribe() || observer->Policy().Replay().IsOnSubscribe())
    {
        replayHistoryToObserver(observer, this->state().Controller()->History());
    }

    observer->SetSubject(this->GetPtr());
    return Subscription::Ptr( new Subscription(observer) );
}

bool SubjectBase::Unsubscribe(std::shared_ptr<ObserverAction> observer)
{
    return this->state().Observers()->Remove(observer);
}

bool SubjectBase::Block()
{
    return this->state().Status().Block();
}

bool SubjectBase::Unblock()
{
    return this->state().Status().Unblock();
}

bool SubjectBase::IsBlocked() const
{
    return this->state().Status().IsBlocked();
}

bool SubjectBase::IsActive() const
{
    return this->state().Status().IsActive();
}

bool SubjectBase::IsEmpty() const
{
    return this->state().Observers()->IsEmpty();
}

int SubjectBase::Size() const
{
    return this->state().Observers()->Size();
}

bool SubjectBase::UnsubscribeAll()
{
    return this->state().Observers()->Unsubscribe();
}

SubjectPolicy SubjectBase::Policy() const
{
    return this->config();
}

const Status::SubjectStatus &SubjectBase::StatusConst() const
{
    return this->state().Status();
}

SubjectId SubjectBase::Id() const
{
    return this->state().Id();
}

// ---------------------------------------------------------
// Reactor methods
// ---------------------------------------------------------

Event::Ptr SubjectBase::Next(Event::Ptr event)
{
    bool initialize = Templates::DoubleCheckedLocking::Initialize<SubjectBase>(this);
    if(!initialize) return Event::Ptr(); //EventNoOp::Create();

    // ----------------------------------------
    // Apply ontent filter on entrance
    // ----------------------------------------

    if(Filter(event))
    {
        event->Filter();
        OnFilteredOut(event);
    }
    else
    {
        bool isAdded = this->state().Controller()->Next(event);

        if(!isAdded)
        {
            event->Reject();
            OnFilteredOut(event);
        }
        else if(this->config().Invocation().IsSync())
        {
            event->WaitFor(this->config().Timeout().AsDuration().InMillis());
        }
    }

    return event;
}

bool SubjectBase::Complete()
{
    return this->state().Observers()->Complete() && this->state().Controller()->Complete();
}

bool SubjectBase::Error(Exception exception)
{
    this->Cancel();

    this->state().Observers()->Error(exception);
    return this->state().Controller()->Error(exception);
}

// ---------------------------------------------------------
// Support pull model interaction
// ---------------------------------------------------------

Events SubjectBase::History() const
{
    return this->state().Controller()->History();
}

bool SubjectBase::Submit(Event::Ptr event)
{
    Events events;
    events.push_back(event);

    return Submit(events);
}

bool SubjectBase::Submit(Events events)
{
    if(this->IsBlocked()) return false;
    if(this->state().Observers()->IsEmpty()) {
        IWARNING() << "No observers!";
        return false;
    }

    // -----------------------------
    // Apply timebased filter
    // -----------------------------
    Events filteredEvents = Strategy::FilterEventsOnTime::Instance().Perform(events, this->config().MinSeparation());

    if(!filteredEvents.empty())
    {
        for(Concurrent::Event::Ptr event : filteredEvents)
            event->Filter();

        OnFilteredOut(filteredEvents);
    }

    //EventResults results = this->state().Observers()->Next(events);

    IINFO() << "Submitting " << events.size() << " events to " << this->state().Observers()->Size() << " observers";

    // -----------------------------
    // Push to observers
    // -----------------------------
    EventResults results = Strategy::PlayEventsToObservers::Instance().Perform(
                this->state().Observers(),
                events,
                this->config().Computation(),
                this->config().Timeout());

    // TODO: Anything to do with results? Statistics?

    return true;
}

// ---------------------------------------------------------
// Invoke is called from flow controller
// ---------------------------------------------------------

bool SubjectBase::Invoke()
{
    if(this->IsBlocked()) return false;
    if(this->state().Observers()->IsEmpty()) {
        IWARNING() << "No observers!";
        return false;
    }

    Events events = this->state().Controller()->Pull();
    if(events.empty()) return false;

    return Submit(events);
}

bool SubjectBase::Cancel()
{
    return this->state().Status().Deactivate();
}

bool SubjectBase::IsDone() const
{
    return !this->state().Status().IsActive();
}

bool SubjectBase::IsSuccess() const
{
    return true;
}

// ---------------------------------------------------------
// Initialize methods
// ---------------------------------------------------------

void SubjectBase::Initialize()
{
    this->state().Initialize(this->GetPtr());
}

bool SubjectBase::IsInitialized() const
{
    return this->state().IsInitialized();
}

// ---------------------------------------------------------
// private implementation
// ---------------------------------------------------------

bool SubjectBase::replayHistoryToObserver(std::shared_ptr<ObserverAction> observer, Events events)
{
    if(events.empty()) return false;

    Concurrent::Observers::Ptr observerGroup(new Concurrent::Observers());
    observerGroup->Add(observer);

    FutureTask<EventResults>::Ptr replayTask = Strategy::PlayEventsToObservers::Create(
                Strategy::PlayEventsToObservers::InstancePtr(),
                observerGroup,
                events,
                this->config().Computation(),
                this->config().Timeout());

    ThreadPool::Ptr pool = Concurrent::ThreadPoolFactory::Instance().GetDefault();

    //ScheduledFutureTask<EventResults> scheduledFuture =
    pool->Schedule<EventResults>(replayTask, TaskPolicy::RunOnceImmediately());

    //scheduledFuture.WaitFor(this->config().policy().Timeout().TimeSinceEpoch());
    return !events.empty();
}

}}}
