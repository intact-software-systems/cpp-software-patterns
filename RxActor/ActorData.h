#pragma once

#include "RxActor/CommonDefines.h"
#include "RxActor/Processor.h"
#include "RxActor/Supervisor.h"

namespace RxActor {

template <typename T>
class ActorData
{
public:
    ActorData(std::shared_ptr<RxActor::Processor<T>> processor, ChannelHandle handle, ActorId actorId, Supervisor supervisor)
        : processor_(processor)
        , channel_(BaseLib::Concurrent::ChannelFactory::CreateRWChannel<T>(handle))
        , handle_(handle)
        , actorId_(actorId)
        , supervisor_(supervisor)
    { }

    virtual ~ActorData()
    { }

    std::shared_ptr<RxActor::Processor<T>> processor_;
    std::shared_ptr<RWChannel<T>>          channel_;

    const ChannelHandle handle_;
    const ActorId       actorId_;
    const Supervisor    supervisor_;


    // TODO: Access list of children in ActorGraph

    // std::atomic<ActorProxy>   sender_;
    // TODO: Check ActorContext. Parent, etc
};

}
