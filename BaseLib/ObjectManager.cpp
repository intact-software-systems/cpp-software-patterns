#include "BaseLib/ObjectManager.h"
#include "BaseLib/ObjectBase.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/MutexLocker.h"
#include "BaseLib/Exception.h"
#include "BaseLib/Debug.h"

using namespace std;

namespace BaseLib
{

ObjectManager* ObjectManager::objectManager_ = NULL;

// TODO: Deadlock detected when starting thread!
//  -> ObjectBase::ObjectBase() -> getOrCreateObjectManager -> start() -> getOrCreateThreadManager() -> getOrCreateObjectManager().....
ObjectManager::ObjectManager(std::string name)
    : Thread(name, false)
    , managerMutex_(MutexPolicy::No())
{
}

ObjectManager::~ObjectManager()
{
}

ObjectManager* ObjectManager::createObjectManager() throw()
{
    static Mutex staticMutex(MutexPolicy::No());
    MutexLocker lock(&staticMutex);

    if(ObjectManager::objectManager_  == NULL)
    {
        ObjectManager::objectManager_ = new ObjectManager();
        ObjectManager::objectManager_->start();
    }
    else
        IWARNING() << "ObjectManager already initialized!";

    return ObjectManager::objectManager_;
}

ObjectManager* ObjectManager::getObjectManager()
{
    if(ObjectManager::objectManager_ == NULL)
    {
        throw FatalException("ObjectManager not initialized!");
    }

    return ObjectManager::objectManager_;
}

// TODO: All ObjectBase implementations inherit a type of smart pointers that do not delete
// the Object when ref count is zero, instead ObjectManager deletes
// - Weak pointer implementation

void ObjectManager::run()
{
    IDEBUG() << "running";

    while(true)
    {
        msleep(1000);
    }
}

bool ObjectManager::AddObject(ObjectBase *base) throw()
{
    MutexLocker lock(&managerMutex_);

    if(setObjectBase_.count(base) > 0)
    {
        IDEBUG() << "ERROR! Object registered twice! Memory leaks? ";
        return false;
    }

    setObjectBase_.insert(base);
    return true;
}

bool ObjectManager::RemoveObject(ObjectBase *base) throw()
{
    MutexLocker lock(&managerMutex_);

    if(setObjectBase_.count(base) <= 0)
    {
        IDEBUG() << "ERROR! Object not registered? ";
        return false;
    }

    setObjectBase_.erase(base);
    return true;
}

ObjectBase* ObjectManager::GetObject(const string &objectName) const throw()
{
    MutexLocker lock(&managerMutex_);

    ObjectBase *objectBase = NULL;

    for(SetObjectBase::const_iterator it = setObjectBase_.begin(), it_end = setObjectBase_.end(); it != it_end; ++it)
    {
        ObjectBase *obj = *it;
        ASSERT(obj != NULL);
        if(obj->Name() != objectName) continue;

        objectBase = obj;
        break;
    }

    if(objectBase == NULL)
        IDEBUG() << "WARNING! ObjectName " << objectName << " not found. Is it registered? ";

    return objectBase;
}

}; // namespace BaseLib

