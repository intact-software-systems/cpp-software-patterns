#include "BaseLib/IncludeLibs.h"
#include "RxData/IncludeLibs.h"
#include "NetworkLib/IncludeLibs.h"

#include"DatabaseAccessTest.h"

using namespace BaseLib;
using namespace BaseLib::Collection;
using namespace BaseLib::Concurrent;
using namespace BaseLib::Templates;
using namespace NetworkLib;

//using namespace DataManagement;
//using namespace DataManagement::Cache;
//using namespace DataManagement::Machine;

namespace DataManagementTest {

/**
 * @brief The ValidatorResult class has uniform access to validation result.
 */
class ValidatorResult
{
public:
    ValidatorResult() : isValid_(false) {}
    ValidatorResult(bool valid) : isValid_(valid) {}

    ~ValidatorResult() {}

    bool IsValid() const { return isValid_; }

    void SetValid(bool valid) { isValid_ = valid; }

private:
    bool isValid_;
};

/**
 * @brief The TestContextObject class holds the context (all state) for a TestObject
 */
class TestContextObject : public ContextObject<NullConfig, NullTransientState, NullCriticalState>
                        , public IsValidMethod<ValidatorResult>
{
public:
    TestContextObject() {}
    ~TestContextObject() {}

    virtual ValidatorResult IsValid() const
    {
        // Validate context object according to its configuration and current state.
        return ValidatorResult(true);
    }

    static TestContextObject Default()
    {
        return TestContextObject();
    }
};


/**
 * @brief The TestObject class holds the logic to be performed on the TestContextObject
 */
class TestObject : public IsValidMethod<ValidatorResult>
{
public:
    TestObject(TestContextObject context = TestContextObject::Default())
        : context_(context)
    {}
    ~TestObject() {}

    virtual ValidatorResult IsValid() const
    {
        return context_.IsValid();
    }

    bool DoSomething()
    {
        /*
        if(context_.GetConfig().LeastRecentlyUsed())
        {
            context_.GetTransientState().UpdateState();
            context_.GetCriticalState().Insert(anObject);
        }*/

        return false;
    }

private:
    TestContextObject context_;
};


void testCache()
{
//    CacheFactory::Instance().createCacheWriter<

    DataManagement::Cache::CacheDescription description("testCache");

    DataManagement::Cache::Cache::Ptr cache(new DataManagement::Cache::Cache(description, true));

}

}

int main(int argc, char *argv[])
{
    //DataManagementTest::testCache();
    //CacheFactory::Instance().createCacheWriter<Subscriber::Ptr, InstanceHandle>(description, TYPE_NAME(Subscriber));
    //cache_->createAccess(DataManagement::Cache::CacheUsage::READ_WRITE);
    //DataManagement::Cache::CacheFactory::Instance().createDataAccess()

    DataManagementTest::DatabaseAccessTest databaseAccessTest; // = new DatabaseAccessTest();

    databaseAccessTest.aSimpleSequentialDatabaseTest();
    databaseAccessTest.aConcurrentDatabaseTest();

    return 0;
}
