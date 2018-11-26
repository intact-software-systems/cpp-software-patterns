#ifndef RxData_Cache_ObjectCacheWriter_h_IsIncluded
#define RxData_Cache_ObjectCacheWriter_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/object/ObjectHomeBase.h"
#include"RxData/cache/interfaces/ObjectReference.h"
#include"RxData/cache/object/ObjectRoot.h"
#include"RxData/cache/object/ObjectCache.h"

namespace RxData
{

/**
 * @brief The ObjectCacheWriter class
 */
template <typename KEY, typename DATA>
class ObjectCacheWriter
{
public:
    ObjectCacheWriter(ObjectCache::Ptr objectCache)
        : objectCache_(objectCache)
    { }
    ~ObjectCacheWriter()
    { }

    CLASS_TRAITS(ObjectCacheWriter)

	/**
	 * @return Number of cached objects.
	 */	
	int size() const
    {
        return objectCache_->size();
    }
	
	/**
	 * @return TRUE if cache is empty.
	 */
	bool isEmpty() const
    {
        return objectCache_->isEmpty();
    }
	
	/**
	 * Clears the cache.
	 */
	void clearAll()
    {
        return objectCache_->clearAll();
    }

    /**
     * @brief containsKey
     * @param key
     * @return
     */
    bool containsKey(KEY key) const
    {
        return objectCache_->containsKey<KEY>(key);
    }

	// ----------------------------------
	// Take operations
	// ----------------------------------
	
	/**
	 * Remove and return object identified with key.
	 */
    DATA take(KEY key)
    {
        return objectCache_->take<DATA, KEY>(key);
    }
		
	/**
	 * Remove and return list of ObjectReferences using keys.
	 */
    std::map<KEY, DATA> take(const std::set<KEY> &listOfKeys)
    {
        return objectCache_->take<DATA, KEY>(listOfKeys);
    }

	/**
	 * Return and remove all objects in the cache. 
	 */
	std::map<KEY, DATA> takeAll()
    {
        return objectCache_->takeAll<DATA, KEY>();
    }

	// ----------------------------------
	// Write operations
	// ----------------------------------

	/**
	 * Write data to cache.
	 * 
	 * @return The object reference written to cache.
	 */
    typename ObjectReference<DATA>::Ptr write(DATA data, KEY key, ObjectHomeBase::Ptr objectHome)
    {
        return objectCache_->write<DATA, KEY>(data, key, objectHome);
    }
	
	/**
	 * Write data to cache, 
	 * 
	 * @return The object references written to cache.
	 */
    std::map<KEY, typename ObjectReference<DATA>::Ptr > write(const std::map<KEY, DATA> &mapOfData, ObjectHomeBase::Ptr objectHome)
    {
        return objectCache_->write<DATA, KEY>(mapOfData, objectHome);
    }

	/**
	 * Remove data with key.
	 * 
	 * @return ObjectRoot removed from cache, if any.
	 */
    ObjectRoot::Ptr remove(KEY key)
    {
        return objectCache_->remove<KEY>(key);
    }
	
	/**
	 * Remove data identified in list of keys.
	 */
    void remove(const std::set<KEY> &keyList)
    {
        objectCache_->remove<KEY>(keyList);
    }

	/**
	 * Removes n oldest entries from object-cache based on last access time.
	 * 
	 * @param <KEY>
	 * @param numSamplesToRemove
	 * @return removed key-set
	 */
	std::set<KEY> removeNLeastRecentlyUsed(int64 numSamplesToRemove)
    {
        return objectCache_->removeNLeastRecentlyUsed<KEY>(numSamplesToRemove);
    }
	
	/**
	 * Removes n least frequently accessed/used entries from object-cache based 
	 * on the access counter.
	 * 
	 * @param <KEY>
	 * @param numSamplesToRemove
	 * @return removed key-set
	 */
    std::set<KEY> removeNLeastFrequentlyUsed(int64 numSamplesToRemove)
    {
        return objectCache_->removeNLeastFrequentlyUsed<KEY>(numSamplesToRemove);
    }

private:
    ObjectCache::Ptr objectCache_;
};

}

#endif
