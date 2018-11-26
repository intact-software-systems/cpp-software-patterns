#pragma once

#include "CommonDefines.h"

namespace RxActor {

struct SupervisorData
{
    SupervisorData(DeciderAlgorithm decider)
        : decider_(decider)
    { }

    DeciderAlgorithm decider_;
};

}