#ifndef RxData_Cache_ObjectReaderStub_h_IsIncluded
#define RxData_Cache_ObjectReaderStub_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/CacheBase.h"
#include"RxData/cache/interfaces/ObjectLoader.h"
#include"RxData/cache/interfaces/ObjectReader.h"
#include"RxData/cache/object/ObjectHome.h"

#include"RxData/Export.h"
namespace RxData
{

template <typename DATA, typename KEY>
class ObjectReaderStub : public ObjectReader<DATA, KEY>
{
private:
	/**
	 * The data-source to locate objects to cache. 
	 */
	typename ObjectLoader<DATA, KEY>::Ptr objectLoader;
	
	/**
	 * The home of object type DATA
	 */
	typename ObjectHome<DATA, KEY>::Ptr objectHome;
	
	/**
	 * The access point of the cached data.
	 */
	CacheBase::Ptr cacheBase;
	
public:
	/**
	 * ObjectReaderStub that access the cache, and also has an ObjectLoader to load not available data.
	 * 
	 * @param objectReader
	 * @param objectWriter
	 * @param objectLoader
	 */
	ObjectReaderStub(typename ObjectHome<DATA, KEY>::Ptr objectHome, CacheBase::Ptr cacheBase, typename ObjectLoader<DATA, KEY>::Ptr objectLoader)
        : objectLoader(objectLoader)
        , objectHome(objectHome)
        , cacheBase(cacheBase)
    { }
	
	/**
	 * ObjectReaderStub that access the cache, not any data-source.
	 * 
	 * @param objectHome
	 * @param cacheBase
	 */
	ObjectReaderStub(typename ObjectHome<DATA, KEY>::Ptr objectHome, CacheBase::Ptr cacheBase)
        : objectLoader()
        , objectHome(objectHome)
        , cacheBase(cacheBase)
	{ }

    CLASS_TRAITS(ObjectReaderStub)


    /**
     * @brief containsKey
     * @param key
     * @return
     */
    virtual bool containsKey(KEY key)
    {
        return objectHome->containsObject(key, cacheBase);
    }

    /**
     * @brief findObject
     * @param key
     * @return
     */
    virtual DATA findObject(KEY key)
	{
        if(objectHome->containsObject(key, cacheBase))
        {
            return objectHome->findObject(key, cacheBase);
        }

		if(objectLoader)
			return objectLoader->loadObject(key);
		
        return DATA();
	}

    /**
     * @brief getObjects
     * @return
     */
    virtual std::map<KEY, DATA> getObjects()
	{
		return objectHome->getObjects(cacheBase);
	}

    /**
     * @brief getCreatedObjects
     * @return
     */
    virtual std::list<DATA> getCreatedObjects()
	{
		return objectHome->getCreatedObjects(cacheBase);
	}

    /**
     * @brief getModifiedObjects
     * @return
     */
	virtual std::list<DATA> getModifiedObjects()
	{
		return objectHome->getModifiedObjects(cacheBase);
	}

    /**
     * @brief getDeletedObjects
     * @return
     */
	virtual std::list<DATA> getDeletedObjects()
	{
		return objectHome->getDeletedObjects(cacheBase);
	}
};

}

#endif


