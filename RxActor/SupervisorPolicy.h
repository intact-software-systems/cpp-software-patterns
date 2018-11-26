#pragma once

#include "CommonDefines.h"

namespace RxActor {

struct SupervisorPolicy
{
    SupervisorPolicy()
    { }

    SupervisorStrategy strategy_  = SupervisorStrategy::AllForOne;
    Policy::Criterion  criterion_ = Policy::Criterion::All();
};

}


