#pragma once

#include <RxTransport/Writer/MessageWriterPolicy.h>
#include <RxTransport/Reader/MessageReaderPolicy.h>

#include "RxTransport/CommonDefines.h"
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageControllerPolicy
{
public:
    MessageControllerPolicy(
        const FlowControllerPolicy& defaultFlowPolicy,
        const MessageWriterPolicy& defaultMessageWriterPolicy,
        const MessageReaderPolicy& defaultMessageReaderPolicy
    );

    static MessageControllerPolicy RequestReply();

    static MessageControllerPolicy FireAndForget();

    static MessageControllerPolicy Multicast();

private:
    FlowControllerPolicy defaultFlowPolicy_;
    MessageWriterPolicy  defaultMessageWriterPolicy_;
    MessageReaderPolicy  defaultMessageReaderPolicy_;
};

}}
