#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Export.h"

namespace Reactor {

/**
 * @brief The SocketCondition class
 *
 * TODO: Create this in AsynchScheduler and pass to SyncScheduler through OnNext(condition)
 */
class TransitionCondition
    : public BaseLib::Concurrent::GuardCondition
{
public:
    TransitionCondition()
        : BaseLib::Concurrent::GuardCondition()
    { }

    virtual ~TransitionCondition()
    { }

    CLASS_TRAITS(TransitionCondition)

    /**
     *  Input that triggers a transition.
     */
    bool InputTransitionTrigger(Templates::IsTriggeredMethod::Ptr method)
    {
        SetTriggerValue(method->IsTriggered());
        return method->IsTriggered();
    }

    //virtual void UpdateTrigger(T)
    //{ }
};

}
