#include"RxStreamer/ReactorFactory.h"
#include"RxStreamer/DataAccessController.h"

namespace Reactor
{

//ReactorConfig::CommandControllerAccess FactoryState::executors() const
//{
//    return ReactorConfig::GetCommandControllerAccess();
//}

std::shared_ptr<DataAccessController> ReactorFactory::Create(DataAccessControllerPolicy policy, DataAccessGroupChain chain, RxData::CacheDescription cacheDescription)
{
    DataAccessController::Ptr controller(new DataAccessController(policy, chain, RxThreadPoolFactory::ControllerPool()));

    // TODO: Write to cache

    return controller;
}

}
