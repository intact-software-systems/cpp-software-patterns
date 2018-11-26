#include "RxStreamer/RxActionChain.h"

namespace Reactor
{

RxActionChain::RxActionChain(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId, DataAccessControllerPolicy policy)
    : dataAccessController_(RxDataAccessController::Create(masterCacheId, policy, rxContext))
{ }

RxActionChain::~RxActionChain()
{}

// --------------------------------------------------
// Factory functions
// --------------------------------------------------

RxActionChain RxActionChain::Empty()
{
    return RxActionChain(Templates::ActionContextPtr( new Templates::ActionContext()), RxData::CacheDescription::Empty(), DataAccessControllerPolicy::Default());
}

RxActionChain RxActionChain::Create(Templates::ActionContextPtr actionContext, RxData::CacheDescription cacheId)
{
    return RxActionChain(actionContext, cacheId, DataAccessControllerPolicy::Default());
}

RxActionChain RxActionChain::Create(Templates::ActionContextPtr actionContext, RxData::CacheDescription cacheId, DataAccessControllerPolicy policy)
{
    return RxActionChain(actionContext, cacheId, policy);
}

// --------------------------------------------------
// Access state
// --------------------------------------------------

DataAccessController::Ptr RxActionChain::Controller() const
{
    return dataAccessController_.Controller();
}

bool RxActionChain::Subscribe()
{
    return dataAccessController_.Controller()->Subscribe();
}

bool RxActionChain::IsInitialized()
{
    return true;
}

// --------------------------------------------------
// Create and add groups
// --------------------------------------------------

RxDataAccessGroup RxActionChain::Sequential()
{
    return dataAccessController_.Sequential();
}

RxDataAccessGroup RxActionChain::Parallel()
{
    return dataAccessController_.Parallel();
}


}
