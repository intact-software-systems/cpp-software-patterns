#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Message.h"
#include"RxTransport/MessagePolicy.h"
#include"RxTransport/MessageFuture.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageBuilder
{
public:
    MessageBuilder();
    virtual ~MessageBuilder();

    MessageBuilder& Accessor(ChannelAccessorKind kind);
    MessageBuilder& Handle(ChannelHandle channelHandle);
    MessageBuilder& FromTo(GUUID from, GUUID to);
    MessageBuilder& With(MessagePolicy policy);
    MessageBuilder& Add(std::shared_ptr<SubMessage> msg);

    bool IsContent() const;

    int     NumberOf(MessageKind kind) const;
    size_t  TotalNumberOf() const;

    template <typename T>
    Message<T> Create()
    {
        Message<T> message = Message<T>(handle_, from_, to_, messages_, policy_, channelAccessorKind_);

        for(SubMessagePtr subMessage : messages_)
        {
            message.status()->NextState(subMessage->Kind(), 1);
        }

        return message;
    }

private:
    ChannelHandle handle_;

    GUUID from_;
    GUUID to_;

    MessagePolicy policy_;

    std::list<std::shared_ptr<SubMessage>> messages_;
    ChannelAccessorKind                    channelAccessorKind_;
};

}}

