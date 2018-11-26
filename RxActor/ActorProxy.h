#pragma once

#include "CommonDefines.h"

namespace RxActor {

/**
 * Immutable handle (proxy) to an Actor, which may or may not reside on the local host or inside the same ActorSystem.
 *
 * ActorProxy == ActorProxy
 */
template <typename T>
class ActorProxy
    : public Templates::Key2<ChannelHandle, ActorId>
      , public Templates::ContextDataShared<PublisherChannel<T>>
{
public:
    ActorProxy(ChannelHandle t, ActorId u)
        : Templates::Key2<ChannelHandle, ActorId>(t, u)
        , Templates::ContextDataShared<PublisherChannel<T>>(ChannelFactory::CreatePublisher<T>(t))
    { }

    virtual ~ActorProxy()
    { }

    virtual void Tell(T msg, ActorProxy<T> sender)
    {
        // TODO: Include sender.actorId to message
        this->context()->Next(msg);
    }

    virtual bool IsTerminated() const
    {
        return false;
    }

    virtual void Forward(T msg, ActorBasePtr context)
    {
        // To support forwarding I need a special ActorMessage<T> that includes originalSender and intermediateSender
    }

    virtual ActorPath Path() const
    {
        return Templates::NameDescription();
    }

    static ActorProxy NoSender()
    {
        static std::string   a = "no";
        static ChannelHandle handle{a, a, a};

        return ActorProxy<T>(handle, ActorId::Create());
    }
};

}
