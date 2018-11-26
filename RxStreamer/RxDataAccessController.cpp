#include "RxStreamer/ReactorFactory.h"
#include "RxStreamer/RxDataAccessController.h"

namespace Reactor
{

RxDataAccessController::RxDataAccessController(
        RxData::CacheDescription description,
        DataAccessControllerPolicy policy,
        Templates::ActionContextPtr actionContext)
    : actionContext_(actionContext)
    , masterCacheId_(description)
    , chain_()
{
    dataAccessController_ = ReactorFactory::Instance().Create(policy, chain_, description);
}

RxDataAccessController::~RxDataAccessController()
{ }

Templates::ActionContextPtr RxDataAccessController::ActionContext() const
{
    return actionContext_;
}

DataAccessController::Ptr RxDataAccessController::Controller() const
{
    return dataAccessController_;
}

RxData::CacheDescription RxDataAccessController::CacheId() const
{
    return masterCacheId_;
}

DataAccessGroupChain RxDataAccessController::Chain() const
{
    return chain_;
}

RxDataAccessGroup RxDataAccessController::Sequential()
{
    RxDataAccessGroup group = RxDataAccessGroup::Sequential(actionContext_);
    chain_.Group().push_back(group.Group());

    return group;
}

RxDataAccessGroup RxDataAccessController::Parallel()
{
    RxDataAccessGroup group = RxDataAccessGroup::Parallel(actionContext_);
    chain_.Group().push_back(group.Group());

    return group;
}

// --------------------------------------------------
// Factory functions
// --------------------------------------------------

RxDataAccessController RxDataAccessController::Create(
        RxData::CacheDescription masterCacheId,
        DataAccessControllerPolicy policy,
        Templates::ActionContextPtr actionContext)
{
    return RxDataAccessController(masterCacheId, policy, actionContext);
}

}
