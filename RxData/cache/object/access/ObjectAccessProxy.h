#ifndef OBJECTACCESSPROXY_H
#define OBJECTACCESSPROXY_H

#include"RxData/CommonDefines.h"

#include"RxData/cache/Cache.h"
#include"RxData/cache/interfaces/ObjectAccess.h"
#include"RxData/cache/object/ObjectHome.h"
#include"RxData/cache/object/reader/ObjectReaderStub.h"
#include"RxData/cache/object/writer/ObjectWriterProxy.h"


namespace RxData
{

template <typename DATA, typename KEY>
class ObjectAccessProxy : public ObjectAccess<DATA, KEY>
{
public:
    ObjectAccessProxy(Cache::Ptr cache, std::string typeName)
        : objectWriter_(cache, typeName)
        , objectReader_(cache->getOrCreateHome<DATA, KEY>(typeName), cache)
    { }

    CLASS_TRAITS(ObjectAccessProxy)

public:
    /**
     * Write (key, data) to Cache.
     */
    virtual bool Write(DATA data, KEY key)
    {
        return objectWriter_.Write(data, key);
    }

    /**
     * Write (key, data) to Cache.
     */
    virtual bool WriteAll(const std::map<KEY, DATA> &values)
    {
        return objectWriter_.WriteAll(values);
    }

    /**
     * Dispose of data using a key.
     */
    virtual bool Dispose(KEY key)
    {
        return objectWriter_.Dispose(key);
    }

    /**
     * Clears the data in the cache for all data-types registered.
     *
     * When the cache is cleared (empty) it can still be used without side-effects.
     */
    virtual void DisposeAll()
    {
        objectWriter_.DisposeAll();
    }

    /**
     * @brief ContainsKey
     * @param key
     * @return
     */
    virtual bool ContainsKey(KEY key)
    {
        return objectReader_.containsKey(key);
    }

    /**
     * @brief containsKey
     * @param key
     * @return
     */
    virtual bool containsKey(KEY key)
    {
        return objectReader_.containsKey(key);
    }

    /**
     * @brief findObject
     * @param key
     * @return
     */
    virtual DATA findObject(KEY key)
    {
        return objectReader_.findObject(key);
    }

    /**
     * @brief getObjects
     * @return
     */
    virtual std::map<KEY, DATA> getObjects()
    {
        return objectReader_.getObjects();
    }

    /**
     * @brief getCreatedObjects
     * @return
     */
    virtual std::list<DATA> getCreatedObjects()
    {
        return objectReader_.getCreatedObjects();
    }

    /**
     * @brief getModifiedObjects
     * @return
     */
    virtual std::list<DATA> getModifiedObjects()
    {
        return objectReader_.getModifiedObjects();
    }

    /**
     * @brief getDeletedObjects
     * @return
     */
    virtual std::list<DATA> getDeletedObjects()
    {
        return objectReader_.getDeletedObjects();
    }

private:
    ObjectWriterProxy<DATA, KEY> objectWriter_;
    ObjectReaderStub<DATA, KEY> objectReader_;
};

}

#endif // OBJECTACCESSPROXY_H
