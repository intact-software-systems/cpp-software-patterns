#include "MessageWriterPolicy.h"

namespace BaseLib { namespace Concurrent {

MessageWriterPolicy::MessageWriterPolicy(
    Policy::Interval nackSuppressionDuration,
    FlowControllerPolicy flowControllerPolicy,
    Policy::CommDirection kind,
    TaskPolicy heartBeat,
    TaskPolicy nackResponse,
    TaskPolicy data
)
    : nackSuppressionDuration(nackSuppressionDuration)
    , heartBeat(heartBeat)
    , nackResponse(nackResponse)
    , data(data)
    , flowControllerPolicy(flowControllerPolicy)
    , writerPolicy(TaskPolicy::RunForever())
    , commDirection(kind)
    , budget_(Duration::Infinite())
    , reliability_(Policy::ReliabilityKind::Reliable)
    , history_()
    , liveliness_()
    , priority_(Policy::Priority::Default())
    , durability_()
    , destinationOrder_()
    , ownership_()
{ }

MessageWriterPolicy::~MessageWriterPolicy()
{ }

MessageWriterPolicy MessageWriterPolicy::Default()
{
    return MessageWriterPolicy(
        Policy::Interval::RunEveryMs(500),
        FlowControllerPolicy::Default(),
        Policy::CommDirection::Push,
        Concurrent::TaskPolicy::RunForever(Duration::FromMilliseconds(5000)),
        Concurrent::TaskPolicy::RunForever(Duration::FromMilliseconds(5000)),
        Concurrent::TaskPolicy::RunForever(Duration::FromMilliseconds(5000))
    );
}

}}
