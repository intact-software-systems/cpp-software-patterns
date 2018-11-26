#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

/** -------------------------------------------------------------------
Policy support in observer:
- Request last n signals to connecting observer
- Pull and push model support in observer (History)
- Max Signal speed (Interval)
- Filter based on content
- Min speed - Deadline - expecting value in configurable intervals. Use thread-pool's monitoring of policy violation and call-back upon interval violations.
- Content filter not here, In a config perhaps
------------------------------------------------------------------- */
class DLL_STATE ObserverPolicy
{
public:
    ObserverPolicy();
    virtual ~ObserverPolicy();

    Policy::Replay Replay() const;
    Policy::Deadline Deadline() const;
    Policy::TimeBasedFilter MinSeparation() const;

    FlowControllerPolicy FlowPolicy() const;

    static ObserverPolicy Default()
    {
        return ObserverPolicy();
    }

private:
    Policy::Replay          replay_;
    Policy::Deadline        deadline_;
    Policy::TimeBasedFilter minSeparation_;

    FlowControllerPolicy    flowPolicy_;
};

}}
