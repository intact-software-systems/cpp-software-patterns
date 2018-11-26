#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageReaderPolicy
{
public:
    MessageReaderPolicy(
        Policy::Interval heartBeatSuppressionDuration,
        bool expectsInlineQos,
        FlowControllerPolicy flowControllerPolicy,
        Policy::CommDirection kind,
        TaskPolicy requestTask,
        TaskPolicy heartBeatResponse
    );

    virtual ~MessageReaderPolicy();

    static MessageReaderPolicy Default();

    const Policy::Interval heartBeatSuppressionDuration;  // ignore HEARTBEATs that arrive too soon after previous HEARTBEAT
    const TaskPolicy       heartBeatResponseTask;          // delay sending of a positive/negative ack
    const TaskPolicy       requestTask;
    const bool             expectsInlineQos;

    const FlowControllerPolicy     flowControllerPolicy;
    const TaskPolicy               readerPolicy;
    const Policy::CommDirection    commDirection;
    const Policy::LatencyBudget    latencyBudget;
    const Policy::Reliability      reliability;
    const Policy::History          history_;
    const Policy::Liveliness       liveliness_;
    const Policy::Priority         priority_;
    const Policy::Durability       durability_;
    const Policy::DestinationOrder destinationOrder_;
    const Policy::Lifespan         lifespan_;
    const Policy::Ownership        ownership_;
};

}}
