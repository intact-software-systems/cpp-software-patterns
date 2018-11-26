#include "RxStreamer/DataAccessGroupChainIterator.h"

namespace Reactor
{

DataAccessGroupChainIterator::DataAccessGroupChainIterator(const DataAccessGroupChain &chain)
    : chain_(chain)
    , group_()
    , groupIterator_()
    , index_(0)
{ }

DataAccessGroupChainIterator::~DataAccessGroupChainIterator()
{ }

bool DataAccessGroupChainIterator::IsCurrent() const
{
    return index_ != 0;
}

DataAccessGroup DataAccessGroupChainIterator::Current()
{
    Locker lock(this);
    return group_;
}

DataAccessGroupIterator DataAccessGroupChainIterator::GroupIterator()
{
    Locker lock(this);
    return groupIterator_;
}

bool DataAccessGroupChainIterator::HasNext() const
{
    Locker lock(this);
    return hasNext();
}

DataAccessGroup DataAccessGroupChainIterator::Next()
{
    Locker lock(this);

    if(!hasNext())
    {
        throw BaseLib::ErrorException("Group does not have next!");
    }

    int pos = getAndIncrement();

    DataAccessGroup group = chain_.Group().at(pos);

    groupIterator_ = DataAccessGroupIterator(group);

    return group;
}

void DataAccessGroupChainIterator::Remove()
{
    // Not supported
}

void DataAccessGroupChainIterator::Rewind()
{
    Locker lock(this);

    index_ = 0;
    group_ = DataAccessGroup();
    groupIterator_ = DataAccessGroupIterator();
}

bool DataAccessGroupChainIterator::hasNext() const
{
    return index_ < chain_.Group().size();
}

int DataAccessGroupChainIterator::getAndIncrement()
{
    int pos = index_;
    ++index_;
    return pos;
}

}

