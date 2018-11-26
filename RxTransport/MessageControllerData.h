#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Reader/MessageReaders.h"
#include"RxTransport/Writer/MessageWriters.h"
#include"RxTransport/MessageControllerPolicy.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class MessageControllerData
{
    typedef std::shared_ptr<details::FlowControllerAction<Message<T>>> Flower;

public:
    MessageControllerData(ChannelHandle id, FlowControllerPolicy policy)
        : flowController_(new details::FlowControllerAction<Message<T>>(policy))
        , handle_(id)
    { }

    virtual ~MessageControllerData()
    { }

    Flower flowController() const
    {
        return flowController_;
    }

    const MessageReaders<T>& readers() const
    {
        return readers_;
    }

    const MessageWriters<T>& writers() const
    {
        return writers_;
    }

    MessageReaders<T>& readers()
    {
        return readers_;
    }

    MessageWriters<T>& writers()
    {
        return writers_;
    }

    const ChannelHandle& handle() const
    {
        return handle_;
    }

private:
    Flower flowController_;

    MessageReaders<T> readers_;
    MessageWriters<T> writers_;

    const ChannelHandle handle_;
};

}}
