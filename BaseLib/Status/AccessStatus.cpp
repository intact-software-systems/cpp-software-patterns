#include "BaseLib/Status/AccessStatus.h"

namespace BaseLib { namespace Status {

AccessStatus::AccessStatus()
    : kind_(AccessStatusKind::WRITE)
    , time_()
    , count_()
{ }

AccessStatus::~AccessStatus()
{ }

// ------------------------------------------------------
// Simple getters
// ------------------------------------------------------

AccessCount &AccessStatus::Count()
{
    return count_;
}

AccessTime &AccessStatus::Time()
{
    return time_;
}

const AccessCount &AccessStatus::Count() const
{
    return count_;
}

const AccessTime &AccessStatus::Time() const
{
    return time_;
}

AccessStatusKind::Type AccessStatus::State() const
{
    return kind_;
}

// ------------------------------------------------------
// Using interfaces: Called when execution starts and stops
// ------------------------------------------------------

void AccessStatus::Modify()
{
    count_.Modify();
    time_.Modify();
    kind_ = AccessStatusKind::MODIFY;
}

void AccessStatus::Read()
{
    count_.Read();
    time_.Read();
    kind_ = AccessStatusKind::READ;
}

void AccessStatus::NotModify()
{
    count_.NotModify();
    time_.NotModify();
    kind_ = AccessStatusKind::NOT_MODIFY;
}

void AccessStatus::Delete()
{
    count_.Modify();
    kind_ = AccessStatusKind::DELETED;
}

void AccessStatus::RenewLease() {
    time_.Start();
}

// ------------------------------------------------------
// Using interfaces: Check status
// ------------------------------------------------------

bool AccessStatus::IsRead() const
{
    return kind_ == AccessStatusKind::READ;
}

bool AccessStatus::IsModified() const
{
    return kind_ == AccessStatusKind::MODIFY;
}

bool AccessStatus::IsNotModified() const
{
    return kind_ == AccessStatusKind::NOT_MODIFY;
}

bool AccessStatus::IsDeleted() const
{
    return kind_ == AccessStatusKind::DELETED;
}

}}
