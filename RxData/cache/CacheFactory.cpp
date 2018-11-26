#include"RxData/cache/CacheFactory.h"

namespace RxData {

/**
 * Creates a new Cache object and returns it. The Cache object is saved in the
 * factory and can be accessed by its name later.
 *
 * This method takes as a parameter cache_usage, which indicates the future usage
 * of the Cache (namely WRITE ONLY no subscription, READ ONLY no publication, or
 * READ WRITE both modes) and a description of the Cache (at a minimum, this
 * CacheDescription gathers the concerned DomainParticipant as well as a name allocated
 * to the Cache). Depending on the cache_usage a Publisher, a Subscriber, or both
 * will be created for the unique usage of the Cache. These two objects will be attached
 * to the passed DomainParticipant.
 *
 * @see Note! According to the specification, the CacheUsage for a Cache should always be READ-ONLY!
 *
 * @param description
 * @return
 */
Cache::Ptr CacheFactory::getOrCreateCache(CacheDescription description)
{
    MutexTypeLocker<CacheFactory> lock(this);

    // ---------------------------------------------------------
    // if description name already exists, return that Cache
    // ---------------------------------------------------------
    if(dataMap_.containsKey(description))
    {
        return dataMap_.at(description);
    }

    // ---------------------------------------------------------
    // Create a new Cache based on the description name
    // ---------------------------------------------------------
    bool updatesEnabled = true;
    Cache::Ptr cache( new Cache(description, updatesEnabled) );

    // ---------------------------------------------------------
    // For cache monitoring and policy attach the virtual machine as a listener
    // ---------------------------------------------------------
    cache->Connect(&RxData::VirtualMachineCache::Supervisor());

    {
        bool isInserted = dataMap_.put(description, cache);

        // -- debug --
        ASSERT(isInserted);
        // -- debug --
    }

    return cache;
}

/**
 * To retrieve a Cache based on the name given in the CacheDescription (find_cache_by_name).
 * If the specified name does not identify an existing Cache, a NULL is returned.
 *
 * @param cacheName
 * @return
 */
Cache::Ptr CacheFactory::findCacheByName(std::string cacheName)
{
    MutexTypeLocker<CacheFactory> lock(this);

    for(CacheMap::iterator it = dataMap_.begin(), it_end = dataMap_.end(); it != it_end; ++it)
    {
        Cache::Ptr cache = it->second;

        if(cache->getCacheName() == cacheName)
            return cache;
    }

    return Cache::Ptr();
}

/**
 * To retrieve a Cache based on the CacheDescription.
 *
 * If the specified description does not identify an existing Cache, a NULL is returned.
 *
 * @param description
 * @return
 */
Cache::Ptr CacheFactory::findCacheByDescription(CacheDescription description)
{
    MutexTypeLocker<CacheFactory> lock(this);

    if(dataMap_.containsKey(description))
    {
        return dataMap_.at(description);
    }

    return Cache::Ptr();
}

/**
 * To delete a Cache (delete_cache). This operation releases all the resources allocated to the Cache.
 *
 * @param cache
 */
void CacheFactory::deleteCache(Cache::Ptr cache)
{
    MutexTypeLocker<CacheFactory> lock(this);

    cache->clearAllData();

    dataMap_.erase(cache->getCacheDescription());
}

/**
 * Clears (not removes) all Cache objects created by this CacheFactory.
 */
void CacheFactory::clearAllCaches()
{
    MutexTypeLocker<CacheFactory> lock(this);

    for(CacheMap::iterator it = dataMap_.begin(), it_end = dataMap_.end(); it != it_end; ++it)
    {
        Cache::Ptr cache = it->second;

        cache->clearAllData();
    }
}

/**
 * Clears and removes all Cache objects created by this CacheFactory.
 */
void CacheFactory::deleteAllCaches()
{
    MutexTypeLocker<CacheFactory> lock(this);

    for(CacheMap::iterator it = dataMap_.begin(), it_end = dataMap_.end(); it != it_end; ++it)
    {
        Cache::Ptr cache = it->second;

        cache->clearAllData();
    }

    dataMap_.clear();
}

ObjectHomePolicy CacheFactory::defaultCachePolicy() const
{
    return ObjectHomePolicy::Default();
}

}
