#include"BaseLib/Event.h"

namespace BaseLib { namespace Concurrent
{

Event::Event(EventData::Ptr eventData, Templates::Any eventId)
    : Function(eventId)
    , source_()
    , eventData_(eventData)
    , stackTrace_("")
{ }

Event::~Event()
{ }

Event::Ptr Event::GetPtr()
{
    return shared_from_this();
}

// --------------------------
// wait for event delivery. Do I need an EventResult?
// --------------------------

bool Event::WaitFor(int64 msecs) const
{
    Locker locker(this);
    return waitForReceived<Status::EventStatus>(&status_, msecs);
}

void Event::AddResult(Templates::Any val)
{
    Locker lock(this);
    results_.push_back(val);
}

// --------------------------
// For filtering
// --------------------------

void Event::Accept()
{
    Locker locker(this);
    status_.Accept();

    this->wakeAll();
}

void Event::Ignore()
{
    Locker locker(this);
    status_.Ignore();

    this->wakeAll();
}

void Event::Filter()
{
    Locker locker(this);
    status_.Filter();

    this->wakeAll();
}

void Event::Reject()
{
    Locker locker(this);
    status_.Reject();

    this->wakeAll();
}

bool Event::IsAccepted() const
{
    return status_.IsAccepted();
}

bool Event::IsReceived() const
{
    return status_.IsReceived();
}

bool Event::IsApplicationEvent() const
{
    return true;
}

// --------------------------
// Status and identification
// --------------------------

Templates::Any Event::EventId() const
{
    return this->Id();
}

const Status::EventStatus &Event::Status() const
{
    return status_;
}

EventData::Ptr Event::Data() const
{
    return eventData_;
}

BaseLib::GeneralException Event::Exception() const
{
    return stackTrace_;
}

}}

