#ifndef RxData_Cache_ObjectReaderProxy_h_IsIncluded
#define RxData_Cache_ObjectReaderProxy_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/CacheBase.h"
#include"RxData/cache/interfaces/ObjectLoader.h"
#include"RxData/cache/interfaces/ObjectReader.h"
#include"RxData/cache/interfaces/ObjectWriter.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * 
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA, typename KEY>
class ObjectReaderProxy : public ObjectReader<DATA, KEY>
{
private:
	/**
	 * The source of cached data.
	 */
	typename ObjectReader<DATA, KEY>::Ptr objectReader;
	
	/**
	 * The cache to write loaded data.
	 */
	typename ObjectWriter<DATA, KEY>::Ptr objectWriter;
	
	/**
	 * The data-source to locate objects to cache. 
	 */
    typename ObjectLoader<DATA, KEY>::Ptr objectLoader;

public:
	/**
	 * 
	 * @param objectReader
	 * @param objectWriter
	 * @param objectLoader
	 */
	ObjectReaderProxy(typename ObjectReader<DATA, KEY>::Ptr objectReader,
			  		  typename ObjectWriter<DATA, KEY>::Ptr objectWriter,
					  typename ObjectLoader<DATA, KEY>::Ptr objectLoader)
        : objectReader(objectReader)
        , objectWriter(objectWriter)
        , objectLoader(objectLoader)
	{ }

    /**
     * @brief containsKey
     * @param key
     * @return
     */
    virtual bool containsKey(KEY key)
    {
        return objectReader->containsKey(key);
    }

	/**
	 * Searches for object in ObjectReader with key in local cache. 
	 * 
	 * If found, it returns immediately.
	 * 
	 * If not found then it calls the ObjectLoader to load the data-object with key. 
	 * If successful, the data-object is written to cache using the ObjectWriter, and 
	 * finally returned. 
	 */
	virtual DATA findObject(KEY key)
	{
		// -----------------------------------
		// See if ObjectReader (cache?) has object 
		// -----------------------------------
		if(objectReader->containsKey(key))
		{
			// -- debug --
			IDEBUG() << "Cache hit for key" << key;
			// -- debug --

			return objectReader->findObject(key);
		}

		// ------------------------------------------------------
		// Add data to cache if found by object loader
		// ------------------------------------------------------
		if(objectLoader->containsKey(key))
		{
            // ------------------------------------------------------
            // ObjectLoader wraps a data-source which gets the data.
            //
            // Examples:
            // - ProductionPlanQueries returning SensorSet
            // - A service accessing remote ProductionPlanQueries
            // ------------------------------------------------------
            DATA data = objectLoader->loadObject(key);

            // ------------------------------------------------------
            // Add data to cache if found by object loader
            // ------------------------------------------------------
			bool isWritten = objectWriter->Write(data, key);

			// -- debug --
			ASSERT(isWritten);
			IDEBUG() << "Cache writing for key" << key;
			// -- debug --			

            return data;
        }
		else
		{
			// -- debug --
			IWARNING() << "Load object miss for key " << key;
			// -- debug --

            return DATA();
        }
	}

	/**
	 * Retrieves all objects in cache.
	 */
	virtual std::map<KEY, DATA> getObjects()
	{
		return objectReader->getObjects();
	}

	/**
	 * Retrieves all created objects in cache.
	 */
	virtual std::list<DATA> getCreatedObjects()
	{
		return objectReader->getCreatedObjects();
	}

	/**
	 * Retrieves all modified objects in cache.
	 */
	virtual std::list<DATA> getModifiedObjects()
	{
		return objectReader->getModifiedObjects();
	}

	/**
	 * Retrieves all deleted objects in cache.
	 */
	virtual std::list<DATA> getDeletedObjects()
	{
		return objectReader->getDeletedObjects();
	}	
};

}

#endif


