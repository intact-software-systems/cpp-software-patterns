/*
 * DataCache.h
 *
 *  Created on: Feb 28, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Cache_DataCache_h_IsIncluded
#define RTPS_Cache_DataCache_h_IsIncluded

#include"RTPS/Cache/DataCacheBase.h"
#include"RTPS/Cache/CacheChangeBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"

namespace RTPS
{
/*-------------------------------------------------------------------------------------
The HistoryCache is part of the interface between DDS and RTPS and plays different roles on the reader and the writer
side.

On the writer side, the HistoryCache contains the partial history of changes to data-objects made by the corresponding
DDS Writer that are needed to service existing and future matched RTPS Reader endpoints. The partial history needed
depends on the DDS Qos and the state of the communications with the matched RTPS Reader endpoints.
On the reader side, it contains the partial superposition of changes to data-objects made by all the matched RTPS Writer
endpoints.

The word partial is used to indicate that it is not necessary that the full history of all changes ever made is maintained.
Rather what is needed is the subset of the history needed to meet the behavioral needs of the RTPS protocol and the QoS
needs of the related DDS entities. The rules that define this subset are defined by the RTPS protocol and depend both on
the state of the communications protocol and on the QoS of the related DDS entities.
The HistoryCache is part of the interface between DDS and RTPS. In other words, both the RTPS entities and their
related DDS entities are able to invoke the operations on their associated HistoryCache.
-------------------------------------------------------------------------------------*/

/**
 * @brief The DataCache class
 *
 * TODO: For each topic, create one reader and one writer cache.
 */
class DLL_STATE DataCache : public DataCacheBase
{
public:
	typedef std::map<SequenceNumber, CacheChange::Ptr> MapSeqCacheChange;
	// TODO: Organize CacheChange by InstanceHandle also!

public:
	DataCache();
	virtual ~DataCache();

public:
	virtual void AddChange(const CacheChange::Ptr &cacheChange);
	virtual bool RemoveChange(const CacheChange::Ptr &cacheChange);
	virtual bool RemoveChange(const SequenceNumber &sequenceNumber);

	// TODO: virtual CacheChange::Ptr GetChange(const DCPS::InstanceHandle &handle);
	virtual CacheChange::Ptr 	GetChange(const SequenceNumber &sequenceNumber) const;
	virtual SequenceNumber 		GetSequenceNumberMin() const;
	virtual SequenceNumber 		GetSequenceNumberMax() const;

public:
	MapSeqCacheChange&  		GetCacheChanges() { return mapSeqCacheChange_; }

private:
	MapSeqCacheChange  	mapSeqCacheChange_;
};

} // namespace RTPS

#endif // RTPS_Cache_DataCache_h_IsIncluded
