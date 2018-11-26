#include "RxStreamer/AccessFactory.h"

namespace Reactor
{

std::shared_ptr<DataAccessController> AccessFactory::Create(DataAccessControllerPolicy policy, DataAccessGroupChain chain, RxData::CacheDescription cacheDescription)
{
    std::shared_ptr<DataAccessController> controller(new DataAccessController(policy, chain, RxThreadPoolFactory::ControllerPool()));

    // TODO: Write to cache

    return controller;
}


}