#pragma once

#include <functional>
#include "CommonDefines.h"
#include "SupervisorAlgorithms.h"
#include "SupervisorPolicy.h"
#include "SupervisorData.h"

namespace RxActor {


/**
 * int      maxAttempts;
 * Duration withinTimeRange;
 */
class Supervisor
    : public Templates::ContextObjectShared<SupervisorPolicy, SupervisorData, SupervisorStatus>
{
    typedef RxActor::SupervisorAlgorithms SA;

public:
    Supervisor(DeciderAlgorithm decider, SupervisorPolicy* policy);

    virtual ~Supervisor();

    const SupervisorPolicy& Policy() const;

    DeciderAlgorithm     Decider() const;
    Policy::ReactionKind Decide(BaseLib::Exception exception);

    //void ProcessFailure(ActorBasePtr context, ActorProxy child, BaseLib::Exception exception);
};

}
