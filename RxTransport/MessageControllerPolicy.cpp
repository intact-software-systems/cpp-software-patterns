#include "MessageControllerPolicy.h"

namespace BaseLib { namespace Concurrent {

MessageControllerPolicy::MessageControllerPolicy(
    const FlowControllerPolicy& defaultFlowPolicy,
    const MessageWriterPolicy& defaultMessageWriterPolicy,
    const MessageReaderPolicy& defaultMessageReaderPolicy
)
    : defaultFlowPolicy_(defaultFlowPolicy)
    , defaultMessageWriterPolicy_(defaultMessageWriterPolicy)
    , defaultMessageReaderPolicy_(defaultMessageReaderPolicy)
{ }

MessageControllerPolicy MessageControllerPolicy::RequestReply()
{
    return MessageControllerPolicy(FlowControllerPolicy::Default(), MessageWriterPolicy::Default(), MessageReaderPolicy::Default());
}

MessageControllerPolicy MessageControllerPolicy::FireAndForget()
{
    return MessageControllerPolicy(FlowControllerPolicy::Default(), MessageWriterPolicy::Default(), MessageReaderPolicy::Default());
}

MessageControllerPolicy MessageControllerPolicy::Multicast()
{
    return MessageControllerPolicy(FlowControllerPolicy::Default(), MessageWriterPolicy::Default(), MessageReaderPolicy::Default());
}

}}
