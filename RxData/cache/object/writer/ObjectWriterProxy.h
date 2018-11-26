#ifndef RxData_Cache_ObjectWriterProxy_h_IsIncluded
#define RxData_Cache_ObjectWriterProxy_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/Cache.h"
#include"RxData/cache/interfaces/ObjectWriter.h"
#include"RxData/cache/object/ObjectHome.h"

namespace RxData
{

/**
 * Simplified implementation of writing to the Cache.
 *
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA, typename KEY>
class ObjectWriterProxy : public ObjectWriter<DATA, KEY>
{
private:
    Cache::Ptr      cache_;      // TODO: Change to interface
    std::string 	typeName_;

public:
    ObjectWriterProxy(Cache::Ptr cache, std::string typeName)
        : cache_(cache)
        , typeName_(typeName)
    { }

    CLASS_TRAITS(ObjectWriterProxy)

    /**
     * Write (key, data) to Cache.
     */
    virtual bool Write(DATA data, KEY key)
    {
        return cache_->OnDataAvailable<DATA, KEY>(data, key);
    }

    /**
     * Write (key, data) to Cache.
     */
    virtual bool WriteAll(const std::map<KEY, DATA> &values)
    {
        bool allWritten = true;
        for(typename std::map<KEY, DATA>::const_iterator it = values.begin(), it_end = values.end(); it != it_end; ++it)
        {
            bool isWritten = cache_->OnDataAvailable<DATA, KEY>(it->second, it->first);

            if(!isWritten) allWritten = false;
        }

        return allWritten;
    }

    /**
     * Dispose of data using a key.
     */
    virtual bool Dispose(KEY key)
    {
        // --------------------------------------------------------
        // Get the ObjectHome and dispose data using a null pointer
        // --------------------------------------------------------
        typename ObjectHome<DATA, KEY>::Ptr objHome = cache_->getHomeByName<DATA, KEY>(typeName_);
        if(!objHome) return false;

        return objHome->OnDataAvailable(DATA(), key, cache_);
    }

    /**
     * Clears the data in the cache for all data-types registered.
     *
     * When the cache is cleared (empty) it can still be used without side-effects.
     */
    virtual void DisposeAll()
    {
        cache_->clearAllData();
    }

    /**
     * @brief ContainsKey
     * @param key
     * @return
     */
    virtual bool ContainsKey(KEY key)
    {
        // --------------------------------------------------------
        // Get the ObjectHome and check if key is present in cache
        // --------------------------------------------------------
        typename ObjectHome<DATA, KEY>::Ptr objHome = cache_->getHomeByName<DATA, KEY>(typeName_);
        if(!objHome) return false;

        return objHome->containsObject(key, cache_);
    }
};

}

#endif

