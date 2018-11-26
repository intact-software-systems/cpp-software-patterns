#pragma once

#include "RxTransport/MessageBuilder.h"
#include "MessageReaderFunctions.h"
#include "MessageReaderData.h"
#include "MessageReaderAlgorithms.h"
#include "MessageWriterProxy.h"
#include "MessageWriterProxies.h"
#include "MessageReaderPolicy.h"
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageReaderAlgorithms
{
    template <typename T>
    static MessageStatus Process(const std::list<Message<T>>& messages, MessageReaderData<T>& data)
    {
        MessageStatus processStatus;

        for(const auto& message : messages)
        {
            //IINFO() << "Received: " << message;

            for(auto subMessage : message.data()->Messages())
            {
                ProcessSubMessage<T>(subMessage, data, processStatus);
            }
        }

        return processStatus;
    }

    template <typename T>
    static bool ProcessSubMessage(const std::shared_ptr<SubMessage> subMessage, MessageReaderData<T>& data, MessageStatus& processStatus)
    {
        ASSERT(subMessage);

        switch(subMessage->Kind())
        {
            case MessageKind::Data:
            {
                auto dataMessage = std::dynamic_pointer_cast<DataMessage<T>>(subMessage);
                ASSERT(dataMessage);

                processStatus.NextState(MessageKind::Data, dataMessage->SeqNumber());

                bool updated = data.proxies().Update(dataMessage->Id(), dataMessage->SeqNumber(), ChangeFromWriterStatusKind::RECEIVED);
                ASSERT(updated);

                return data.cache().Add(dataMessage->SeqNumber(), dataMessage);
            }
            case MessageKind::Heartbeat:
            {
                auto heartBeat = std::dynamic_pointer_cast<Heartbeat>(subMessage);
                ASSERT(heartBeat);

                bool missing = data.proxies().Update(heartBeat->Id(), heartBeat->Last(), ChangeFromWriterStatusKind::MISSING);
                bool lost    = data.proxies().Update(heartBeat->Id(), heartBeat->First(), ChangeFromWriterStatusKind::LOST);

                ASSERT(missing);
                ASSERT(lost);

                bool resumeHeartbeat = heartBeat->IsResponseRequired() || (heartBeat->IsResponseRequiredWhenMissingChanges() && data.proxies().IsMissingFor(heartBeat->Id()));
                if(resumeHeartbeat)
                {
                    processStatus.NextState(MessageKind::Heartbeat, 1);
                }

                return resumeHeartbeat;
            }
            case MessageKind::Gap:
            {
                auto gap = std::dynamic_pointer_cast<Gap>(subMessage);
                ASSERT(gap);

                processStatus.NextState(MessageKind::Gap, 1);
                bool updated = data.proxies().Update(gap->Id(), gap->irrelevant(), ChangeFromWriterStatusKind::FILTERED);
                ASSERT(updated);

                return updated;
            }
            case MessageKind::Request:
            {
                auto request = std::dynamic_pointer_cast<Request>(subMessage);
                ASSERT(request);

                // TODO: Pull request sent from Subscriber. Take into account the requested number when sending request.
                processStatus.NextState(MessageKind::Request, request->number().Number());
                return true;
            }
            case MessageKind::AckNack:
            {
                throw CriticalException("Unsupported for " + Utility::GetTypeName<MessageReaderAlgorithms>());
            }
        }
        return false;
    }

    static void TriggerFollowUp(MessageReaderFunctions& functions, const MessageStatus& processStatus)
    {
        if(processStatus.State(MessageKind::AckNack)->IsInstances())
        {
            functions.ackNackTask.Trigger();
        }
        if(processStatus.State(MessageKind::Data)->IsInstances() || processStatus.State(MessageKind::Request)->IsInstances())
        {
            functions.requestTask.Trigger();
        }
    }

    template <typename T>
    static std::list<Message<T>> BuildMessages(MessagePolicy policy, MessageReaderData<T>& data, MessageKind kind)
    {
        std::list<Message<T>> messages;

        //IINFO() << "Building messages for " << data.proxies().toString();

        data.proxies().ForEach(
            [&policy, &data, &kind, &messages](MessageWriterProxyPtr proxy)
            {
                MessageBuilder messageBuilder;

                messageBuilder
                    .Accessor(ChannelAccessorKind::READER)
                    .Handle(data.handle())
                    .FromTo(data.id(), proxy->Id())
                    .With(policy);

                switch(kind)
                {
                    case MessageKind::AckNack:
                    {
                        messageBuilder.Add(std::make_shared<AckNack>(data.id(), proxy->MaxAvailable(), proxy->Missing()));
                        break;
                    }
                    case MessageKind::Request:
                    {
                        // TODO: Calculate requested number (account for pull requests)
                        int request = data.flowController()->AvailableCapacity();
                        ASSERT(request > 0);

                        messageBuilder.Add(std::make_shared<Request>(data.id(), request));
                        break;
                    }
                    case MessageKind::Heartbeat:
                    case MessageKind::Data:
                    case MessageKind::Gap:
                    {
                        IFATAL() << "Misconfigured message reader";
                        break;
                    }
                }

                if(messageBuilder.IsContent())
                {
                    Message<T> message = messageBuilder.template Create<T>();
                    messages.push_back(message);
                }
            }
        );

        return messages;
    }

    template <typename T>
    static bool SendMessages(const std::list<Message<T>>& messages, MessageReaderStatus& status, RxObserverSubject<Message<T>>& channel)
    {
        for(const auto& message : messages)
        {
            ASSERT(message.IsContent());
            //IINFO() << "Sending: " << message;

            for(auto subMessage : message.data()->Messages())
            {
                status.Next<Message<T>>(&message, subMessage->Kind(), 1);
            }

            channel.OnNext(message);
        }

        return !messages.empty();
    }
};

}}
