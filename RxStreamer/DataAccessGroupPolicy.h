#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

// TODO: Support Group Timeout
class DLL_STATE DataAccessGroupPolicy
{
public:
    DataAccessGroupPolicy(Policy::Computation computation);
    virtual ~DataAccessGroupPolicy();

    const Policy::Computation& Computation() const;

    // -----------------------------------------------------------
    // Factory functions
    // -----------------------------------------------------------

    static DataAccessGroupPolicy Sequential();
    static DataAccessGroupPolicy Parallel();

private:
    Policy::Computation computation_;
};

}
