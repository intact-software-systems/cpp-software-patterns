#include"RxObserver/EventResult.h"
#include"RxObserver/Observers.h"

namespace BaseLib { namespace Concurrent
{

EventResult::EventResult(Event::Ptr event, ObserversPtr observerGroup)
    : Templates::Function(event->EventId())
    , event_(event)
    , observerGroup_(observerGroup)
{}

EventResult::~EventResult()
{}

// -----------------------------------------------
// Mark event as received by observer
// TODO: Should I use an observer key instead?
// Review observer key, based on delegate and function?
// TODO: Should it be accepted even if filtered and bloked?
// TODO: Consider using OnNext()
// -----------------------------------------------
void EventResult::OnNext(std::shared_ptr<ObserverAction> received)
{
    bool isRemoved = observerGroup_->Remove(received);
    ASSERT(isRemoved);

    if(observerGroup_->IsEmpty() || !observerGroup_->IsSubscribed())
    {
        event_->Accept();
    }
}

void EventResult::OnComplete()
{
    // TODO: How does this apply?
    ICRITICAL() << "On complete: How does this apply!";
}

void EventResult::OnError(BaseLib::Exception exception)
{
    ICRITICAL() << "Unhandled: " << exception;
}

// -----------------------------------------------
// Interface EventResultBase
// -----------------------------------------------

bool EventResult::WaitFor(int64 msecs) const
{
    return event_->WaitFor(msecs);
}

bool EventResult::IsAccepted() const
{
    return event_->IsAccepted();
}

bool EventResult::IsReceived() const
{
    if(observerGroup_->IsEmpty() || !observerGroup_->IsSubscribed())
        IWARNING() << "Observer group is empty!";

    return event_->IsReceived();
}

const Event& EventResult::GetEvent() const
{
    return event_.operator *();
}

void EventResult::AddResult(Templates::Any value)
{
    event_->AddResult(value);
}

}}
