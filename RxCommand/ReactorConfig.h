#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandController.h"
#include"RxCommand/Export.h"

namespace Reactor
{

class DLL_STATE ReactorConfig
{
public:
    typedef RxData::ObjectAccessProxy<CommandController::Ptr, CommandController::Ptr>  CommandControllerAccess;
    typedef RxData::ObjectHome<CommandController::Ptr, CommandController::Ptr>::Ptr CommandControllerHome;

public:
    static CommandControllerHome   GetCommandControllerHome();
    static CommandControllerAccess GetCommandControllerAccess();

//    static RxData::CacheDescription GetCommandFactoryCacheId() {
//        return CacheConfig.createMasterCacheId(CommandFactory.class);
//    }

//    static DataCacheId getCommandControllerCacheId(MasterCacheId masterCacheId) {
//        return CacheConfig.createDataCacheId(com.telenor.consumer.rx.core.command.api.controller.CommandController.class, masterCacheId);
//    }

private:
    static RxData::Cache::Ptr ReactorCache();
    static std::string CommandControllerTypeName();
    static RxData::CacheDescription ReactorCacheDescription();
};

}
