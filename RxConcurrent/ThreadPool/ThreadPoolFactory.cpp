#include "RxConcurrent/ThreadPool/ThreadPoolFactory.h"

namespace BaseLib { namespace Concurrent {

const std::string ThreadPoolFactory::defaultName_        = std::string("one-pool");
const std::string ThreadPoolFactory::monitorTasksName_   = std::string("monitor-tasks");
const std::string ThreadPoolFactory::monitorWorkersName_ = std::string("monitor-workers");

BaseLib::Singleton<ThreadPoolFactory> ThreadPoolFactory::instance_;

// -------------------------------------------------------
// private ThreadPoolFactory constructor/destructors
// -------------------------------------------------------
ThreadPoolFactory::ThreadPoolFactory() {}

ThreadPoolFactory::~ThreadPoolFactory()
{ }

ThreadPoolFactory &ThreadPoolFactory::Instance()
{
    return ThreadPoolFactory::instance_.GetRef();
}

// -------------------------------------------------------
// get default thread pools
// -------------------------------------------------------

ThreadPool::Ptr ThreadPoolFactory::GetDefault()
{
    Locker lock(this);
    return getOrCreateThreadPool(ThreadPoolFactory::defaultName_);
}

ThreadPool::Ptr ThreadPoolFactory::GetMonitorWorkers()
{
    Locker lock(this);
    return getOrCreateThreadPool(ThreadPoolFactory::monitorWorkersName_);
}

ThreadPool::Ptr ThreadPoolFactory::GetMonitorTasks()
{
    Locker lock(this);
    return getOrCreateThreadPool(ThreadPoolFactory::monitorTasksName_);
}

// -------------------------------------------------------
// create thread pools
// -------------------------------------------------------

ThreadPool::Ptr ThreadPoolFactory::Create(std::string name)
{
    Locker lock(this);
    return getOrCreateThreadPool(name);
}

ThreadPool::Ptr ThreadPoolFactory::Create(std::string name, ThreadPoolPolicy policy)
{
    Locker lock(this);
    return getOrCreateThreadPool(name, policy);
}

// -------------------------------------------------------
// private functions
// -------------------------------------------------------

ThreadPool::Ptr ThreadPoolFactory::getOrCreateThreadPool(const std::string& name, ThreadPoolPolicy policy)
{
    ThreadPools::iterator it = threadPools_.find(name);

    if(it == threadPools_.end())
    {
        ThreadPool::Ptr threadPool(new ThreadPool(name, policy));
        threadPool->Start();

        threadPools_.insert(std::pair<PoolName, ThreadPoolBase::Ptr>(name, threadPool));

        return threadPool->GetPtr();
    }
    else
    {
        return std::dynamic_pointer_cast<ThreadPool>(it->second);
    }
}

}}
