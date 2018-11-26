#ifndef RxData_Cache_ObjectReader_h_IsIncluded
#define RxData_Cache_ObjectReader_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Implemented by Proxies/wrappers of cache objects
 * 
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA, typename KEY>
class ObjectReader
{
public:
    CLASS_TRAITS(ObjectReader)

	/**
	 * Check if cache contains the key
	 */
    virtual bool containsKey(KEY key) = 0;

	/**
	 * Retrieve a cache object based on its object-id.
	 */
	virtual DATA findObject(KEY key) = 0;

	/**
	 * Obtain a list of all cached objects. 
	 */
    virtual std::map<KEY, DATA> getObjects() = 0;
	
	/**
	 * Obtain a list of cached objects that are newly created.
	 */
    virtual std::list<DATA> getCreatedObjects() = 0;
	
	/**
	 * Obtain a list of cached objects that are modified.
	 */
	virtual std::list<DATA> getModifiedObjects() = 0;
	
	/**
	 * Obtain a list of cached objects that are deleted.
	 */
	virtual std::list<DATA> getDeletedObjects() = 0;
};

}

#endif
