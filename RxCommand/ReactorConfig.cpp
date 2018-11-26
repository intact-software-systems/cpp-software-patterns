#include"RxCommand/ReactorConfig.h"

namespace Reactor
{

ReactorConfig::CommandControllerHome ReactorConfig::GetCommandControllerHome()
{
    return ReactorConfig::ReactorCache()->getOrCreateHome<Reactor::CommandController::Ptr, Reactor::CommandController::Ptr>(ReactorConfig::CommandControllerTypeName());
}

ReactorConfig::CommandControllerAccess ReactorConfig::GetCommandControllerAccess()
{
    return CommandControllerAccess(ReactorConfig::ReactorCache(), ReactorConfig::CommandControllerTypeName());
}

std::string ReactorConfig::CommandControllerTypeName()
{
    return BaseLib::Utility::GetTypeName<Reactor::CommandController::Ptr>();
}

RxData::CacheDescription ReactorConfig::ReactorCacheDescription()
{
    std::stringstream name;
    name << "ReactorCache.domain.default";

    return RxData::CacheDescription(name.str());
}

RxData::Cache::Ptr ReactorConfig::ReactorCache()
{
    return RxData::CacheFactory::Instance().getOrCreateCache(ReactorConfig::ReactorCacheDescription());
}

}
