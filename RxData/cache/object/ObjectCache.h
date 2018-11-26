#ifndef RxData_Cache_ObjectCache_h_IsIncluded
#define RxData_Cache_ObjectCache_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/CacheInterface.h"
#include"RxData/cache/interfaces/ObjectReference.h"
#include"RxData/cache/object/ObjectReference.h"

#include"RxData/cache/object/ObjectId.h"
#include"RxData/cache/object/ObjectRoot.h"
#include"RxData/cache/object/ObjectHomeBase.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * A thread-safe cache for a specific type of data. Provides read and write access.
 *
 * The two interfaces, ObjectCacheReader and ObjectCacheWriter, separates
 * the read and write access to the ObjectCache
 *
 * TODO: Inherit from IMap and only implement functions not present in IMap.
 *
 * @author KVik
 */
class DLL_STATE ObjectCache : public ENABLE_SHARED_FROM_THIS(ObjectCache)
{
public:
    // ----------------------------------
    // Typedefs
    // ----------------------------------
    typedef IMap<ObjectId, ObjectRoot::Ptr> MapObject;

private:
    // ----------------------------------
    // Attributes
    // ----------------------------------

    /**
     * The actual data objects in the cache.
     */
    MapObject objects_;

    /**
     * Type-name that the cache stores.
     */
    std::string typeName_;

    /**
     * The Cache that owns this ObjectCache.
     */
    CacheBaseInterface::WeakPtr owner_;

    /**
     * @brief mutex_
     */
    mutable Mutex mutex_;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    ObjectCache(std::string typeName_, CacheBaseInterface::WeakPtr owner);
    virtual ~ObjectCache();

    CLASS_TRAITS(ObjectCache)

    /**
     * @brief GetPtr
     * @return
     */
    ObjectCache::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    // ----------------------------------
    // Misc. cache operations
    // ----------------------------------

    /**
     * Clears the cache.
     */
    void clearAll();

    /**
     * @return Number of cached objects.
     */
    size_t size() const;

    /**
     * @return TRUE if cache is empty.
     */
    bool isEmpty() const;

    /**
     * @brief containsKey
     * @param key
     * @return
     */
    template <typename KEY>
    bool containsKey(KEY key) const
    {
        MutexLocker lock(&mutex_);
        return containsKeyPrivate<KEY>(key);
    }

    /**
     * @brief copyObjectToCache
     * @param key
     * @param destination
     * @return
     */
    bool copyObjectToCache(ObjectId key, ObjectCache::Ptr destination);

    // ----------------------------------
    // Read and take operations
    // ----------------------------------

    /**
     * Find and return object identified with key.
     */
    template <typename DATA, typename KEY>
    DATA read(KEY key) const
    {
        typename Impl::ObjectReference<DATA>::Ptr objRef = readPrivate<DATA, KEY>(key);
        if(!objRef) return DATA();

        return objRef->get();
    }

    /**
     * Find and return list of ObjectReferences using keys.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> read(const std::list<KEY> &listOfKeys) const
    {
        // ----------------------------------------------------
        // Map of objects to be returned
        // ----------------------------------------------------
        IMap<KEY, DATA> mapOfObjects;

        // ----------------------------------------------------
        // Iterate through keys, find cached data and enter map
        // ----------------------------------------------------
        typename Impl::ObjectReference<DATA>::Ptr objRef;
        for(typename std::list<KEY>::const_iterator it = listOfKeys.begin(), it_end = listOfKeys.end(); it != it_end; ++it)
        {
            KEY key = *it;

            objRef = readPrivate<DATA, KEY>(key);

            // -- debug --
            ASSERT(objRef);
            // -- debug --

            // TODO: necessary?
            // if(!objRef) mapOfObjects.put(key, DATA());

            if(objRef)
            {
                mapOfObjects.put(key, objRef->get());
            }
        }

        return mapOfObjects;
    }

    /**
     * Populate parameter map with all objects in cache.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> readAll() const
    {
        return readAllPrivate<DATA, KEY>();
    }

    /**
     * Remove and return object identified with key.
     */
    template <typename DATA, typename KEY>
    DATA take(KEY key)
    {
        typename Impl::ObjectReference<DATA>::Ptr objRef = takePrivate<DATA, KEY>(key);
        if(!objRef) return DATA();

        return objRef->get();
    }

    /**
     * Remove and return list of ObjectReferences using keys.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> take(const std::set<KEY> &listOfKeys)
    {
        // ----------------------------------------------------
        // Map of objects to be returned
        // ----------------------------------------------------
        IMap<KEY, DATA> mapOfObjects;

        // ----------------------------------------------------
        // Iterate through keys, find cached data and enter map
        // ----------------------------------------------------
        typename Impl::ObjectReference<DATA>::Ptr objRef;
        for(typename std::set<KEY>::const_iterator it = listOfKeys.begin(), it_end = listOfKeys.end(); it != it_end; ++it)
        {
            KEY key = *it;

            objRef = takePrivate<DATA, KEY>(key);

            // TODO: necessary?
            // if(!objRef) mapOfObjects.put(key, DATA());

            if(objRef)
            {
                mapOfObjects.put(key, objRef->get());
            }
        }

        return mapOfObjects;
    }

    /**
     * Return and remove all objects in the cache.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> takeAll()
    {
        return takeAllPrivate<DATA, KEY>();
    }

    /**
     * Find and return objects with given read-state.
     */
    template <typename DATA>
    std::list<DATA> readWithReadState(ObjectState::Type objectState)
    {
        return readWithReadStatePrivate<DATA>(objectState);
    }

    /**
     * Find and return object-keys referring to objects with given read-state.
     */
    template <typename KEY>
    std::set<KEY> readKeysWithReadState(ObjectState::Type objectState)
    {
        return readKeysWithReadStatePrivate<KEY>(objectState);
    }

    /**
     * Find and return objects with given write-state.
     */
    template <typename DATA>
    std::list<DATA> readWithWriteState(ObjectState::Type objectState)
    {
        return readWithWriteStatePrivate<DATA>(objectState);
    }

    // ----------------------------------
    // Write operations
    // ----------------------------------

    /**
     * Write data to cache.
     *
     * @return The object reference written to cache.
     */
    template <typename DATA, typename KEY>
    typename Impl::ObjectReference<DATA>::Ptr write(const DATA &data, KEY key, ObjectHomeBase::Ptr objectHome)
    {
        return writePrivate<DATA, KEY>(data, key, objectHome);
    }

    /**
     * Write data to cache,
     *
     * @return The object references written to cache.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, typename ObjectReference<DATA>::Ptr > write(const std::map<KEY, DATA> &mapOfData, ObjectHomeBase::Ptr objectHome)
    {
        // ------------------------------------------------
        // The object references are returned
        // ------------------------------------------------
        IMap<KEY, typename ObjectReference<DATA>::Ptr > outMapOfData;

        // ------------------------------------------------
        // Iterate, write to cache, and fill up outMap to
        // be returned to caller
        // ------------------------------------------------
        for(typename std::map<KEY, DATA>::const_iterator it = mapOfData.begin(), it_end = mapOfData.end(); it != it_end; ++it)
        {
            typename ObjectReference<DATA>::Ptr objRef = writePrivate<DATA, KEY>(it->second, it->first, objectHome);

            // -- debug --
            ASSERT(objRef);
            // -- debug --

            outMapOfData.put(it->first, objRef);
        }

        return outMapOfData;
    }

    /**
     * Write ObjectRoot to cache.
     */
    bool write(ObjectRoot::Ptr newObj);


    /**
     * Remove data with key.
     *
     * @return ObjectRoot removed from cache, if any.
     */
    template <typename KEY>
    ObjectRoot::Ptr remove(KEY key)
    {
        return removePrivate<KEY>(key);
    }

    /**
     * Remove data identified in list of keys.
     */
    template <typename KEY>
    void remove(const std::set<KEY> &keyList)
    {
        for(typename std::set<KEY>::const_iterator it = keyList.begin(), it_end = keyList.end(); it != it_end; ++it)
        {
            removePrivate<KEY>(*it);
        }
    }

    /**
     * Removes n oldest entries from object-cache based on last access time.
     *
     * @param <KEY>
     * @param numSamplesToRemove
     * @return removed key-set
     */
    template <typename KEY>
    std::set<KEY> removeNLeastRecentlyUsed(int64 numSamplesToRemove)
    {
        if(numSamplesToRemove <= 0) return std::set<KEY>();

        // ----------------------------------------------
        // Get the n oldest entries
        // ----------------------------------------------
        std::set<KEY> setOfLRUs = getNLeastRecentlyUsedKeysPrivate<KEY>(numSamplesToRemove);

        // ----------------------------------------------
        // Remove the n oldest entries
        // ----------------------------------------------
        for(typename std::set<KEY>::const_iterator it = setOfLRUs.begin(), it_end = setOfLRUs.end(); it != it_end; ++it)
        {
            KEY key = *it;

            ObjectRoot::Ptr objRoot = removePrivate<KEY>(key);

            // -- debug --
            if(!objRoot) IDEBUG() << "Object with key " << key << " not found!";
            // -- debug --
        }

        return setOfLRUs;
    }

    /**
     * Removes n least frequently accessed/used entries from object-cache based
     * on the access counter.
     *
     * @param <KEY>
     * @param numSamplesToRemove
     * @return removed key-set
     */
    template <typename KEY>
    std::set<KEY> removeNLeastFrequentlyUsed(int64 numSamplesToRemove)
    {
        if(numSamplesToRemove <= 0) return std::set<KEY>();

        // ----------------------------------------------
        // Get the n LFU entries
        // ----------------------------------------------
        std::set<KEY> setOfLFUs = getNLeastFrequentlyUsedKeysPrivate<KEY>(numSamplesToRemove);

        // ----------------------------------------------
        // Remove the n oldest entries
        // ----------------------------------------------
        for(typename std::set<KEY>::const_iterator it = setOfLFUs.begin(), it_end = setOfLFUs.end(); it != it_end; ++it)
        {
            KEY key = *it;

            ObjectRoot::Ptr objRoot = removePrivate<KEY>(key);

            // -- debug --
            if(!objRoot) IDEBUG() << "Object with key " << key << " not found!";
            // -- debug --
        }

        return setOfLFUs;
    }

private:
    // --------------------------------------------------------
    // Synchronized and private implementations of map access
    // --------------------------------------------------------
    template <typename DATA>
    static typename Impl::ObjectReference<DATA>::Ptr castToObjectReference(ObjectRoot::Ptr objRoot)
    {
        if(!objRoot)
        {
            return typename Impl::ObjectReference<DATA>::Ptr();
        }

        return std::dynamic_pointer_cast<Impl::ObjectReference<DATA>>(objRoot);
    }

    /**
     * Clears the cache
     */
    void clearAllPrivate()
    {
        MutexLocker lock(&mutex_);
        objects_.clear();
    }

    /**
     * @brief sizePrivate
     * @return
     */
    size_t sizePrivate() const
    {
        MutexLocker lock(&mutex_);
        return objects_.size();
    }

    /**
     * @brief isEmptyPrivate
     * @return
     */
    bool isEmptyPrivate() const
    {
        MutexLocker lock(&mutex_);
        return objects_.empty();
    }

    /**
     * Find and return object identified with key.
     */
    template <typename DATA, typename KEY>
    typename Impl::ObjectReference<DATA>::Ptr readPrivate(KEY key) const
    {
        MutexLocker lock(&mutex_);

        MapObject::const_iterator pos = this->findObject<KEY>(key);

        if(pos == objects_.end())
            return (typename Impl::ObjectReference<DATA>::Ptr());

        ObjectRoot::Ptr objRoot = pos->second;

        return ObjectCache::castToObjectReference<DATA>(objRoot);
    }

    /**
     * Populate parameter map with all objects in cache.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> readAllPrivate() const
    {
        MutexLocker lock(&mutex_);

        // ----------------------------------------------------
        // Map of objects to be returned
        // ----------------------------------------------------
        IMap<KEY, DATA> mapOfObjects;

        // ----------------------------------------------------
        // Put all objects to created map
        // ----------------------------------------------------
        readAllObjectsToMap<DATA, KEY>(mapOfObjects);

        return (std::map<KEY, DATA>)mapOfObjects;
    }

    /**
     * Remove and return object identified with key.
     */
    template <typename DATA, typename KEY>
    typename Impl::ObjectReference<DATA>::Ptr takePrivate(KEY key)
    {
        MutexLocker lock(&mutex_);

        ObjectRoot::Ptr objRoot = this->removeObject<KEY>(key);
        if(!objRoot)
        {
            return ObjectReference<DATA>::Ptr();
        }

        return ObjectCache::castToObjectReference<DATA>(objRoot);
    }

    /**
     * Return and remove all objects in the cache.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> takeAllPrivate()
    {
        MutexLocker lock(&mutex_);

        IMap<KEY, DATA> mapOfObjects;

        // ----------------------------------------------------
        // Read the objects and replace objects
        // ----------------------------------------------------
        readAllObjectsToMap<DATA, KEY>(mapOfObjects);

        objects_.clear();

        return (std::map<KEY, DATA>)mapOfObjects;
    }

    /**
     * Inserts all objects in objects to the argument mapOfObjects.
     */
    template <typename DATA, typename KEY>
    void readAllObjectsToMap(IMap<KEY, DATA> &mapOfObjects) const
    {
        for(MapObject::const_iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
        {
            const ObjectId &id = it->first;

            KEY key = id.get<KEY>();

            typename Impl::ObjectReference<DATA>::Ptr objRef = ObjectCache::castToObjectReference<DATA>(it->second);

            // -- debug --
            ASSERT(objRef);
            // -- debug --

            mapOfObjects.put(key, objRef->get());
        }
    }

    /**
     * Private synchronized implementation of writing to the cache.
     */
    template <typename DATA, typename KEY>
    typename Impl::ObjectReference<DATA>::Ptr writePrivate(const DATA &data, KEY key, ObjectHomeBase::Ptr objectHome)
    {
        MutexLocker lock(&mutex_);

        typename Impl::ObjectReference<DATA>::Ptr objRef;

        if(this->containsKeyPrivate<KEY>(key))
        {
            ObjectRoot::Ptr obj = getObject<KEY>(key);
            objRef = ObjectCache::castToObjectReference<DATA>(obj);
        }

        // --------------------------------------------------
        // if exists then update data and meta-data of object
        // --------------------------------------------------
        if(objRef)
        {
            bool isDataIdentical = (objRef->get() == data);
            bool isDataNull = false; //!data;

            if(!isDataIdentical)
            {
                objRef->set(data);
            }

            ObjectCache::updateObjectState(isDataIdentical, isDataNull, objRef);
            // bool isUpdated;
        }
        // --------------------------------------------------
        // if new then create object-holder and add to cache
        // --------------------------------------------------
        else //if(!obj) // && data)
        {
            objRef = ObjectCache::createObjectReference<DATA, KEY>(data, key, objectHome, owner_);

            // -- debug --
            ASSERT(objRef);
            // -- debug --

            bool isInserted = objects_.put(objRef->getObjectId(), objRef);

            // -- debug --
            ASSERT(isInserted);
            // -- debug --
        }
//		else //if(data) // (obj instanceof ObjectReference) == false
//		{
//			// --------------------------------------
//			// Q: Could or should this happen!!!?
//			// A: NO!!!!
//			// --------------------------------------

//			// -- debug --
//			ICRITICAL() << "Cache writing failed for data: " << TYPE_NAME(data);
//            ASSERT(objRef);
//            //ASSERT(obj instanceof ObjectReference);
//			// -- debug --
//		}

        return objRef;
    }

    /**
     * Update object reference states.
     *
     * 	Q: Is it an allowed state to have data = null?
     * 	A: Yes, it should be allowed to clear out data.
     */
    static bool updateObjectState(bool isDataIdentical, bool isDataNull, ObjectRoot::Ptr objRef);

    /**
     * Factory function for ObjectReference<DATA>.
     */
    template <typename DATA, typename KEY>
    static typename Impl::ObjectReference<DATA>::Ptr createObjectReference(const DATA &data, KEY key, ObjectHomeBase::Ptr objectHome, CacheBaseInterface::WeakPtr owner)
    {
        typename Impl::ObjectReference<DATA>::Ptr objRef;

        CacheBaseInterface::Ptr ownerPtr = owner.lock();

        if(!ownerPtr)
        {
            ICRITICAL() << "Owner is NULL!";
            return objRef;
        }

        // ---------------------------------------------------
        // When READ_ONLY, the write-state is VOID
        // ---------------------------------------------------
        if(ownerPtr->getCacheUsage() == CacheUsage::READ_ONLY)
        {
            // -- debug --
            ASSERT(ownerPtr->getCacheKind() == CacheKind::CACHE_KIND || ownerPtr->getCacheKind() == CacheKind::CACHEACCESS_KIND);
            // -- debug --

            objRef = ObjectCache::createObjectReference<DATA, KEY>(
                            data,
                            key,
                            ObjectState::OBJECT_NEW,
                            ObjectState::OBJECT_VOID,
                            objectHome,
                            TYPE_NAME(data),
                            owner);
        }
        else if(ownerPtr->getCacheUsage() == CacheUsage::WRITE_ONLY)
        {
            // -- debug --
            ASSERT(ownerPtr->getCacheKind() == CacheKind::CACHEACCESS_KIND);
            // -- debug --

            objRef = ObjectCache::createObjectReference<DATA, KEY>(
                            data,
                            key,
                            ObjectState::OBJECT_VOID,
                            ObjectState::OBJECT_NEW,
                            objectHome,
                            TYPE_NAME(data),
                            owner);
        }
        else if(ownerPtr->getCacheUsage() == CacheUsage::READ_WRITE)
        {
            // -- debug --
            ASSERT(ownerPtr->getCacheKind() == CacheKind::CACHEACCESS_KIND);
            // -- debug --

            objRef = ObjectCache::createObjectReference<DATA, KEY>(
                            data,
                            key,
                            ObjectState::OBJECT_NEW,
                            ObjectState::OBJECT_NEW,
                            objectHome,
                            TYPE_NAME(data),
                            owner);
        }

        return objRef;
    }

    /**
     * "Factory" for ObjectReference<DATA>
     */
    template <typename DATA, typename KEY>
    static typename Impl::ObjectReference<DATA>::Ptr createObjectReference(
            const DATA &data,
            KEY key,
            ObjectState::Type readState,
            ObjectState::Type writeState,
            ObjectHomeBase::Ptr objectHome,
            std::string typeName,
            CacheBaseInterface::WeakPtr owner)
    {
        return Impl::ObjectReference<DATA>::createObjectReference(
                    data,
                    ObjectId(key),
                    readState,
                    writeState,
                    objectHome,
                    typeName,
                    owner);

//		return typename Cache::ObjectReference<DATA>::Ptr( new Impl::ObjectReference<DATA>(
//				data,
//				key,
//				readState,
//				writeState,
//				objectHome,
//				typeName,
//				owner));
    }

    /**
     * Find and return objects with given read-state.
     */
    template <typename DATA>
    std::list<DATA> readWithReadStatePrivate(ObjectState::Type objectState) const
    {
        MutexLocker lock(&mutex_);

        std::list<DATA> listObjects;

        for(MapObject::const_iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
        {
            ObjectRoot::Ptr obj = it->second;

            // -- debug --
            ASSERT(obj);
            // -- debug --

            if(obj && obj->getReadState() == objectState)
            {
                typename Impl::ObjectReference<DATA>::Ptr objRef = ObjectCache::castToObjectReference<DATA>(obj);

                if(objRef) listObjects.push_back(objRef->get());
            }
        }

        return listObjects;
    }

    /**
     * Find and return object-keys with given read-state.
     */
    template <typename KEY>
    std::set<KEY> readKeysWithReadStatePrivate(ObjectState::Type objectState) const
    {
        MutexLocker lock(&mutex_);

        std::set<KEY> setKeys;

        for(MapObject::const_iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
        {
            ObjectRoot::Ptr obj = it->second;

            // -- debug --
            ASSERT(obj);
            // -- debug --

            if(obj && obj->getReadState() == objectState)
            {
                setKeys.insert(obj->getObjectKey<KEY>());
            }
        }

        return setKeys;
    }

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
    template <typename KEY>
    std::set<KEY> getNLeastRecentlyUsedKeysPrivate(int64 numSamplesToRemove) const
    {
        MutexLocker lock(&mutex_);

        // ----------------------------------------------
        // Order entries according to age
        // ----------------------------------------------
        IMap<int64, KEY> LRUEntries;

        int64 currentTimeMs = Utility::GetCurrentTimeMs();

        // ----------------------------------------------
        // Iterate through all objects and insert to LRU map
        // ----------------------------------------------
        for(MapObject::const_iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
        {
            ObjectRoot::Ptr obj = it->second;

            // -- debug --
            ASSERT(obj);
            // -- debug --

            if(obj)
            {
                int64 timeSinceAccessedMs = (currentTimeMs - obj->getLastAccessTime());
                LRUEntries.put(timeSinceAccessedMs, obj->getObjectKey<KEY>());
            }
        }

        // ----------------------------------------------
        // Identify n oldest (LeastRecentlyUsed) entries
        // ----------------------------------------------
        std::set<KEY> setKeys;

        for(typename IMap<int64, KEY>::const_reverse_iterator it = LRUEntries.rbegin(), it_end = LRUEntries.rend(); it != it_end; ++it)
        {
            numSamplesToRemove--;

            setKeys.insert(it->second);

            if(numSamplesToRemove <= 0) break;
        }

        return setKeys;
    }

    /**
     * N Least Frequently Used (LFU) items are returned.
     *
     * @param <KEY>
     * @param numSamplesToRemove
     *
     * @return
     */
    template <typename KEY>
    std::set<KEY> getNLeastFrequentlyUsedKeysPrivate(int64 numSamplesToRemove) const
    {
        MutexLocker lock(&mutex_);

        // ----------------------------------------------
        // Order entries according to number of times accessed
        // Java simulating a C++ std::multimap
        // ----------------------------------------------
        IMultiMap<int64, KEY> LFUEntries;

        // ----------------------------------------------
        // Iterate through all objects and insert to LFU map
        // ----------------------------------------------
        for(MapObject::const_iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
        {
            ObjectRoot::Ptr obj = it->second;

            // -- debug --
            ASSERT(obj);
            // -- debug --

            if(!obj) continue;

            LFUEntries.put(obj->getNumTimesAccessed(), obj->getObjectKey<KEY>());
        }

        // ----------------------------------------------
        // Identify n least accessed (LeastFrequentlyUsed) entries
        // ----------------------------------------------
        std::set<KEY> setKeys;

        for(typename IMultiMap<int64, KEY>::const_iterator it = LFUEntries.begin(), it_end = LFUEntries.end(); it != it_end; ++it)
        {
            // ---------------------------------------------
            // Decremented when keys are added to setKyes
            // ---------------------------------------------
            if(numSamplesToRemove <= 0) break;

            numSamplesToRemove--;

            setKeys.insert(it->second);
        }

        return setKeys;
    }

    /**
     * Find and return objects with given write-state.
     */
    template <typename DATA>
    std::list<DATA> readWithWriteStatePrivate(ObjectState::Type objectState) const
    {
        MutexLocker lock(&mutex_);

        std::list<DATA> listObjects;

        for(MapObject::const_iterator it = objects_.begin(), it_end = objects_.end(); it != it_end; ++it)
        {
            ObjectRoot::Ptr obj = it->second;

            // -- debug --
            ASSERT(obj);
            // -- debug --

            if(obj && obj->getWriteState() == objectState)
            {
                typename Impl::ObjectReference<DATA>::Ptr objRef = ObjectCache::castToObjectReference<DATA>(obj);

                if(objRef)
                {
                    listObjects.push_back(objRef->get());
                }
            }
        }

        return listObjects;
    }

    /**
     * Remove data with key.
     *
     * @return ObjectRoot removed from cache, if any.
     */
    template <typename KEY>
    ObjectRoot::Ptr removePrivate(KEY key)
    {
        MutexLocker lock(&mutex_);
        return removeObject<KEY>(key);
    }

    /**
     * @brief containsKeyPrivate
     * @param key
     * @return
     */
    template <typename KEY>
    bool containsKeyPrivate(KEY key) const
    {
        MapObject::const_iterator pos = this->findObject<KEY>(key);

        return pos != objects_.end();
    }

    // ------------------------------------------------------
    // To allow the user to use ObjectId as key
    // ------------------------------------------------------

    template <typename KEY>
    MapObject::const_iterator findObject(KEY key) const;

    template <typename KEY>
    ObjectRoot::Ptr removeObject(KEY key);

    template <typename KEY>
    ObjectRoot::Ptr getObject(KEY key) const;
};


/**
 * @brief ObjectCache::getObject<ObjectId>
 * @param key
 * @return
 */
//template <>
//typename ObjectRoot::Ptr ObjectCache::getObject<ObjectId>(ObjectId key) const
//{
//    ObjectRoot::Ptr obj = objects.get(key);
//    return obj->GetPtr();
//}

//template <>
//ObjectCache::MapObject::const_iterator ObjectCache::findObject<ObjectId>(ObjectId key) const
//{
//    return objects.find(key);
//}

//template <>
//ObjectRoot::Ptr ObjectCache::removeObject<ObjectId>(ObjectId key)
//{
//    return objects.remove(key);
//}

template <typename KEY>
ObjectCache::MapObject::const_iterator ObjectCache::findObject(KEY key) const
{
//    if(typeid(key) == typeid(ObjectId) )
//        return objects.find(key);

    return objects_.find(ObjectId(key));
}

template <typename KEY>
ObjectRoot::Ptr ObjectCache::removeObject(KEY key)
{
    return objects_.remove(ObjectId(key));
}

template <typename KEY>
ObjectRoot::Ptr ObjectCache::getObject(KEY key) const
{
    return objects_.get(ObjectId(key));
}

}

#endif

