#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/MessageController.h"
#include "ChannelFeedbackEvent.h"

namespace BaseLib { namespace Concurrent {

/* ----------------------------------------------------------------------------------
 Functionality needed:
 - Late arriving subscribers - replay n most recent messages to these
      - Let this functionality be implemented in RxObserver, use this->DataCache().SelectLastN();
 - Feedback as events to individual subscribers for policy violations, message arrival, etc
 - Request(int n) - for pull based communication
 - Feedback on discovery of writer/publisher
 ----------------------------------------------------------------------------------*/
template <typename T>
class SubscriberChannel
    : public Status::ActivationStatus
      , public RxObserver<T>
      , public ChannelFeedbackForSubscriber
      , public MessageFeedbackForSubscriber
      , public Templates::Key1<ReaderId>
      , protected Templates::ContextObjectShared<MessagePolicy, MessageChannel<T>>
{
public:
    SubscriberChannel(std::shared_ptr<MessageChannel<T>> channel, ReaderId readerId, MessagePolicy messagePolicy)
        : Templates::Key1<ReaderId>(readerId)
        , Templates::ContextObjectShared<MessagePolicy, MessageChannel<T>>
            (
                new MessagePolicy(messagePolicy), channel->shared_from_this()
            )
    { }

    virtual ~SubscriberChannel()
    {
        this->data()->RemoveReader(this->Id());
    }

    RxData::ObjectAccessProxy<T, InstanceHandle> DataCache()
    {
        std::shared_ptr<MessageReader<T>> reader = this->data()->FindReader(this->Id());
        ASSERT(reader);

        return reader->DataCache();
    }

    ChannelHandle Handle() const
    {
        return this->data()->Id();
    }

    void Request(int n)
    {
        this->data()->Request(this->Id(), n);
    }

//    void RequestFrom(int n, WriterId writerId)
//    {
//        this->data()->Request(this->Id(), n);
//    }

    // --------------------------------------------------------
    // Interface RxObserver
    // --------------------------------------------------------

    virtual void OnError(GeneralException exception)
    {
        if(onError)
        {
            onError(exception);
        }
    }

    virtual void OnComplete()
    {
        if(onComplete)
        {
            onComplete();
        }
    }

    virtual void OnNext(T value)
    {
        if(onData)
        {
            onData(value);
        }
    }

    // --------------------------------------------------------
    // Set callback functions
    // --------------------------------------------------------

    SubscriberChannel<T>& OnErrorDo(std::function<void(GeneralException)> onErrorDo)
    {
        onError = onErrorDo;
        return *this;
    }

    SubscriberChannel<T>& OnCompleteDo(std::function<void()> onCompleteDo)
    {
        onComplete = onCompleteDo;
        return *this;
    }

    SubscriberChannel<T>& OnDataDo(std::function<void(T)> onDataDo)
    {
        onData = onDataDo;
        return *this;
    }

    SubscriberChannel<T>& OnMessageFeedbackDo(MessageFeedbackFunctionForSubscriber func)
    {
        messageFeedbackFunction = func;
        return *this;
    }

    SubscriberChannel<T>& OnChannelFeedbackDo(ChannelFeedbackFunctionForSubscriber func)
    {
        channelFeedbackFunction = func;
        return *this;
    }

    // --------------------------------------------------------
    // Interfaces for feedback
    // --------------------------------------------------------

    void OnMessageFeedback(MessageReaderFeedbackKind kind, SequenceNumber number, MessageWriterProxyStatus status)
    {
        if(messageFeedbackFunction)
        {
            messageFeedbackFunction(kind, number, status);
        }
    }

    virtual void OnChannelFeedback(MessageReaderFeedbackKind kind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>> value)
    {
        if(channelFeedbackFunction)
        {
            channelFeedbackFunction(kind, value);
        }
    }

private:
    MessageFeedbackFunctionForSubscriber  messageFeedbackFunction;
    ChannelFeedbackFunctionForSubscriber  channelFeedbackFunction;

    std::function<void(T)>                onData;
    std::function<void()>                 onComplete;
    std::function<void(GeneralException)> onError;
};

}}
