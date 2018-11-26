#include "RxStreamer/DataAccessGroupIterator.h"

namespace Reactor
{

DataAccessGroupIterator::DataAccessGroupIterator(const DataAccessGroup &group)
    : group_(group)
    , currentAccess_()
    , index_(0)
{ }

DataAccessGroupIterator::DataAccessGroupIterator()
    : group_()
    , currentAccess_()
    , index_(0)
{ }

DataAccessGroupIterator::~DataAccessGroupIterator()
{ }

bool DataAccessGroupIterator::IsCurrent() const
{
    Locker lock(this);
    return currentAccess_ != nullptr;
}

DataAccessBase::Ptr DataAccessGroupIterator::Current()
{
    Locker lock(this);
    return currentAccess_;
}

DataAccessBase::Ptr DataAccessGroupIterator::Next()
{
    Locker lock(this);

    if(!hasNext())
    {
        IWARNING() << "Iterator is beyond end!";
        return DataAccessBase::Ptr();
    }

    currentAccess_ = group_.getList().at(index_);
    return currentAccess_;
}

bool DataAccessGroupIterator::HasNext() const
{
    Locker lock(this);
    return hasNext();
}

void DataAccessGroupIterator::Remove()
{
    // Not supported
}

void DataAccessGroupIterator::Rewind()
{
    Locker lock(this);

    index_ = 0;
    currentAccess_ = DataAccessBase::Ptr();
}

bool DataAccessGroupIterator::IsStarted() const
{
    Locker lock(this);
    return index_ == 0;
}

bool DataAccessGroupIterator::hasNext() const
{
    return index_ < group_.Size();
}

int DataAccessGroupIterator::getAndIncrement()
{
    int pos = index_;
    ++index_;
    return pos;
}
}
