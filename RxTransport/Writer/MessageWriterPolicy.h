#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageWriterPolicy
{
public:
    MessageWriterPolicy(
        Policy::Interval nackSuppressionDuration,
        FlowControllerPolicy flowControllerPolicy,
        Policy::CommDirection kind,
        TaskPolicy heartBeat,
        TaskPolicy nackResponse,
        TaskPolicy data
    );

    virtual ~MessageWriterPolicy();

    static MessageWriterPolicy Default();

    const Policy::Interval nackSuppressionDuration;  // TODO: How to take this into account? (nackResponseDelay = max, nackSuppressionDuration = min) ?
    const TaskPolicy       heartBeat;                // Is this Liveliness?
    const TaskPolicy       nackResponse;
    const TaskPolicy       data;

    const FlowControllerPolicy     flowControllerPolicy;
    const TaskPolicy               writerPolicy;
    const Policy::CommDirection    commDirection;
    const Policy::LatencyBudget    budget_;
    const Policy::Reliability      reliability_;
    const Policy::History          history_;
    const Policy::Liveliness       liveliness_;
    const Policy::Priority         priority_;
    const Policy::Durability       durability_;
    const Policy::DestinationOrder destinationOrder_;
    const Policy::Lifespan         lifespan_;
    const Policy::Ownership        ownership_;
};

}}
