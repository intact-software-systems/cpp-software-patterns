#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/ReactorFactory.h"
#include"RxStreamer/DataAccessController.h"
#include"RxStreamer/DataAccessGroupChain.h"
#include"RxStreamer/RxDataAccessGroup.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE RxDataAccessController
{
public:
    RxDataAccessController(
            RxData::CacheDescription description,
            DataAccessControllerPolicy policy,
            Templates::ActionContextPtr actionContext);

    virtual ~RxDataAccessController();

    // --------------------------------------------------
    // Access state
    // --------------------------------------------------

    Templates::ActionContextPtr ActionContext() const;

    DataAccessController::Ptr Controller() const;

    RxData::CacheDescription CacheId() const;

    DataAccessGroupChain Chain() const;

    RxDataAccessGroup Sequential();
    RxDataAccessGroup Parallel();

    // --------------------------------------------------
    // Factory functions
    // --------------------------------------------------

    static RxDataAccessController Create(
            RxData::CacheDescription masterCacheId,
            DataAccessControllerPolicy policy,
            Templates::ActionContextPtr actionContext);

private:
    const Templates::ActionContextPtr actionContext_;
    const RxData::CacheDescription      masterCacheId_;

    DataAccessGroupChain                chain_;

    DataAccessController::Ptr dataAccessController_;
};

}
