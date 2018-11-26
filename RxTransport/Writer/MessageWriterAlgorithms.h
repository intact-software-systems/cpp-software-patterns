#pragma once

#include "RxTransport/Message.h"
#include "RxTransport/MessageBuilder.h"
#include "MessageWriterPolicy.h"
#include "MessageReaderProxy.h"

#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageWriterAlgorithms
{
    template <typename T>
    static MessageStatus Process(std::list<Message<T>> messages, Policy::CommDirection direction, MessageWriterData<T>& data)
    {
        MessageStatus processStatus;

        for(const auto& message : messages)
        {
            //IINFO() << "Processing: " << message;

            for(auto subMessage : message.data()->Messages())
            {
                bool processed = ProcessSubMessage<T>(subMessage, direction, data, processStatus);
                ASSERT(processed);
            }
        }

        ASSERT(processStatus.IsStatus());

        return processStatus;
    }

    template <typename T>
    static bool ProcessSubMessage(SubMessagePtr subMessage, Policy::CommDirection direction, MessageWriterData<T>& data, MessageStatus& processStatus)
    {
        switch(subMessage->Kind())
        {
            case MessageKind::Data:
            {
                std::shared_ptr<DataMessage<T>> dataMessage = std::dynamic_pointer_cast<DataMessage<T>>(subMessage);
                ASSERT(dataMessage);

                processStatus.NextState(MessageKind::Data, dataMessage->SeqNumber());
                data.cache().Add(dataMessage->SeqNumber(), dataMessage);

                switch(direction)
                {
                    case Policy::CommDirection::PushPull:
                    case Policy::CommDirection::Push:
                    {
                        data.proxies().AddSequenceNumber(dataMessage->SeqNumber(), ChangeForReaderStatusKind::UNSENT);

                        IINFO() << "Added changes to: " << data.proxies();
                        break;
                    }
                    case Policy::CommDirection::Pull:
                    {
                        data.proxies().AddSequenceNumber(dataMessage->SeqNumber(), ChangeForReaderStatusKind::UNACKNOWLEDGED);
                        break;
                    }
                }

                return true;
            }
            case MessageKind::AckNack:
            {
                std::shared_ptr<AckNack> ackNack = std::dynamic_pointer_cast<AckNack>(subMessage);
                ASSERT(ackNack);

                processStatus.NextState(MessageKind::AckNack, 1);

                MapChangeForReader updated = data.proxies().Next(ackNack->Id(), ackNack);

                for(auto entry : updated)
                {
                    data.cache().Update(entry.first, entry.second->CurrentState());
                }

                return true;
            }
            case MessageKind::Request:
            {
                std::shared_ptr<Request> request = std::dynamic_pointer_cast<Request>(subMessage);
                ASSERT(request);

                processStatus.NextState(MessageKind::Request, 1);
                return data.proxies().Next(request->Id(), request);
            }
            case MessageKind::Gap:
            case MessageKind::Heartbeat:
            {
                throw CriticalException("Unsupported messages for MessageWriter");
            }
        }
        return false;
    }

    static void TriggerFollowUp(MessageWriterFunctions& functions, const MessageStatus& processStatus)
    {
        if(processStatus.State(MessageKind::Data)->IsInstances())
        {
            functions.dataSchedFuture.Trigger();
        }
        if(processStatus.State(MessageKind::AckNack)->IsInstances())
        {
            functions.nackTask.Trigger();
        }
    }

    template <typename T>
    static std::list<Message<T>> BuildHeartbeats(MessagePolicy policy, MessageWriterData<T>& data)
    {
        if(data.proxies().IsEmpty())
        {
            return std::list<Message<T>>();
        }

        // TODO: Review when to send heartbeat
        // Use MessageWriterStatus.PreviousHeartbeat
        // Only send if new information? or send regardless? Policy on this?

        auto heartbeat = std::make_shared<Heartbeat>(data.id(), data.cache().Min(), data.cache().Max(), false, false);

        std::list<Message<T>> messages;

        data.proxies().ForEach(
            [&policy, &data, &heartbeat, &messages](std::shared_ptr<MessageReaderProxy> reader)
            {
                MessageBuilder messageBuilder;

                Message<T> message = messageBuilder
                    .Accessor(ChannelAccessorKind::WRITER)
                    .Handle(data.handle())
                    .FromTo(data.id(), reader->Id())
                    .With(policy)
                    .Add(heartbeat)
                    .template Create<T>();

                ASSERT(message.IsContent());

                messages.push_back(message);
            }
        );

        return messages;
    }

    template <typename T>
    static std::list<Message<T>> BuildMessages(MessagePolicy policy, MessageWriterData<T>& data, ChangeForReaderStatusKind kind)
    {
        std::list<Message<T>> messages;

        //IINFO() << "Building messages for " << data.proxies().toString();

        data.proxies().ForEach(
            [&policy, &data, &kind, &messages](std::shared_ptr<MessageReaderProxy> reader)
            {
                int number = reader->RequestedNumber().Number();
                if(number > 0)
                {
                    //IINFO() << reader->toString();

                    MapChangeForReader changes = reader->SelectN(number, kind);

                    IINFO() << reader << ". Selected " << (char) kind << ": " << changes.keySet();

                    if(!changes.empty())
                    {
                        Message<T> message = BuildMessageForReader<T>(policy, data, reader->Id(), changes);

                        if(message.IsContent())
                        {
                            int dataMessages = message.NumberOf(MessageKind::Data);
                            reader->Underway(dataMessages);
                            messages.push_back(message);
                        }
                    }
                }
            }
        );

        return messages;
    }

    template <typename T>
    static Message<T> BuildMessageForReader(const MessagePolicy& policy, MessageWriterData<T>& data, const ReaderId& readerId, const MapChangeForReader& changes)
    {
        if(!changes.empty())
        {
            std::unique_ptr<MessageBuilder> builder = std::make_unique<MessageBuilder>();

            builder
                ->Accessor(ChannelAccessorKind::WRITER)
                .Handle(data.handle())
                .FromTo(data.id(), readerId)
                .With(policy);

            AddMessages<T>(data, changes, builder);

            return builder->template Create<T>();
        }
        else
        {
            return Message<T>::Empty();
        }
    }

    template <typename T>
    static bool AddMessages(MessageWriterData<T>& data, const MapChangeForReader& changes, std::unique_ptr<MessageBuilder>& builder)
    {
        SequenceNumberSet missing;

        for(auto entry : changes)
        {
            const SequenceNumber                     & number            = entry.first;
            std::shared_ptr<MessageReaderProxyStatus>& readerProxyStatus = entry.second;

            std::shared_ptr<DataMessage<T>> cached = data.cache().Find(number);

            if(!cached)
            {
                missing.insert(number);
                IINFO() << "Missing cache change for " << number;
            }
            else if(readerProxyStatus->State(ChangeForReaderStatusKind::FILTERED)->TotalSum() > 0)
            {
                cached->Status().NextState(TransmissionStatusKind::FILTERED, 1);
                missing.insert(number);
            }
            else
            {
                readerProxyStatus->Next<MessageWriterData<T>>(&data, ChangeForReaderStatusKind::UNDERWAY, 1);
                cached->Status().NextState(TransmissionStatusKind::UNDERWAY, 1);

                builder->Add(cached);
            }
        }

        if(!missing.empty())
        {
            IINFO() << "Filtered changes: " << missing;
            builder->Add(std::make_shared<Gap>(data.id(), missing));
        }

        return builder->IsContent();
    }

    template <typename T>
    static bool SendMessages(const std::list<Message<T>>& messages, MessageWriterStatus& status, RxObserverSubject<Message<T>>& channel)
    {
        for(const auto& message : messages)
        {
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
