#ifndef RxData_Cache_ObjectCacheReader_h_IsIncluded
#define RxData_Cache_ObjectCacheReader_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/object/ObjectState.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * @brief The ObjectCacheReader class
 */
class DLL_STATE ObjectCacheReader
{
public:
    ObjectCacheReader(ObjectCache::Ptr objectCache)
        : objectCache_(objectCache)
    { }
    ~ObjectCacheReader()
    { }

    CLASS_TRAITS(ObjectCacheReader)

    /**
     * @return Number of cached objects.
     */
    virtual size_t size() const
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
    template <typename KEY>
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
    template <typename DATA, typename KEY>
    DATA read(KEY key) const
    {
        return objectCache_->read<DATA, KEY>(key);
    }

    /**
     * Find and return list of ObjectReferences using keys.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> read(const std::list<KEY> &listOfKeys) const
    {
        return objectCache_->read<DATA, KEY>(listOfKeys);
    }

    /**
     * Populate parameter map with all objects in cache.
     */
    template <typename DATA, typename KEY>
    std::map<KEY, DATA> readAll() const
    {
        return objectCache_->readAll<DATA, KEY>();
    }

    /**
     * Find and return objects with given read-state.
     */
    template <typename DATA>
    std::list<DATA> readWithReadState(ObjectState::Type objectState) const
    {
        return objectCache_->readWithReadState<DATA>(objectState);
    }

    /**
     * @brief readKeysWithReadState
     * @param objectState
     * @return
     */
    template <typename KEY>
    std::set<KEY> readKeysWithReadState(ObjectState::Type objectState)
    {
        return objectCache_->readKeysWithReadState<KEY>(objectState);
    }

    /**
     * Find and return objects with given write-state.
     */
    template <typename DATA>
    std::list<DATA> readWithWriteState(ObjectState::Type objectState) const
    {
        return objectCache_->readWithWriteState<DATA>(objectState);
    }

private:
    ObjectCache::Ptr objectCache_;
};

}

#endif


