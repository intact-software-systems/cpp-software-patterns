#include "RxConcurrent/ThreadPool/RxThreadPoolFactory.h"

namespace BaseLib { namespace Concurrent
{

RxThreadPoolFactory::RxThreadPoolFactory()
    : Templates::ContextObject<RxThreadPoolFactoryPolicy, RxThreadPoolFactoryState>()
{ }

RxThreadPoolFactory::~RxThreadPoolFactory()
{ }

RxThreadPool::Ptr RxThreadPoolFactory::CommandPool()
{
    return RxThreadPoolFactory::Instance().state().defaultCommandPool();
}

RxThreadPool::Ptr RxThreadPoolFactory::ControllerPool()
{
    return RxThreadPoolFactory::Instance().state().controllerPool();
}

RxThreadPool::Ptr RxThreadPoolFactory::MonitorPool()
{
    return RxThreadPoolFactory::Instance().state().monitorPool();
}

void RxThreadPoolFactory::Initialize()
{
    this->state().Initialize();
}

bool RxThreadPoolFactory::IsInitialized()
{
    return this->state().IsInitialized();
}

}}
