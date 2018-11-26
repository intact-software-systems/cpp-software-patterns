#include "RxStreamer/StreamerGroup.h"

namespace Reactor
{

StreamerGroup::StreamerGroup(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, RxDataAccessGroup group)
    : masterCacheId_(masterCacheId)
    , dataAccessPolicy_(dataAccessPolicy)
    , group_(group)
{ }

StreamerGroup::~StreamerGroup()
{ }

StreamerGroup StreamerGroup::Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, RxDataAccessGroup group)
{
    return StreamerGroup(masterCacheId, rxPolicy, group);
}

RxData::CacheDescription StreamerGroup::CacheId() const
{
    return masterCacheId_;
}

DataAccessPolicy StreamerGroup::Policy() const
{
    return dataAccessPolicy_;
}

RxDataAccessGroup StreamerGroup::Group() const
{
    return group_;
}

}

