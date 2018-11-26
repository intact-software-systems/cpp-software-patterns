#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessController.h"
#include"RxStreamer/RxDataAccessController.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE RxChain
{
public:
    virtual ~RxChain() {}

    virtual bool Subscribe() = 0;
    virtual bool IsInitialized() = 0;

    virtual DataAccessController::Ptr Controller() const = 0;
};

class DLL_STATE RxChainNoOp
        : public RxChain
{
public:
    virtual ~RxChainNoOp() {}

    virtual bool Subscribe() { return false; }
    virtual bool IsInitialized() { return false; }
    virtual DataAccessController::Ptr Controller() const { return DataAccessController::Ptr();  }
};

class DLL_STATE RxActionChain
        : public RxChain
{
public:
    RxActionChain(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId, DataAccessControllerPolicy policy);
    virtual ~RxActionChain();

    CLASS_TRAITS(RxActionChain)

    // --------------------------------------------------
    // Factory functions
    // --------------------------------------------------

    static RxActionChain Empty();
    static RxActionChain Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId);
    static RxActionChain Create(Templates::ActionContextPtr actionContext, RxData::CacheDescription cacheId, DataAccessControllerPolicy policy);

    // --------------------------------------------------
    // Interface RxChain
    // --------------------------------------------------

    virtual DataAccessController::Ptr Controller() const;

    virtual bool Subscribe();
    virtual bool IsInitialized();

    // --------------------------------------------------
    // Create and add groups
    // --------------------------------------------------

    RxDataAccessGroup Sequential();
    RxDataAccessGroup Parallel();

private:
    RxDataAccessController dataAccessController_;
};

}
