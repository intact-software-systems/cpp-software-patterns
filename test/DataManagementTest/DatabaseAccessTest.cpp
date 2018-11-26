#include "DatabaseAccessTest.h"

using namespace DataManagement;
using namespace DataManagement::Cache;


namespace DataManagementTest
{

DatabaseAccessTest::DatabaseAccessTest()
{
    initalize();
}

DatabaseAccessTest::~DatabaseAccessTest()
{

}

void DatabaseAccessTest::initalize()
{
    // -----------------------------------------------------------------------
    // Create the data-base loader and attach the actual data-base query object
    // -----------------------------------------------------------------------
    DataBase *databaseAccess = new DatabaseAccess();
    DatabaseLoader<SomeData, int>::Ptr databaseLoader(new DatabaseLoader<SomeData, int>(databaseAccess) );

    // -----------------------------------------------------------------------
    // Create the Cache object and the objectHome for SomeData
    // -----------------------------------------------------------------------
    DataManagement::Cache::Cache::Ptr cache = CacheFactory::Instance().getOrCreateCache(CacheDescription("aCache"));
    ObjectHome<SomeData, int>::Ptr objectHome = cache->getOrCreateHome<SomeData, int>(TYPE_NAME(SomeData));

    // -----------------------------------------------------------------------
    // Create the proxies for reading and writing
    // -----------------------------------------------------------------------
    cacheReader = ObjectReaderStub<SomeData, int>::Ptr( new ObjectReaderStub<SomeData, int>(objectHome, cache));
    cacheWriter = ObjectWriterProxy<SomeData, int>::Ptr( new ObjectWriterProxy<SomeData, int>(cache, objectHome->getTypeName()) );
    dataAccess 	= ObjectReaderProxy<SomeData, int>::Ptr(new ObjectReaderProxy<SomeData, int>(cacheReader, cacheWriter, databaseLoader));
}

void DatabaseAccessTest::aSimpleSequentialDatabaseTest()
{
    for(int i = 0; i < 1000; i++)
    {
        SomeData data(i);

        bool isAdded = cacheWriter->Write(data, i);

        IDEBUG() << "Wrote " << data;

        // -- debug --
        ASSERT(isAdded);
        // -- debug --
    }

    for(int i = 900; i < 1100; i ++)
    {
        if(!dataAccess->containsKey(i))
            continue;

        SomeData data = dataAccess->findObject(i);

        IDEBUG() << "Read " << data;

        // -- debug --
        //assert (data != null);
        // -- debug --
    }
}

void DatabaseAccessTest::aConcurrentDatabaseTest()
{
    // -------------------------------------
    // Start CacheWriterThread - Write
    // -------------------------------------
    CacheWriterThread *cacheWriterThread = new CacheWriterThread(this->cacheWriter);

    BaseLib::Concurrent::ThreadPoolFactory::Instance().GetDefault()->Schedule(cacheWriterThread, TaskPolicy::Default());

    // -------------------------------------
    // Start DataAccessThread - Read/Write
    // -------------------------------------
    DataAccessThread *dataAccessThread = new DataAccessThread(this->dataAccess);

    BaseLib::Concurrent::ThreadPoolFactory::Instance().GetDefault()->Schedule(dataAccessThread, TaskPolicy::Default());

    // -------------------------------------
    // Yes, I know it is ugly!!!
    // -------------------------------------

    Thread::sleep(3);

    // -------------------------------------
    // Start CacheWriterThread - Read
    // -------------------------------------
    CacheReaderThread *cacheReaderThread = new CacheReaderThread(this->cacheReader);

    BaseLib::Concurrent::ThreadPoolFactory::Instance().GetDefault()->Schedule(cacheReaderThread, TaskPolicy::Default());
}

}
