/*
 * DataCacheBase.h
 *
 *  Created on: Feb 28, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Cache_DataCacheBase_h_IsIncluded
#define RTPS_Cache_DataCacheBase_h_IsIncluded

#include"RTPS/Cache/CacheChange.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE DataCacheBase
{
public:
    virtual ~DataCacheBase();

    /*
    This operation inserts the CacheChange a_change into the HistoryCache.
    This operation will only fail if there are not enough resources to add the change to the HistoryCache. It is the
    responsibility of the DDS service implementation to configure the HistoryCache in a manner consistent with the DDS
    Entity RESOURCE_LIMITS QoS and to propagate any errors to the DDS-user in the manner specified by the DDS
    specification.

    This operation performs the following logical steps:
    ADD a_change TO this.changes;
    */
    virtual void AddChange(const CacheChange::Ptr &change) = 0;

    /*
    This operation indicates that a previously-added CacheChange has become irrelevant and the details regarding the
    CacheChange need not be maintained in the HistoryCache. The determination of irrelevance is made based on the QoS
    associated with the related DDS entity and on the acknowledgment status of the CacheChange. This is described in
    Section 8.4.1.

    This operation performs the following logical steps:
    REMOVE a_change FROM this.changes;
     */
    virtual bool RemoveChange(const CacheChange::Ptr &change) = 0;
    virtual bool RemoveChange(const SequenceNumber &) = 0;

    virtual CacheChange::Ptr GetChange(const SequenceNumber &) const = 0;

    /*
    This operation retrieves the smallest value of the CacheChange::sequenceNumber attribute among the CacheChange
    stored in the HistoryCache.

    This operation performs the following logical steps:
    min_seq_num := MIN { change.sequenceNumber WHERE (change IN this.changes) }
    return min_seq_num;
    */
    virtual SequenceNumber 	GetSequenceNumberMin() const = 0;

    /*
    This operation retrieves the largest value of the CacheChange::sequenceNumber attribute among the CacheChange stored
    in the HistoryCache.

    This operation performs the following logical steps:
    max_seq_num := MAX { change.sequenceNumber WHERE (change IN this.changes) }
    return max_seq_num;
     */
    virtual SequenceNumber 	GetSequenceNumberMax() const = 0;
};

} // namespace RTPS

#endif // Cache_DataCacheBase_h_IsIncluded
