#ifndef RxData_Cache_DataReaderListener_h_IsIncluded
#define RxData_Cache_DataReaderListener_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/CacheBase.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * Implemented by ObjectHome to fill up cache. ObjectHome is the consumer/subscriber. 
 * 
 * @author KVik
 */
template <typename DATA, typename KEY>
class DataReaderListener
{
	virtual bool OnDataAvailable(const DATA &data, KEY key, CacheBase::Ptr source) = 0;
};

}

#endif
