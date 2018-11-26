#ifndef RxData_Cache_ObjectCacheProxy_h_IsIncluded
#define RxData_Cache_ObjectCacheProxy_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/object/ObjectState.h"
#include"RxData/cache/Cache.h"
#include"RxData/cache/object/ObjectCache.h"

namespace RxData
{

/**
 * @brief The ObjectCacheReader class
 */
template <typename KEY, typename DATA>
class ObjectCacheProxy
{
public:
    ObjectCacheProxy(Cache::Ptr cache, std::string typeName)
        : cache_(cache)
        , objectCache_(cache->getOrCreateObjectCache(typeName))
        , typeName_(typeName)
    {}
    ~ObjectCacheProxy()
    { }

    CLASS_TRAITS(ObjectCacheProxy)

    /**
	 * Clears the cache.
	 */
	void clearAll()
    {
        objectCache_->clearAll();
    }

	/**
	 * @return Number of cached objects.
	 */
	virtual int size() const
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
     * @brief containsKey
     * @param key
     * @return
     */
    bool containsKey(KEY key) const
    {
        return objectCache_->containsKey<KEY>(key);
    }

	// ----------------------------------
	// Read operations
	// ----------------------------------

	/**
	 * Find and return object identified with key.
	 */
    DATA read(KEY key) const
    {
        return objectCache_->read<DATA, KEY>(key);
    }

	/**
	 * Find and return list of ObjectReferences using keys.
	 */
	std::map<KEY, DATA> read(const std::list<KEY> &listOfKeys) const
    {
        return objectCache_->read<DATA, KEY>(listOfKeys);
    }

	/**
	 * Populate parameter map with all objects in cache.
	 */
	std::map<KEY, DATA> readAll() const
    {
        return objectCache_->readAll<DATA, KEY>();
    }

	/**
	 * Find and return objects with given read-state.
	 */
	std::list<DATA> readWithReadState(ObjectState::Type objectState) const
    {
        return objectCache_->readWithReadState<DATA>(objectState);
    }

    /**
     * @brief readKeysWithReadState
     * @param objectState
     * @return
     */
	std::set<KEY> readKeysWithReadState(ObjectState::Type objectState)
	{
		return objectCache_->readKeysWithReadState<KEY>(objectState);
	}

	/**
	 * Find and return objects with given write-state.
	 */
	std::list<DATA> readWithWriteState(ObjectState::Type objectState) const
    {
        return objectCache_->readWithWriteState<DATA>(objectState);
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
    typename ObjectReference<DATA>::Ptr write(DATA data, KEY key)
    {
        return objectCache_->write<DATA, KEY>(data, key, cache_->getHomeByName(typeName_) );
    }

	/**
	 * Write data to cache,
	 *
	 * @return The object references written to cache.
	 */
    std::map<KEY, typename ObjectReference<DATA>::Ptr > write(const std::map<KEY, DATA> &mapOfData)
    {
        return objectCache_->write<DATA, KEY>(mapOfData, cache_->getHomeByName(typeName_) );
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
    Cache::Ptr          cache_;
    ObjectCache::Ptr    objectCache_;
    std::string         typeName_;
};

}

#endif


