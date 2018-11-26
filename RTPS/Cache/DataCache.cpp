/*
 * DataCache.cpp
 *
 *  Created on: Feb 28, 2012
 *      Author: knuthelv
 */

#include"RTPS/Cache/DataCache.h"
#include"RTPS/Cache/CacheChange.h"

namespace RTPS {

DataCache::DataCache()
{
}

DataCache::~DataCache()
{
}

/*-------------------------------------------------------------------------------------
This operation inserts the CacheChange a_change into the HistoryCache.
This operation will only fail if there are not enough resources to add the change to the HistoryCache. It is the
responsibility of the DDS service implementation to configure the HistoryCache in a manner consistent with the DDS
Entity RESOURCE_LIMITS QoS and to propagate any errors to the DDS-user in the manner specified by the DDS
specification.

This operation performs the following logical steps:
ADD a_change TO this.changes;
-------------------------------------------------------------------------------------*/
void DataCache::AddChange(const CacheChange::Ptr& cacheChange)
{
    // -- debug --
    ASSERT(mapSeqCacheChange_.count(cacheChange->GetSequenceNumber()) <= 0);
    // -- end debug --

    // TODO: check resource limits

    this->mapSeqCacheChange_.insert(
        std::pair<SequenceNumber, CacheChange::Ptr>(
            cacheChange->GetSequenceNumber(),
            cacheChange
        ));


    // -- debug --
    ASSERT(mapSeqCacheChange_.count(cacheChange->GetSequenceNumber()) == 1);
    // -- end debug --
}

/*-------------------------------------------------------------------------------------
This operation indicates that a previously-added CacheChange has become irrelevant and the details regarding the
CacheChange need not be maintained in the HistoryCache. The determination of irrelevance is made based on the QoS
associated with the related DDS entity and on the acknowledgment status of the CacheChange. This is described in
Section 8.4.1.
This operation performs the following logical steps:
REMOVE a_change FROM this.changes;
-------------------------------------------------------------------------------------*/
bool DataCache::RemoveChange(const CacheChange::Ptr& cacheChange)
{
    MapSeqCacheChange::const_iterator it = mapSeqCacheChange_.find(cacheChange->GetSequenceNumber());

    if(it == mapSeqCacheChange_.end())
    {
        IWARNING() << "sequence number " << cacheChange->GetSequenceNumber() << " is not present!";
        return false;
    }

    return mapSeqCacheChange_.erase(cacheChange->GetSequenceNumber()) == 1;
}

bool DataCache::RemoveChange(const SequenceNumber& sequenceNumber)
{
    MapSeqCacheChange::const_iterator it = mapSeqCacheChange_.find(sequenceNumber);

    if(it == mapSeqCacheChange_.end())
    {
        IWARNING() << "sequence number " << sequenceNumber.GetSequenceNumber() << " is not present!";
        return false;
    }

    return mapSeqCacheChange_.erase(sequenceNumber) == 1;
}

CacheChange::Ptr DataCache::GetChange(const SequenceNumber& sequenceNumber) const
{
    MapSeqCacheChange::const_iterator it = mapSeqCacheChange_.find(sequenceNumber);

    if(it == mapSeqCacheChange_.end())
    {
        IWARNING() << "sequence number " << sequenceNumber.GetSequenceNumber() << " is not present!";
        return CacheChange::Ptr();
    }

    return it->second;
}

/*-------------------------------------------------------------------------------------
This operation retrieves the smallest value of the CacheChange::sequenceNumber attribute among the CacheChange
stored in the HistoryCache. This operation performs the following logical steps:
min_seq_num := MIN { change.sequenceNumber WHERE (change IN this.changes) }
return min_seq_num;
-------------------------------------------------------------------------------------*/
SequenceNumber DataCache::GetSequenceNumberMin() const
{
    if(mapSeqCacheChange_.empty())
    {
        return SequenceNumber::UNKNOWN();
    }

    return mapSeqCacheChange_.begin()->first;
}

SequenceNumber DataCache::GetSequenceNumberMax() const
{
    if(mapSeqCacheChange_.empty())
    {
        return SequenceNumber::UNKNOWN();
    }

    return mapSeqCacheChange_.end()->first;
}

}
