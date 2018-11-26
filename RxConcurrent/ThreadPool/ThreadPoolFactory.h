#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/ThreadPool/ThreadPool.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * @brief The ThreadPoolFactory class
 *
 * TODO: Implement with ObjectManager pattern
 */
class DLL_STATE ThreadPoolFactory
    : public Templates::NoCopy<ThreadPoolFactory>
    , public Templates::LockableType<ThreadPoolFactory, Mutex>
    , public Templates::CreateMethod1<ThreadPool::Ptr, std::string>
    , public Templates::CreateMethod2<ThreadPool::Ptr, std::string, Concurrent::ThreadPoolPolicy>
{
    typedef MutexTypeLocker<ThreadPoolFactory> Locker;

    static const std::string defaultName_;
    static const std::string monitorTasksName_;
    static const std::string monitorWorkersName_;

public:
    ThreadPoolFactory();
    virtual ~ThreadPoolFactory();

    static ThreadPoolFactory& Instance();

    ThreadPool::Ptr GetDefault();
    ThreadPool::Ptr GetMonitorWorkers();
    ThreadPool::Ptr GetMonitorTasks();

    virtual ThreadPool::Ptr Create(std::string name);
    virtual ThreadPool::Ptr Create(std::string name, ThreadPoolPolicy policy);

private:
    ThreadPool::Ptr getOrCreateThreadPool(const std::string& name, ThreadPoolPolicy policy = ThreadPoolPolicy::Default());

private:
    typedef BaseLib::Concurrent::ThreadPoolBase::PoolName PoolName;
    typedef std::map<PoolName, ThreadPoolBase::Ptr>       ThreadPools;

private:
    ThreadPools threadPools_;

    static BaseLib::Singleton<ThreadPoolFactory> instance_;
};

}}
