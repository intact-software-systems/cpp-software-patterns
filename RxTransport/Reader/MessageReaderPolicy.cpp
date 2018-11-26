#include "MessageReaderPolicy.h"

namespace BaseLib { namespace Concurrent {

MessageReaderPolicy::MessageReaderPolicy(
    Policy::Interval heartBeatSuppressionDuration,
    bool expectsInlineQos,
    FlowControllerPolicy flowControllerPolicy,
    Policy::CommDirection kind,
    TaskPolicy requestTask,
    TaskPolicy heartBeatResponse
)
    : heartBeatSuppressionDuration(heartBeatSuppressionDuration)
    , expectsInlineQos(expectsInlineQos)
    , flowControllerPolicy(flowControllerPolicy)
    , commDirection(kind)
    , requestTask(requestTask)
    , heartBeatResponseTask(heartBeatResponse)
    , reliability(Policy::ReliabilityKind::Reliable)
{ }

MessageReaderPolicy::~MessageReaderPolicy()
{ }

MessageReaderPolicy MessageReaderPolicy::Default()
{
    return MessageReaderPolicy(
        Policy::Interval::RunEveryMs(500),
        true,
        Concurrent::FlowControllerPolicy::Default(),
        Policy::CommDirection::Push,
        Concurrent::TaskPolicy::RunForever(Duration::FromMilliseconds(5000)),
        Concurrent::TaskPolicy::RunForever(Duration::FromMilliseconds(5000))
    );
}

}}
