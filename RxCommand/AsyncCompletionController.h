#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandController.h"
#include"RxCommand/CommandControllerSubscription.h"
#include"RxCommand/CommandControllerPolicy.h"
#include"RxCommand/Export.h"

namespace Reactor {

// Monitor async actions and notify command controller upon completion/timeout
class DLL_STATE AsyncCompletionController : public Reactor::details::CommandController
{
public:
    AsyncCompletionController(const CommandControllerPolicy& policy, const RxThreadPool::Ptr& threadPool)
        : CommandController(policy, threadPool)
    { }
};

}
