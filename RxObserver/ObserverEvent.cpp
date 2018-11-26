#include"RxObserver/ObserverEvent.h"
#include"RxObserver/ObserverBase.h"
#include"RxObserver/EventResult.h"

namespace BaseLib { namespace Concurrent
{

ObserverEvent::ObserverEvent(EventResultPtr event, ObserverActionPtr observer)
    : Concurrent::Event(event->GetEvent().Data(), event->GetEvent().EventId())
    , event_(event)
    , observer_(observer)
{ }

ObserverEvent::~ObserverEvent()
{ }

void ObserverEvent::Accept()
{
    Event::Accept();
    event_->OnNext(this->observer_);
}

void ObserverEvent::Ignore()
{
    Event::Ignore();
    event_->OnNext(this->observer_);
}

void ObserverEvent::Filter()
{
    Event::Filter();
    event_->OnNext(this->observer_);
}

void ObserverEvent::Reject()
{
    Event::Reject();
    event_->OnNext(this->observer_);
}

void ObserverEvent::SetResult(Templates::Any val)
{
    value_ = val;
    event_->AddResult(val);
}

}}
