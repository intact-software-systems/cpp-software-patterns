#pragma once

#include"RxConcurrent/ThreadPool/RxThreadPool.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

class DLL_STATE RxThreadPoolFactoryPolicy
{};

class DLL_STATE RxThreadPoolFactoryState
{
public:
    //RxThreadPoolFactoryPolicy::DefaultRxThreadPoolFactoryCachePolicy cachePolicy)
    RxThreadPoolFactoryState()
    {
        Initialize();
//        this->factoryCacheId = MachineConfig::getRxThreadPoolFactoryCacheId();
//        this->commandControllerCacheId = MachineConfig::getRxThreadPoolCacheId(factoryCacheId);
//        this->cachePolicy = cachePolicy;
    }

//    DataCache<RxThreadPoolId, RxThreadPool> poolcache() {
//        return RxData::CacheFactory::Instance().getOrCreateCache(factoryCacheId, commandControllerCacheId, cachePolicy);
//    }

    RxThreadPool::Ptr defaultCommandPool() { return commandPool_; }
    RxThreadPool::Ptr controllerPool() { return controllerPool_; }
    RxThreadPool::Ptr monitorPool() { return monitorPool_; }

    void Initialize()
    {
        commandPool_ = RxThreadPool::Create(ThreadPoolPolicy::Custom(Policy::MaxLimit<int64>::InclusiveLimitTo(50)), "rx-command");
        controllerPool_ = RxThreadPool::Create(ThreadPoolPolicy::Custom(Policy::MaxLimit<int64>::InclusiveLimitTo(5)), "rx-controller");
        monitorPool_ = RxThreadPool::Create(ThreadPoolPolicy::Custom(Policy::MaxLimit<int64>::InclusiveLimitTo(5)), "rx-monitor");

        commandPool_->Start();
        controllerPool_->Start();
        monitorPool_->Start();
    }

    bool IsInitialized()
    {
        return commandPool_ != nullptr && controllerPool_ != nullptr && monitorPool_ != nullptr;
    }

private:
    /**
     * Default thread pool used by user-defined commands.
     */
    RxThreadPool::Ptr commandPool_;

    /**
     * Default thread pool used by user-defined commands.
     */
    RxThreadPool::Ptr controllerPool_;

    /**
     * Default thread pool used by monitors
     */
    RxThreadPool::Ptr monitorPool_;

    /**
     * Access to cached command controllers
     */
//    MasterCacheId factoryCacheId;
//    DataCacheId commandControllerCacheId;
//    RxThreadPoolFactoryPolicy::DefaultRxThreadPoolFactoryCachePolicy cachePolicy;
};

class DLL_STATE RxThreadPoolFactory
        : Templates::FactorySingleton<RxThreadPoolFactory>
        , Templates::ContextObject<RxThreadPoolFactoryPolicy, RxThreadPoolFactoryState>
{
public:
    RxThreadPoolFactory();
    virtual ~RxThreadPoolFactory();

    static RxThreadPool::Ptr CommandPool();
    static RxThreadPool::Ptr ControllerPool();
    static RxThreadPool::Ptr MonitorPool();

    void Initialize();
    bool IsInitialized();
};

}}
