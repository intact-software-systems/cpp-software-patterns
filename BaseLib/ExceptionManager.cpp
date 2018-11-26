#include "BaseLib/ExceptionManager.h"
#include "BaseLib/Exception.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/MutexLocker.h"

using namespace std;

namespace BaseLib
{

ExceptionManager* ExceptionManager::exceptionManager_ = NULL;

ExceptionManager::ExceptionManager(std::string name)
    : Thread(name)
    , ObjectBase(name)
{

}

ExceptionManager::~ExceptionManager()
{
}

void ExceptionManager::run()
{
    IDEBUG() << "running";
    while(true)
    {
        msleep(1000);
    }
}

ExceptionManager* ExceptionManager::getOrCreateExceptionManager()
{
    static Mutex staticMutex;
    MutexLocker lock(&staticMutex);

    if(ExceptionManager::exceptionManager_ == NULL)
        ExceptionManager::exceptionManager_ = new ExceptionManager();

    return ExceptionManager::exceptionManager_;
}

bool ExceptionManager::AddObject(Exception *base)
{
    MutexLocker lock(&managerMutex_);

    if(setException_.count(base) >= 1)
    {
        IDEBUG() << "ERROR! Exception already present in ExceptionManager!";
        return false;
    }

    setException_.insert(base);
    return true;
}

bool ExceptionManager::RemoveObject(Exception *base)
{
    MutexLocker lock(&managerMutex_);

    if(setException_.count(base) <= 0)
    {
        IDEBUG() << "ERROR! already remomved from ExceptionManager!";
        return false;
    }

    setException_.erase(base);
    return true;
}

}


