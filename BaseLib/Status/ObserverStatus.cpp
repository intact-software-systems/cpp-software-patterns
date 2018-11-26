#include "BaseLib/Status/ObserverStatus.h"

namespace BaseLib { namespace Status
{

ObserverStatus::ObserverStatus()
    : current_(Kind::NO)
    , prev_(Kind::NO)
    , subscribed_(Kind::SUBSCRIBED)
    , blocked_(Kind::BLOCKED)
    , unsubscribed_(Kind::UNSUBSCRIBED)
{ }

ObserverStatus::~ObserverStatus()
{ }

ObserverStatus::Kind ObserverStatus::State() const
{
    Locker locker(this);
    return current_;
}

bool ObserverStatus::Block()
{
    blocked_.Next(1);

    Locker locker(this);

    prev_ = current_;
    current_ = Kind::BLOCKED;
    return true;
}

bool ObserverStatus::Unblock()
{
    Locker locker(this);

    Kind temp = current_;

    current_ = prev_;
    prev_ = temp;
    return true;
}

bool ObserverStatus::Subscribe()
{
    subscribed_.Next(1);

    Locker locker(this);

    prev_ = current_;
    current_ = Kind::SUBSCRIBED;
    return true;
}

bool ObserverStatus::Unsubscribe()
{
    unsubscribed_.Next(1);

    Locker locker(this);

    prev_ = current_;
    current_ = Kind::UNSUBSCRIBED;
    return true;
}

bool ObserverStatus::IsBlocked() const
{
    Locker locker(this);
    return current_ == Kind::BLOCKED;
}

bool ObserverStatus::IsSubscribed() const
{
    Locker locker(this);
    return current_ == Kind::SUBSCRIBED;
}

const StateStatus<ObserverStatus::Kind> &ObserverStatus::Subscribed() const
{
    return subscribed_;
}

const StateStatus<ObserverStatus::Kind> &ObserverStatus::Blocked() const
{
    return blocked_;
}

const StateStatus<ObserverStatus::Kind> &ObserverStatus::Unsubscribed() const
{
    return unsubscribed_;
}

}}
