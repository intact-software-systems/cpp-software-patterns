#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/ThreadPool/TaskPolicy.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

/** -------------------------------------------------------------------
Policy support in FlowControllerPolicy:
- bounded speeds
- flow control with congestion control
- Request last n signals to connecting observer
- Pull and push model support in observer
- Max Signal speed (Interval)
- Filter based on content
- Min speed - Deadline - expecting value in configurable intervals. Use thread-pool's monitoring of policy violation and call-back upon interval violations.
- Throughput policy (events per second)
- TODO: Priority on events? Requires weighted fair queueing
- TODO: History is max items, Lifespan is maximum lifetime in queue/history (sliding time window)
------------------------------------------------------------------- */

class DLL_STATE FlowControllerPolicy
{
public:
    FlowControllerPolicy(TaskPolicy policy = TaskPolicy::RunForever());
    virtual ~FlowControllerPolicy();

    const TaskPolicy& Task() const;

    Policy::Throughput    Throughput() const;
    Policy::History       History() const;
    Policy::MaxLimit<int> MaxQueueSize() const;

    // Unused
    Policy::Congestion      Congestion() const;
    Policy::Deadline        Deadline() const;
    Policy::TimeBasedFilter MinSeparation() const;

    static FlowControllerPolicy Default();

private:
    TaskPolicy taskPolicy_;

    // ------------------------------------
    // Flow control
    // ------------------------------------

    Policy::Throughput throughput_;
    Policy::History    history_;
    Policy::Deadline   deadline_;

    Policy::MaxLimit<int>   maxQueueSize_;
    Policy::Congestion      congestion_;
    Policy::TimeBasedFilter minSeparation_;
};

}}
