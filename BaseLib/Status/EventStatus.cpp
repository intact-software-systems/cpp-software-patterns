#include "BaseLib/Status/EventStatus.h"

namespace BaseLib { namespace Status
{
EventStatus::EventStatus()
    : kind_(Kind::NO)
    , prev_(Kind::NO)
    , accepted_(Kind::ACCEPTED)
    , ignored_(Kind::IGNORED)
    , filtered_(Kind::FILTERED)
    , rejected_(Kind::REJECTED)
    , blocked_(Kind::BLOCKED)
    , queued_(Kind::QUEUED)
{ }

EventStatus::~EventStatus()
{ }

EventStatus::Kind EventStatus::State() const
{
    Locker locker(this);
    return kind_;
}

void EventStatus::Accept()
{
    accepted_.Next(1);

    Locker locker(this);

    prev_ = kind_;
    kind_ = Kind::ACCEPTED;
}

void EventStatus::Ignore()
{
    ignored_.Next(1);

    Locker locker(this);

    prev_ = kind_;
    kind_ = Kind::IGNORED;
}

void EventStatus::Filter()
{
    filtered_.Next(1);

    Locker locker(this);

    prev_ = kind_;
    kind_ = Kind::FILTERED;
}

void EventStatus::Reject()
{
    rejected_.Next(1);

    Locker locker(this);

    prev_ = kind_;
    kind_ = Kind::REJECTED;
}

void EventStatus::Block()
{
    blocked_.Next(1);

    Locker locker(this);

    prev_ = kind_;
    kind_ = Kind::BLOCKED;
}

void EventStatus::Unblock()
{
    Locker locker(this);
    Kind temp = kind_;

    kind_ = prev_;
    prev_ = temp;
}

void EventStatus::Queue()
{
    queued_.Next(1);

    Locker locker(this);

    prev_ = kind_;
    kind_ = Kind::QUEUED;
}

bool EventStatus::IsAccepted() const
{
    Locker locker(this);
    return kind_ == Kind::ACCEPTED;
}

bool EventStatus::IsIgnored() const
{
    Locker locker(this);
    return kind_ == Kind::IGNORED;
}

bool EventStatus::IsFiltered() const
{
    Locker locker(this);
    return kind_ == Kind::FILTERED;
}

bool EventStatus::IsRejected() const
{
    Locker locker(this);
    return kind_ == Kind::REJECTED;
}

bool EventStatus::IsReceived() const
{
    return IsAccepted() || IsIgnored() || IsFiltered() || IsBlocked() || IsRejected();
}

bool EventStatus::IsBlocked() const
{
    Locker locker(this);
    return kind_ == Kind::BLOCKED;
}

const StateStatus<EventStatus::Kind> &EventStatus::Accepted() const
{
    return accepted_;
}

const StateStatus<EventStatus::Kind> &EventStatus::Ignored() const
{
    return ignored_;
}

const StateStatus<EventStatus::Kind> &EventStatus::Filtered() const
{
    return filtered_;
}

const StateStatus<EventStatus::Kind> &EventStatus::Rejected() const
{
    return rejected_;
}

const StateStatus<EventStatus::Kind> &EventStatus::Blocked() const
{
    return blocked_;
}

const StateStatus<EventStatus::Kind> &EventStatus::Queued() const
{
    return queued_;
}

}}
