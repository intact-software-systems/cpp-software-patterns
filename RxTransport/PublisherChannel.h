#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/MessageChannel.h"
#include "ChannelFeedbackEvent.h"

namespace BaseLib { namespace Concurrent {

/*----------------------------------------------------------------------------------
 Functionality needed:
 - Publish to all in a channel
 - Publish to individual readers. For example: Late arriving subscribers
      - Note! This can be done in the SubscriberChannel by using the cached messages
 - Publish with varying message policies that override writer policy
 - Feedback on messages and channel status
    - OnReaderAdded(), OnReaderRemoved()
----------------------------------------------------------------------------------*/

template <typename T>
class PublisherChannel
    : public Status::ActivationStatus
      , public Templates::Key1<WriterId>
      , public ChannelFeedbackForPublisher
      , public MessageFeedbackForPublisher
      , protected Templates::ContextObjectShared<MessagePolicy, MessageChannel<T>>
{
public:
    PublisherChannel(std::shared_ptr<MessageChannel<T>> channel, WriterId writerId, MessagePolicy policy)
        : Templates::Key1<WriterId>(writerId)
        , Templates::ContextObjectShared<MessagePolicy, MessageChannel<T>>
            (
                new MessagePolicy(policy), channel->shared_from_this()
            )
    { }

    virtual ~PublisherChannel()
    {
        this->data()->RemoveWriter(this->Id());
    }

    ChannelHandle handle() const
    {
        return this->data()->Id();
    }

    RxData::ObjectAccessProxy<T, InstanceHandle> DataCache()
    {
        auto writer = this->data()->FindWriter(this->Id());
        ASSERT(writer);

        return writer->DataCache();
    }

    // --------------------------------------------------------
    // Interface ReactorMethods
    // --------------------------------------------------------

    bool Error(GeneralException exception)
    {
        return this->data()->Error(exception);
    }

    bool Complete()
    {
        return this->data()->RemoveWriter(this->Id()) != nullptr;
    }

    MessageFuture<T> Next(T value)
    {
        return next(value, InstanceHandle::NIL(), MessageChangeKind::ALIVE);
    }

    MessageFuture<T> Next(T value, InstanceHandle handle, MessageChangeKind changeKind = MessageChangeKind::ALIVE)
    {
        return next(value, handle, changeKind);
    }

    // TODO: Support unicast. Specialized class?
    MessageFuture<T> Next(T, ReaderId)
    {
        return MessageFuture<T>::Empty(); //next(value, handle);
    }

    // --------------------------------------------------------
    // Interfaces for feedback
    // --------------------------------------------------------

    void OnMessageFeedback(MessageWriterFeedbackKind t, SequenceNumber u, MessageReaderProxyStatus v)
    {
        if(messageFeedbackFunction)
        {
            messageFeedbackFunction(t, u, v);
        }
    }

    virtual void OnChannelFeedback(MessageWriterFeedbackKind kind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>> value)
    {
        if(channelFeedbackFunction)
        {
            channelFeedbackFunction(kind, value);
        }
    }

    // --------------------------------------------------------
    // Set callback functions
    // --------------------------------------------------------

    PublisherChannel<T>& OnMessageFeedbackDo(MessageFeedbackFunctionForPublisher func)
    {
        messageFeedbackFunction = func;
        return *this;
    }

    PublisherChannel<T>& OnChannelFeedbackDo(ChannelFeedbackFunctionForPublisher func)
    {
        channelFeedbackFunction = func;
        return *this;
    }

private:

    MessageFuture<T> next(T value, InstanceHandle handle, MessageChangeKind changeKind)
    {
        if(!IsActive())
        {
            IINFO() << "Publisher is deactivated " << Id();
            return MessageFuture<T>::Empty();
        }

        DataHolder<T> dataHolder(
            value,
            Id(),
            handle,
            changeKind,
            this->config().delegate()
        );

        return this->data()->Next(std::pair<MessagePolicy, DataHolder<T>>(this->config().delegate(), dataHolder));
    }

private:
    MessageFeedbackFunctionForPublisher messageFeedbackFunction;
    ChannelFeedbackFunctionForPublisher channelFeedbackFunction;
};

}}
