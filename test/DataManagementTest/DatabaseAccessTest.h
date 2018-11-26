#ifndef DATABASEACCESSTEST_H
#define DATABASEACCESSTEST_H

#include "BaseLib/IncludeLibs.h"
#include "RxData/IncludeLibs.h"
#include "NetworkLib/IncludeLibs.h"


namespace DataManagementTest
{

/**
 * Any data serialized from database to object
 *
 * The data is typically already defined and there is no need to modify it.
 *
 * @author KVik
 *
 */
class SomeData
{
private:
    int key;

public:
    SomeData(int key = 0)
    {
        this->key = key;
    }

    int getKey()
    {
        return key;
    }

    bool operator==(const SomeData &other) const
    {
        return key == other.key;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const SomeData &data)
    {
        ostr << TYPE_NAME(data) << "(key:" << data.key << ")";
        return ostr;
    }
};

/**
 * @brief The DatabaseAccessTest class
 */
class DatabaseAccessTest
{
private:
	DataManagement::Cache::ObjectReaderStub<SomeData, int>::Ptr  cacheReader;
	DataManagement::Cache::ObjectWriterProxy<SomeData, int>::Ptr cacheWriter;
	DataManagement::Cache::ObjectReaderProxy<SomeData, int>::Ptr dataAccess;

public:
    /**
     * @brief DatabaseAccessTest
     */
	DatabaseAccessTest();
    virtual ~DatabaseAccessTest();

    /**
     * @brief initalize
     */
	void initalize();

	/**
	 *
	 */
	void aSimpleSequentialDatabaseTest();

	/**
	 * One CacheWriter writes to cache.
	 * One CacheReader reads from cache.
	 *
	 * One DataAccessObject reads/writes to cache and loads data from ObjectLoader (data-base).
	 */
	void aConcurrentDatabaseTest();

	/**
	 * Access data SomeData:
	 *
	 * - If data is found in Cache, it returns the data.
	 * - If not found in cache, an ObjectLoader is used to load data
	 * - If successfully loaded data from ObjectLoader, cache the data and return.
	 *
	 * @author KVik
	 */
	class DataAccessThread : public Runnable
	{
	private:
		DataManagement::Cache::ObjectReader<SomeData, int>::Ptr dataAccess;

	public:
		DataAccessThread(DataManagement::Cache::ObjectReader<SomeData, int>::Ptr dataAccess)
		{
			this->dataAccess = dataAccess;
		}

		virtual void run()
		{
			for(int i = 800; i < 1300; i++)
			{
				SomeData someData = dataAccess->findObject(i);

				// -- debug --
				//ASSERT(someData);
				// -- debug --
			}
		}
	};

	/**
	 * Writes data to cache.
	 *
	 * @author KVik
	 *
	 */
	class CacheWriterThread : public Runnable
	{
	private:
		DataManagement::Cache::ObjectWriter<SomeData, int>::Ptr cacheWriter;

	public:
		CacheWriterThread(DataManagement::Cache::ObjectWriter<SomeData, int>::Ptr cacheWriter)
		{
			this->cacheWriter = cacheWriter;
		}

		virtual void run()
		{
			for(int i = 0; i < 1000; i++)
			{
				bool isWritten = cacheWriter->Write(SomeData(i), i);

				// -- debug --
				ASSERT(isWritten == true);
				// -- debug --
			}
		}
	};

	/**
	 * Reads data from cache.
	 *
	 * @author KVik
	 *
	 */
	class CacheReaderThread : public Runnable
	{
	private:
		DataManagement::Cache::ObjectReader<SomeData, int>::Ptr cacheReader;

	public:
		CacheReaderThread(DataManagement::Cache::ObjectReader<SomeData, int>::Ptr cacheReader)
		{
			this->cacheReader = cacheReader;
		}

		virtual void run()
		{
			for(int i = 0; i < 1500; i++)
			{
				SomeData someData = cacheReader->findObject(i);

				//if(someData == null)
				//{
					// -- debug --
					//assert(i >= 1300);
					// -- debug --
				//}
			}
		}
	};

    /**
	 * The main interface to be implemented.
	 *
	 * @author KVik
	 *
	 */
	class DataBase
	{
    public:
		virtual SomeData getData(int key) = 0;
	};

	/**
	 * Loads data from a DatabaseAccess object.
	 *
	 * The DatabaseAccess may be anything. For example, a Spring SQL Wrapper, a Java Remote object, etc.
	 *
	 * A specialized ObjectLoader must be implemented in the code.
	 *
	 * @author KVik
	 *
	 * @param <DATA>
	 */
    template <typename DATA, typename KEY>
	class DatabaseLoader : public DataManagement::Cache::ObjectLoader<DATA, KEY>
	{
	private:
		DataBase *database_;

	public:
		DatabaseLoader(DataBase *database)
		{
			this->database_ = database;
		}

        CLASS_TRAITS(DatabaseLoader)

        virtual bool containsKey(KEY key)
        {
            return false;
        }

		virtual DATA loadObject(KEY key)
        {
			return database_->getData(key);
		}

		virtual std::map<KEY, DATA> loadAll()
        {
            return std::map<KEY, DATA>();
		}

		virtual std::map<KEY, DATA> loadModified()
        {
			return std::map<KEY, DATA>();
		}

		virtual bool isUpdated()
        {
			return false;
		}
	};


    /**
	 * Client side: A data-base interface as a Remote Java RMI object.
	 *
	 * @author KVik
	 *
	 */
	class DatabaseRemoteAccessInterface : public DataBase // public Remote
	{
	public:
		virtual SomeData getData(int key) = 0;
	};

	/**
	 * Server side: has a dataAccess object that is a wrapper around (Cache, DataBae).
	 *
	 * @author KVik
	 *
	 */
	class DatabaseRemoteAccess : public DatabaseRemoteAccessInterface
	{
	private:
		DataManagement::Cache::ObjectReaderProxy<SomeData, int>::Ptr dataAccess;

	public:
		DatabaseRemoteAccess(DataManagement::Cache::ObjectReaderProxy<SomeData, int>::Ptr dataAccess)
		{
			this->dataAccess = dataAccess;
		}

		/**
		 * Similar to planQueries.getSensorSettingsAndStuff(acquisitionRecordId);
		 *
		 * @param key
		 * @return
		 */
		SomeData getData(int key)
		{
			return dataAccess->findObject(key);
		}
	};

	/**
	 * A data-base access proxy.
	 *
	 * In ASM this may already exist, for example, ProductionPlanQueries or any data-access or data-generation object.
	 *
	 * @author KVik
	 *
	 */
	class DatabaseAccess : public DataBase
	{
	public:
		DatabaseAccess() { }

		/**
		 * Similar to planQueries.getSensorSettingsAndStuff(acquisitionRecordId);
		 *
		 * @param key
		 * @return
		 */
		virtual SomeData getData(int key)
		{
			return SomeData(key);
		}
	};
};

}

#endif // DATABASEACCESSTEST_H
