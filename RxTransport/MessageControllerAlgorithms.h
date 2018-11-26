#pragma once

#include "RxTransport/Message.h"
#include "RxTransport/MessageControllerPolicy.h"
#include "RxTransport/MessageControllerData.h"
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageControllerAlgorithms
{
    template <typename T>
    static bool RouteMessages(const std::list<Message<T>>& messages, const MessageControllerData<T>& data)
    {
        for(const Message<T>& message : messages)
        {
            switch(message.data()->AccessorKind())
            {
                case ChannelAccessorKind::READER:
                {
                    auto writer = data.writers().Find(message.data()->To());

                    bool result = writer != nullptr
                                  ? writer->Next(message)
                                  : false;
                    break;
                }
                case ChannelAccessorKind::WRITER:
                {
                    auto reader = data.readers().Find(message.data()->To());

                    bool result = reader != nullptr
                                  ? reader->Next(message)
                                  : false;
                    break;
                }
                case ChannelAccessorKind::PUBLISHER:
                {
                    ASSERT(message.data()->To() == MessageAddress::multicast);

                    auto writer = data.writers().Find(message.data()->From());

                    bool result = writer != nullptr
                                  ? writer->Next(message)
                                  : false;
                    break;
                }
                case ChannelAccessorKind::SUBSCRIBER:
                {
                    ASSERT(message.data()->To() == MessageAddress::multicast);

                    auto reader = data.readers().Find(message.data()->From());

                    bool result = reader != nullptr
                                  ? reader->Next(message)
                                  : false;
                    break;
                }
                case ChannelAccessorKind::NO:
                    IFATAL() << "Message routed to no one: " << message;
                    break;
            }
        }

        return !messages.empty();
    }

    template <typename T>
    static Message<T> PullRequest(ReaderId id, int n, const std::shared_ptr<details::FlowControllerAction<Message<T>>>& flowController, const ChannelHandle& channelHandle)
    {
        MessageBuilder builder;

        Message<T> message = builder
            .Accessor(ChannelAccessorKind::SUBSCRIBER)
            .Handle(channelHandle)
            .FromTo(id, MessageAddress::multicast)
            .With(MessagePolicy::Default())
            .Add(std::make_shared<Request>(id, n))
            .template Create<T>();

        return flowController->Next(message)
               ? message
               : Message<T>::Empty();
    }

    template <typename T>
    static MessageFuture<T> RouteData(const MessagePolicy& policy, DataHolder<T>& messageData, MessageControllerData<T>& data, SequenceNumberCounter& status)
    {
        // TODO: race condition atomic inc and get
        SequenceNumber currentSequenceNumber = status.CurrentState();
        ++currentSequenceNumber;

        messageData.status_.NextState(TransmissionStatusKind::ROUTED, 1);

        std::shared_ptr<DataMessage<T>> dataMessage = std::make_shared<DataMessage<T>>(messageData, currentSequenceNumber);

        MessageBuilder builder;

        Message<T> message = builder
            .Accessor(ChannelAccessorKind::PUBLISHER)
            .Handle(data.handle())
            .FromTo(messageData.writerId_, MessageAddress::multicast)
            .With(policy)
            .Add(dataMessage)
            .template Create<T>();

        status.Next<MessageControllerData<T>>(&data, currentSequenceNumber, 1);

        return data.flowController()->Next(message)
               ? MessageFuture<T>(message, dataMessage->DeliveryPromise()->get_future())
               : MessageFuture<T>::Empty();
    }
};

}}
