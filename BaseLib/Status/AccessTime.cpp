#include "BaseLib/Status/AccessTime.h"
#include "BaseLib/Duration.h"

namespace BaseLib { namespace Status {

AccessTime::AccessTime()
    : createdTime_()
    , startTime_(createdTime_)
    , modifiedTime_(createdTime_)
    , readTime_()
{ }

AccessTime::~AccessTime()
{ }

// ------------------------------------------------------
// Interfaces
// ------------------------------------------------------

void AccessTime::Modify()
{
    modifiedTime_ = Timestamp::Now();
}

void AccessTime::NotModify()
{
    // TODO: Currently no access time for not modified
}

void AccessTime::Read()
{
    readTime_ = Timestamp::Now();
}

void AccessTime::Start()
{
    startTime_ = Timestamp::Now();
}

// ------------------------------------------------------
// Access absolute timestamps
// ------------------------------------------------------

const Timestamp& AccessTime::CreatedAt() const
{
    return createdTime_;
}

const Timestamp& AccessTime::ModifiedAt() const
{
    return modifiedTime_;
}

const Timestamp& AccessTime::ReadAt() const
{
    return readTime_;
}

const Timestamp& AccessTime::WrittenAt() const
{
    return modifiedTime_;
}

const Timestamp& AccessTime::StartedAt() const
{
    return startTime_;
}

const Timestamp& AccessTime::LastAccessedAt() const
{
    return std::max<Timestamp>(readTime_, modifiedTime_);
}

// ------------------------------------------------------
// Compute time since a point in time
// ------------------------------------------------------

Duration AccessTime::TimeSinceCreated() const
{
    return createdTime_.SinceStamped();
}

Duration AccessTime::TimeSinceModified() const
{
    return modifiedTime_.SinceStamped();
}

Duration AccessTime::TimeSinceRead() const
{
    return readTime_.SinceStamped();
}

Duration AccessTime::TimeSinceStarted() const
{
    return startTime_.SinceStamped();
}

Duration AccessTime::TimeSinceAccessed() const
{
    auto timeNow = Timestamp::Now();

    return std::max<Duration>(timeNow - modifiedTime_, timeNow - readTime_);
}

}}
