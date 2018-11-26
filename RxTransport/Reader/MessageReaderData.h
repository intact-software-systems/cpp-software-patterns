#pragma once

#include "RxTransport/CommonDefines.h"
#include "RxTransport/MessageCache.h"
#include "RxTransport/Message.h"
#include "MessageWriterProxies.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class MessageReaderData
{
    typedef std::shared_ptr<details::FlowControllerAction<Message<T>>> Flower;

public:
    MessageReaderData(const ChannelHandle& handle, const ReaderId& id, const FlowControllerPolicy& policy)
        : flowController_(new details::FlowControllerAction<Message<T>>(policy))
        , handle_(handle)
        , id_(id)
        , proxies_()
        , cache_(RxData::CacheDescription(id.toString()))
    { }

    virtual ~MessageReaderData()
    { }

    Flower flowController() const
    {
        return flowController_;
    }

    const ReaderId& id() const
    {
        return id_;
    }

    const ChannelHandle& handle() const
    {
        return handle_;
    }

    MessageWriterProxies& proxies()
    {
        return proxies_;
    }

    const MessageWriterProxies& proxies() const
    {
        return proxies_;
    }

    MessageCache<T>& cache()
    {
        return cache_;
    }

private:
    Flower flowController_;

    const ChannelHandle handle_;
    const ReaderId      id_;

    MessageWriterProxies proxies_;
    MessageCache<T>      cache_;
};

}}
