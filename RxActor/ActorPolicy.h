#pragma once

#include "Supervisor.h"

namespace RxActor {

struct ActorPolicy
{
    // MessageChannel policies
    SupervisorPolicy policy;

    // Lifetime
    // MaxNumMessagesToProcess == Attempt in TaskPolicy;
    TaskPolicy taskPolicy;
};

}
