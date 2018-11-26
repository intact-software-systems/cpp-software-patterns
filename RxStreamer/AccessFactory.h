#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessGroupChain.h"
#include"RxStreamer/DataAccessController.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class AccessFactoryPolicy
{};

class AccessFactoryState
{};

class DLL_STATE AccessFactory
        : public Templates::FactorySingleton<AccessFactory>
        , public Templates::ContextObject<AccessFactoryPolicy, AccessFactoryState>
{
public:
    std::shared_ptr<DataAccessController> Create(DataAccessControllerPolicy policy, DataAccessGroupChain chain, RxData::CacheDescription cacheDescription);
};

}
