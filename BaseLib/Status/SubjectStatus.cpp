#include "BaseLib/Status/SubjectStatus.h"

namespace BaseLib { namespace Status
{

SubjectStatus::SubjectStatus()
    : current_(Kind::NO)
    , prev_(Kind::NO)
    , active_(Kind::ACTIVE)
    , blocked_(Kind::BLOCKED)
    , inactive_(Kind::INACTIVE)
{ }

SubjectStatus::~SubjectStatus()
{ }

SubjectStatus::Kind SubjectStatus::State() const
{
    Locker locker(this);
    return current_;
}

bool SubjectStatus::Block()
{
    blocked_.Next(1);

    Locker locker(this);

    prev_ = current_;
    current_ = Kind::BLOCKED;
    return true;
}

bool SubjectStatus::Unblock()
{
    Locker locker(this);

    Kind temp = current_;

    current_ = prev_;
    prev_ = temp;
    return true;
}

bool SubjectStatus::Activate()
{
    active_.Next(1);

    Locker locker(this);

    prev_ = current_;
    current_ = Kind::ACTIVE;
    return true;
}

bool SubjectStatus::Deactivate()
{
    inactive_.Next(1);

    Locker locker(this);

    prev_ = current_;
    current_ = Kind::INACTIVE;
    return true;
}

bool SubjectStatus::IsBlocked() const
{
    Locker locker(this);
    return current_ == Kind::BLOCKED;
}

bool SubjectStatus::IsActive() const
{
    Locker locker(this);
    return current_ == Kind::ACTIVE;
}

const StateStatus<SubjectStatus::Kind> &SubjectStatus::Active() const
{
    return active_;
}

const StateStatus<SubjectStatus::Kind> &SubjectStatus::Blocked() const
{
    return blocked_;
}

const StateStatus<SubjectStatus::Kind> &SubjectStatus::Inactive() const
{
    return inactive_;
}

}}

