#include"RxData/cache/CacheBase.h"

namespace RxData
{

/**
 * @brief CacheBase::CacheBase
 * @param cacheUsage
 * @param cacheKind
 */
CacheBase::CacheBase(CacheUsage::Type cacheUsage, CacheKind::Type cacheKind)
    : cacheUsage_(cacheUsage)
    , cacheKind_(cacheKind)
    , objects_()
{ }

/**
 * @brief CacheBase::~CacheBase
 */
CacheBase::~CacheBase()
{ }

/**
 * Thread-safe function to get or create an ObjectCache as identified by the typeName.
 *
 * @param 	typeName
 * @return 	the new ObjectCache
 */
ObjectCache::Ptr CacheBase::getOrCreateObjectCache(std::string typeName)
{
    // ------------------------------------------
    // synchronized access to map of object-caches
    // ------------------------------------------
    MutexTypeLocker<MapObjectCache> lock(&objects_);

    // ------------------------------------------
    // Existing ObjectCache?
    // ------------------------------------------
    {
        MapObjectCache::iterator it = objects_.find(typeName);
        if(it != objects_.end())
        {
            ObjectCache::Ptr currCache = it->second;

            if(currCache)
            {
                return currCache;
            }
        }
    }

    // ------------------------------------------
    // Create ObjectCache
    // ------------------------------------------
    ObjectCache::Ptr newCache( new ObjectCache(typeName, this->GetPtr()) );
    objects_.put(typeName, newCache);

    return newCache;
}

/**
 * Thread-safe function to remove and clear the ObjectCache identified by the typeName.
 *
 * No side-effects when calling this function and ObjectCache does not exist.
 *
 * @param 	typeName
 * @return 	the removed ObjectCache
 */
ObjectCache::Ptr CacheBase::removeAndClearObjectCache(std::string typeName)
{
    ObjectCache::Ptr objectCache;

    // ------------------------------------------
    // synchronize access to map of object-caches
    // ------------------------------------------
    {
        MutexTypeLocker<MapObjectCache> lock(&objects_);

        {
            MapObjectCache::iterator it = objects_.find(typeName);
            if(it != objects_.end())
            {
                objectCache = it->second;
            }
        }

        objects_.erase(typeName);
    }

    if(objectCache)
    {
        objectCache->clearAll();
    }

    return objectCache;
}

/**
 * Clears out all objects stored in this cache.
 */
void CacheBase::clearAllObjects()
{
    MutexTypeLocker<MapObjectCache> lock(&objects_);

    for(MapObjectCache::iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
    {
        ObjectCache::Ptr objectCache = it->second;

        // -- debug --
        ASSERT(objectCache);
        // -- debug --

        objectCache->clearAll();
    }

    objects_.clear();
}

/**
 * @brief CacheBase::getCacheUsage
 * @return
 */
CacheUsage::Type CacheBase::getCacheUsage() const
{
    return cacheUsage_;
}

/**
 * @brief CacheBase::getCacheKind
 * @return
 */
CacheKind::Type CacheBase::getCacheKind() const
{
    return cacheKind_;
}

}
