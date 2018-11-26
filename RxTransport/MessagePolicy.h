#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

/** -------------------------------------------------------------------
TODO: Policy support in event/message:
- Deadline/lifetime of message. Ex: "message is only valid a certain time-period, and all readers must receive it by then or the message is discarded."
- RequestReply - message requires reply within time (should I use a future?, how to map up request reply pairs? RequestReplyChannel.h/cpp?
- Reliable or BestEffort - to support transaction style messaging
 ------------------------------------------------------------------- */
class DLL_STATE MessagePolicy
{
public:
    MessagePolicy();
    virtual ~MessagePolicy();

    static MessagePolicy Default();

private:
    TaskPolicy taskPolicy_ = TaskPolicy::RunOnceIn(Duration::FromMilliseconds(500));

    //Policy::LatencyBudget latencyBudget_;
    //Policy::Deadline      deadline_;
    //Policy::DestinationOrderKind ordering_;
    //Policy::Durability durability_;
};

}}
