#ifndef RxData_Cache_CacheFactory_h_IsIncluded
#define RxData_Cache_CacheFactory_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/CacheDescription.h"
#include"RxData/cache/Cache.h"

#include"RxData/cache/interfaces/ObjectLoader.h"

#include"RxData/cache/object/ObjectHome.h"
#include"RxData/cache/object/policy/ObjectHomePolicy.h"
#include"RxData/cache/object/reader/ObjectReaderProxy.h"
#include"RxData/cache/object/reader/ObjectReaderStub.h"
#include"RxData/cache/object/writer/ObjectWriterProxy.h"

#include"RxData/machine/VirtualMachineCache.h"

#include"RxData/Export.h"
namespace RxData
{
/**
 * The singleton instance of this class allows the creation of Cache objects.
 *
 * @author KVik
 */
class DLL_STATE CacheFactory
        : public BaseLib::Templates::FactorySingleton<CacheFactory>
{
public:
    // ------------------------------------------
    // typedefs
    // ------------------------------------------
    typedef IMap<CacheDescription, Cache::Ptr> CacheMap;

    // ------------------------------------------
    // private variables
    // ------------------------------------------

    CacheMap dataMap_;

    // ----------------------------------
    // Constructor
    // ----------------------------------

    CacheFactory()
    { }
    virtual ~CacheFactory()
    { }

    // ------------------------------------------
    // Operations
    // ------------------------------------------

    /**
     * Create a proxy for reading/writing to a cache and a data-source, example, a database.
     *
     * This object can be made many times because it is a state-less decorator (Read: Decorator pattern).
     *
     * @param <DATA>
     * @param databaseLoader
     * @param description
     * @param typeName
     * @param policy
     * @return ObjectReaderProxy<DATA>
     */
    template <typename DATA, typename KEY>
    ObjectReaderProxy<DATA, KEY> createDataAccess(typename ObjectLoader<DATA, KEY>::Ptr databaseLoader, CacheDescription description, std::string typeName)
    {
        return createDataAccessPrivate<DATA, KEY>(databaseLoader, description, typeName, ObjectHomePolicy::Default());
    }

    /**
     * Create a proxy for reading/writing to a cache and a data-source, example, a database.
     *
     * This object can be made many times because it is a state-less decorator (Read: Decorator pattern).
     *
     * @param <DATA>
     * @param databaseLoader
     * @param description
     * @param typeName
     * @param policy
     * @return ObjectReaderProxy<DATA>
     */
    template <typename DATA, typename KEY>
    ObjectReaderProxy<DATA, KEY> createDataAccess(typename ObjectLoader<DATA, KEY>::Ptr databaseLoader, CacheDescription description, std::string typeName, ObjectHomePolicy policy)
    {
        return createDataAccessPrivate<DATA, KEY>(databaseLoader, description, typeName, policy);
    }

private:
    /**
     * Create a proxy for reading/writing to a cache and a data-source, example, a database.
     *
     * This object can be made many times because it is a state-less decorator (Read: Decorator pattern).
     *
     * @param <DATA>
     * @param databaseLoader
     * @param description
     * @param typeName
     * @param policy
     * @return ObjectReaderProxy<DATA>
     */
    template <typename DATA, typename KEY>
    ObjectReaderProxy<DATA, KEY> createDataAccessPrivate(typename ObjectLoader<DATA, KEY>::Ptr databaseLoader, CacheDescription description, std::string typeName, ObjectHomePolicy policy)
    {
        // -----------------------------------------------------------------------
        // Create the Cache object and the objectHome for typeName
        // -----------------------------------------------------------------------
        Cache::Ptr cache = getOrCreateCache(description);
        typename ObjectHome<DATA,KEY>::Ptr objectHome = cache->getOrCreateHome<DATA,KEY>(typeName, policy);

        // -----------------------------------------------------------------------
        // Create the proxies for reading and writing
        //
        // Note: These proxy/stub objects can be made many times because they
        // are essentially stateless decorators (Read: Software patterns).
        // -----------------------------------------------------------------------
        typename ObjectReaderStub<DATA, KEY>::Ptr  cacheReader( new ObjectReaderStub<DATA, KEY>(objectHome, cache) );
        typename ObjectWriterProxy<DATA, KEY>::Ptr cacheWriter( new ObjectWriterProxy<DATA, KEY>(cache, objectHome->getTypeName()) );

        // -----------------------------------------------------------------------
        // This proxy takes care of reading data from cache, if available, or
        // reading from data-base and writing to cache before returning data.
        // -----------------------------------------------------------------------

        return ObjectReaderProxy<DATA, KEY>(cacheReader, cacheWriter, databaseLoader);
    }

public:
    /**
     *
     * Create a proxy for writing to a cache and a data-source, example, a database.
     *
     * This object can be made many times because it is a state-less decorator (Read: Decorator pattern).
     *
     * @param <DATA>
     * @param description
     * @param typeName
     * @param policy
     * @return ObjectWriterProxy<DATA>
     */
    template <typename DATA, typename KEY>
    ObjectWriterProxy<DATA, KEY> createCacheWriter(CacheDescription description, std::string typeName)
    {
        return createCacheWriterPrivate<DATA, KEY>(description, typeName, ObjectHomePolicy::Default());
    }

    /**
     *
     * Create a proxy for writing to a cache and a data-source, example, a database.
     *
     * This object can be made many times because it is a state-less decorator (Read: Decorator pattern).
     *
     * @param <DATA>
     * @param description
     * @param typeName
     * @param policy
     * @return ObjectWriterProxy<DATA>
     */
    template <typename DATA, typename KEY>
    ObjectWriterProxy<DATA, KEY> createCacheWriter(CacheDescription description, std::string typeName, ObjectHomePolicy policy)
    {
        return createCacheWriterPrivate<DATA, KEY>(description, typeName, policy);
    }

private:

    /**
     *
     * Create a proxy for writing to a cache and a data-source, example, a database.
     *
     * This object can be made many times because it is a state-less decorator (Read: Decorator pattern).
     *
     * @param <DATA>
     * @param description
     * @param typeName
     * @param policy
     * @return ObjectWriterProxy<DATA>
     */
    template <typename DATA, typename KEY>
    ObjectWriterProxy<DATA, KEY> createCacheWriterPrivate(CacheDescription description, std::string typeName, ObjectHomePolicy policy)
    {
        // -----------------------------------------------------------------------
        // Create the Cache object and the objectHome for typeName
        // -----------------------------------------------------------------------
        Cache::Ptr cache = getOrCreateCache(description);
        typename ObjectHome<DATA,KEY>::Ptr objectHome = cache->getOrCreateHome<DATA,KEY>(typeName, policy);


        return ObjectWriterProxy<DATA, KEY>(cache, objectHome->getTypeName());
    }

public:
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
    Cache::Ptr getOrCreateCache(CacheDescription description);

    /**
     * To retrieve a Cache based on the name given in the CacheDescription (find_cache_by_name).
     * If the specified name does not identify an existing Cache, a NULL is returned.
     *
     * @param cacheName
     * @return
     */
    Cache::Ptr findCacheByName(std::string cacheName);

    /**
     * To retrieve a Cache based on the CacheDescription.
     *
     * If the specified description does not identify an existing Cache, a NULL is returned.
     *
     * @param description
     * @return
     */
    Cache::Ptr findCacheByDescription(CacheDescription description);

    /**
     * To delete a Cache (delete_cache). This operation releases all the resources allocated to the Cache.
     *
     * @param cache
     */
    void deleteCache(Cache::Ptr cache);

    /**
     * Clears (not removes) all Cache objects created by this CacheFactory.
     */
    void clearAllCaches();

    /**
     * Clears and removes all Cache objects created by this CacheFactory.
     */
    void deleteAllCaches();

    /**
     * Default cache policy
     */
    ObjectHomePolicy defaultCachePolicy() const;
};

}

#endif

