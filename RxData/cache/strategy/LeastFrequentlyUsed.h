#ifndef RxData_Cache_LeastFrequentlyUsed_h_IsIncluded
#define RxData_Cache_LeastFrequentlyUsed_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/strategy/EvictionStrategy.h"

namespace RxData
{

template <typename K>
class LeastFrequentlyUsed : public EvictionStrategy<K>
{
public:
    // ----------------------------------
    // Strategy algorithm
    // ----------------------------------

    /**
     * Removes n least frequently accessed/used entries from object-cache based on the access counter.
     */
//    static ISet<K> removeNLeastFrequentlyUsed(ObjectCache::Ptr objectCache, long numSamplesToRemove) const
//    {
//        if(numSamplesToRemove <= 0) return ISet<K>();

//        // ----------------------------------------------
//        // Get the n LFU entries
//        // ----------------------------------------------
//        ISet<K> setOfLFUs = LeastFrequentlyUsed::getNLeastFrequentlyUsedKeys<KEY>(objectCache, numSamplesToRemove);

//        // ----------------------------------------------
//        // Remove the n oldest entries
//        // ----------------------------------------------
//        for(typename std::set<K>::const_iterator it = setOfLFUs.begin(), it_end = setOfLFUs.end(); it != it_end; ++it)
//        {
//            K key = *it;

//            ObjectRoot::Ptr objRoot = objectCache->remove<K>(key);

//            // -- debug --
//            if(!objRoot) IDEBUG() << "Object with key " << key << " not found!";
//            // -- debug --
//        }

//        return setOfLFUs;
//    }

    /**
     * N Least Frequently Used (LFU) items are returned.
     */
//    static ISet<K> getNLeastFrequentlyUsedKeys(ObjectCache::Ptr objectCache, int64 numSamplesToRemove) const
//    {
//        // ----------------------------------------------
//        // Order entries according to number of times accessed
//        // Java simulating a C++ std::multimap
//        // ----------------------------------------------
//        IMultiMap<int64, K> LFUEntries;

//        // ----------------------------------------------
//        // Iterate through all objects and insert to LFU map
//        // ----------------------------------------------
//        for(MapObject::const_iterator it = objectCache->objects_.begin(), it_end = objectCache->objects_.end(); it != it_end; ++it)
//        {
//            ObjectRoot::Ptr obj = it->second;

//            // -- debug --
//            ASSERT(obj);
//            // -- debug --

//            if(!obj) continue;

//            LFUEntries.put(obj->getNumTimesAccessed(), obj->getObjectKey<K>());
//        }

//        // ----------------------------------------------
//        // Identify n least accessed (LeastFrequentlyUsed) entries
//        // ----------------------------------------------
//        ISet<K> setKeys;

//        for(typename IMultiMap<int64, K>::const_iterator it = LFUEntries.begin(), it_end = LFUEntries.end(); it != it_end; ++it)
//        {
//            // ---------------------------------------------
//            // Decremented when keys are added to setKyes
//            // ---------------------------------------------
//            if(numSamplesToRemove <= 0) break;

//            numSamplesToRemove--;

//            setKeys.insert(it->second);
//        }

//        return setKeys;
//    }

    // -----------------------------------------------------------
    // Interface EvictionStrategy
    // -----------------------------------------------------------

    ISet<K> Perform(ObjectCache::Ptr objectCache, int64 numSamples)
    {
        return objectCache->removeNLeastFrequentlyUsed<K>(numSamples);
//        return LeastFrequentlyUsed::removeNLeastFrequentlyUsed(objectCache, numSamples);
    }
};

}

#endif // LEASTFREQUENTLYUSED_H
