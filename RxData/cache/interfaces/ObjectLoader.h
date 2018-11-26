#ifndef RxData_Cache_ObjectLoader_h_IsIncluded
#define RxData_Cache_ObjectLoader_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Interface to be implemented by the data-sources for the cache.
 * 
 * Example: A wrapper around a data-base access object. A wrapper around a topic subscriber.
 * 
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA, typename KEY>
class ObjectLoader
{
public:

    CLASS_TRAITS(ObjectLoader)

    /**
     * @brief containsKey
     * @param key
     * @return
     */
    virtual bool containsKey(KEY key) = 0;

	/**
	 * Search and return object of type DATA with KEY as identifier.
	 *  
	 * @param <KEY>
	 * @param key
	 * @return
	 */
	virtual DATA loadObject(KEY key) = 0;
	
	/**
	 * Returns all objects of type <DATA> associated to the data-source.
	 *  
	 * @return
	 */
	virtual std::map<KEY, DATA> loadAll() = 0;

	/**
	 * Return all modified objects of type DATA associated to the data-source.
	 * The classes implementing this interface are allowed to either return null 
	 * or all (same as loadAll) when they do not keep track of modifications. 
	 *  
	 * @return 
	 */
	virtual std::map<KEY, DATA> loadModified() = 0;
	
	/**
	 * @return TRUE if the data-source objects of type DATA have been updated since	last load.
	 */
	virtual bool isUpdated() = 0;
};

}

#endif
