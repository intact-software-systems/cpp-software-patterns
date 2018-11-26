#ifndef RxData_Cache_LeastRecentlyUsed_h_IsIncluded
#define RxData_Cache_LeastRecentlyUsed_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/strategy/EvictionStrategy.h"

namespace RxData
{

template <typename K>
class LeastRecentlyUsed : public EvictionStrategy<K>
{
public:
    // ----------------------------------
    // Strategy algorithm
    // ----------------------------------

//    static ISet<K> removeNLeastRecentlyUsed(ObjectCache::Ptr objectCache, int64 numSamplesToRemove)
//    {
//        if(numSamplesToRemove <= 0) return ISet<K>();

//        // ----------------------------------------------
//        // Get the n oldest entries
//        // ----------------------------------------------
//        ISet<K> setOfLRUs = LeastRecentlyUsed::getNLeastRecentlyUsedKeysPrivate(objectCache, numSamplesToRemove);

//        // ----------------------------------------------
//        // Remove the n oldest entries
//        // ----------------------------------------------
//        for(typename std::set<K>::const_iterator it = setOfLRUs.begin(), it_end = setOfLRUs.end(); it != it_end; ++it)
//        {
//            K key = *it;

//            ObjectRoot::Ptr objRoot = objectCache->remove<K>(key);

//            // -- debug --
//            if(!objRoot) IDEBUG() << "Object with key " << key << " not found!";
//            // -- debug --
//        }

//        return setOfLRUs;
//    }

    /**
     * N Least Recently Used (LRU) items are returned.
     *
     * TODO! Implement using multimap (See: getNLeastFrequentlyUsedKeysPrivate) to completely
     * avoid the risk of duplicate items being replaced! This is however not a big problem since
     * the time is in milliseconds.
     *
     * @param <KEY>
     * @param numSamplesToRemove
     *
     * @return
     */
//    static ISet<K> getNLeastRecentlyUsedKeysPrivate(ObjectCache::Ptr objectCache, int64 numSamplesToRemove)
//    {
//        // ----------------------------------------------
//        // Order entries according to age
//        // ----------------------------------------------
//        IMap<int64, K> LRUEntries;

//        int64 currentTimeMs = Utility::GetCurrentTimeMs();

//        // ----------------------------------------------
//        // Iterate through all objects and insert to LRU map
//        // ----------------------------------------------
//        for(ObjectCache::MapObject::const_iterator it = objectCache->objects_.begin(), it_end = objectCache->objects_.end(); it != it_end; ++it)
//        {
//            ObjectRoot::Ptr obj = it->second;

//            // -- debug --
//            ASSERT(obj);
//            // -- debug --

//            if(obj)
//            {
//                int64 timeSinceAccessedMs = (currentTimeMs - obj->getLastAccessTime());
//                LRUEntries.put(timeSinceAccessedMs, obj->getObjectKey<K>());
//            }
//        }

//        // ----------------------------------------------
//        // Identify n oldest (LeastRecentlyUsed) entries
//        // ----------------------------------------------
//        ISet<K> setKeys;

//        for(typename IMap<int64, K>::const_reverse_iterator it = LRUEntries.rbegin(), it_end = LRUEntries.rend(); it != it_end; ++it)
//        {
//            numSamplesToRemove--;

//            setKeys.insert(it->second);

//            if(numSamplesToRemove <= 0) break;
//        }

//        return setKeys;
//    }

    // -----------------------------------------------------------
    // Interface EvictionStrategy
    // -----------------------------------------------------------

    ISet<K> Perform(ObjectCache::Ptr objectCache, int64 numSamples)
    {
        return objectCache->removeNLeastRecentlyUsed<K>(numSamples);
//        return LeastRecentlyUsed::removeNLeastRecentlyUsed(objectCache, numSamples);
    }
};

}

#endif

