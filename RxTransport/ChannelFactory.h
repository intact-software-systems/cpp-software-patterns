#pragma once

#include <RxTransport/Writer/MessageWriterPolicy.h>
#include <RxTransport/Reader/MessageReaderPolicy.h>

#include "RxTransport/CommonDefines.h"
#include "RxTransport/MessageController.h"
#include "RxTransport/RWChannel.h"
#include "RxTransport/PublisherChannel.h"
#include "RxTransport/SubscriberChannel.h"
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * Create functionality to configure channels:
 *
 * - Connect to a channel as a reader or a writer
 * - Configure readers to receive data from local and remote writers
 * - Configure writers to send data to local and remote readers
 *
 * Policies on:
 * - Local and/or Remote
 * - LatencyBudget
 * - TransportPriority
 * - DestinationOrder (send or receive timestamp)
 * - Liveliness
 * - Durability (option to persist data)
 *
 * Autoconfigure domains and channels when new readers and writers are created in a "discovery" cache.
 *
 * Channel must be configured through a "discovery mechanism".
 *
 * TODO: RemoveIf deactivated, ClearIf deactivated?
 *
 * This is a LocalChannelFactory, I need a DistributedChannelFactory
 */
class DLL_STATE ChannelFactory
    : public Templates::FactorySingleton<ChannelFactory>
      , protected Templates::ContextDataShared<LookupScopeManager>
{
public:
    ChannelFactory();
    virtual ~ChannelFactory();

    // -------------------------------------------------
    // Activation functions
    // -------------------------------------------------

    bool Activate(DomainScopeHandle handle);
    bool Deactivate(DomainScopeHandle handle);
    bool IsActive(DomainScopeHandle handle);

    bool Activate(ChannelHandle handle);
    bool Deactivate(ChannelHandle handle);
    bool IsActive(ChannelHandle handle);

    // -------------------------------------------------
    // Publisher and subscriber
    // -------------------------------------------------

    template <typename T>
    static std::shared_ptr<PublisherChannel<T>> CreatePublisher(
        ChannelHandle handle,
        MessageControllerPolicy policy = MessageControllerPolicy::FireAndForget(),
        MessageWriterPolicy writerPolicy = MessageWriterPolicy::Default(),
        MessagePolicy messagePolicy = MessagePolicy::Default()
    )
    {
        auto channel = Instance().template GetOrCreate<T>(handle, policy);
        auto writer  = channel->Create(writerPolicy, WriterId::Create());

        IINFO() << writer->toString();

        auto publisher = std::make_shared<PublisherChannel<T>>(channel, writer->Id(), messagePolicy);

        writer->MessageFeedbackChannel().Connect(publisher.get());
        writer->ChannelFeedbackChannel().Connect(publisher.get());
        writer->Activate();

        publisher->Activate();
        return publisher;
    }

    template <typename T>
    static std::shared_ptr<SubscriberChannel<T>> CreateSubscriber(
        ChannelHandle handle,
        MessageControllerPolicy policy = MessageControllerPolicy::FireAndForget(),
        MessageReaderPolicy readerPolicy = MessageReaderPolicy::Default(),
        MessagePolicy messagePolicy = MessagePolicy::Default()
    )
    {
        auto channel = Instance().template GetOrCreate<T>(handle, policy);
        auto reader  = channel->Create(readerPolicy, ReaderId::Create());

        IINFO() << reader->toString();

        auto subscriber = std::make_shared<SubscriberChannel<T>>(channel, reader->Id(), messagePolicy);

        reader->DataChannel().Connect(subscriber.get());
        reader->MessageFeedbackChannel().Connect(subscriber.get());
        reader->ChannelFeedbackChannel().Connect(subscriber.get());
        reader->Activate();

        subscriber->Activate();
        return subscriber;
    }

    template <typename T>
    static std::shared_ptr<RWChannel<T>> CreateRWChannel(
        ChannelHandle handle,
        MessageControllerPolicy policy = MessageControllerPolicy::FireAndForget(),
        MessageWriterPolicy writerPolicy = MessageWriterPolicy::Default(),
        MessageReaderPolicy readerPolicy = MessageReaderPolicy::Default(),
        MessagePolicy messagePolicy = MessagePolicy::Default()
    )
    {
        // TODO: Option to create unique or isolated read write channels
        auto subscriber = CreateSubscriber<T>(handle, policy, readerPolicy, messagePolicy);
        auto publisher  = CreatePublisher<T>(handle, policy, writerPolicy, messagePolicy);

        return std::make_shared<RWChannel<T>>(subscriber, publisher);
    }

    // -------------------------------------------------
    // Templated find and create functions for channel
    // -------------------------------------------------

    template <typename T>
    std::shared_ptr<MessageChannel<T>> GetOrCreate(ChannelHandle handle, MessageControllerPolicy controllerPolicy)
    {
        ChannelScopeManagerPtr manager = this->context()->GetOrCreate(
            DomainScopeHandle{handle.first(), handle.second()},
            [&handle]()
            {
                auto item = std::make_shared<ChannelScopeManager>(DomainScopeHandle{handle.first(), handle.second()});

                bool initialized = item->Initialize();
                ASSERT(initialized);

                item->Activate();

                return item;
            }
        );

        auto messageChannel = manager->GetOrCreate(
            handle.third(),
            [&handle, &controllerPolicy]()
            {
                auto item = std::make_shared<MessageChannel<T>>(
                    std::make_shared<MessageController<T>>(handle, controllerPolicy)
                );

                bool initialized = item->Initialize();
                ASSERT(initialized);

                item->Activate();

                return item;
            }
        );

        return std::dynamic_pointer_cast<MessageChannel<T>>(messageChannel);
    }

    template <typename T>
    std::shared_ptr<MessageChannel<T>> Find(ChannelHandle handle) const
    {
        ChannelScopeManagerPtr manager = this->context()->Find(DomainScopeHandle{handle.first(), handle.second()});

        if(manager != nullptr)
        {
            auto messageChannel = manager->Find(handle.third());
            return messageChannel != nullptr
                   ? std::dynamic_pointer_cast<MessageChannel<T>>(messageChannel)
                   : nullptr;
        }

        return nullptr;
    }

    // ---------------------------------------------
    // Print data
    // ---------------------------------------------

    std::string ToString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const ChannelFactory& proxy)
    {
        ostr << proxy.ToString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<ChannelFactory>& proxy)
    {
        ostr << proxy->ToString();
        return ostr;
    }
};

}}
