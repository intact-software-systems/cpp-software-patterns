#pragma once

#include "RxTransport/CommonDefines.h"
#include "RxTransport/MessageCache.h"
#include "RxTransport/Message.h"
#include "MessageReaderProxies.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class MessageWriterData
{
    typedef std::shared_ptr<details::FlowControllerAction<Message<T>>> Flower;

public:
    MessageWriterData(ChannelHandle handle, WriterId id, const FlowControllerPolicy& policy)
        : flowController_(new details::FlowControllerAction<Message<T>>(policy))
        , handle_(handle)
        , id_(id)
        , proxies_()
        , cache_(RxData::CacheDescription(id.toString()))
    { }

    virtual ~MessageWriterData()
    { }

    Flower flowController() const
    {
        return flowController_;
    }

    WriterId id() const
    {
        return id_;
    }

    const ChannelHandle& handle() const
    {
        return handle_;
    }

    MessageReaderProxies& proxies()
    {
        return proxies_;
    }

    const MessageReaderProxies& proxies() const
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
    const WriterId      id_;

    MessageReaderProxies proxies_;
    MessageCache<T>      cache_;
};

}}
