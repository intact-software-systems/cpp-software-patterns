#pragma once

#include "RxTransport/CommonDefines.h"
#include "RxTransport/MessageController.h"

#include "RxTransport/Writer/MessageReaderProxies.h"
#include "RxTransport/Reader/MessageWriterProxies.h"

namespace BaseLib { namespace Concurrent {

/**
 * Types of channels: unicast, broadcast, multicast
 *
 * Client -> PublisherChannel -> Controller/Router -> Writer -> (Router -> Channel -> Router) -> Reader -> Controller/Router (-> SubscriberChannel) -> Client
 */
template <typename T>
class MessageChannel
    : public MessageChannelBase
      , public Templates::ReactorMethods<MessageFuture<T>, std::pair<MessagePolicy, DataHolder<T>>, BaseLib::GeneralException>
      , protected Templates::ContextDataShared<MessageController<T>>
      , public std::enable_shared_from_this<MessageChannel<T>>
      , public Templates::LockableType<MessageChannel<T>>
{
    typedef MutexTypeLocker<MessageChannel<T>> Locker;

public:
    MessageChannel(std::shared_ptr<MessageController<T>> controller)
        : MessageChannelBase(controller->Id())
        , Templates::ContextDataShared<MessageController<T>>(controller)
    { }

    virtual ~MessageChannel()
    { }

    // --------------------------------------------
    // Create readers and writers
    // --------------------------------------------

    /**
     * Create local message reader and writers
     *
     * - Configure readers to receive data from local and remote writers
     * - Configure writers to send data to local and remote readers
     * - Activate readers and writers
     */
    std::shared_ptr<MessageReader<T>> Create(MessageReaderPolicy policy, const ReaderId& readerId)
    {
        Locker locker(this);

        auto messageReader = std::make_shared<MessageReader<T>>(this->Id(), readerId, policy);
        messageReader->Initialize();

        this->context()->Readers().Add(messageReader->Id(), messageReader);

        messageReader->MessageChannel().Connect(this->context().operator->());

        this->context()->Writers().ForEach(
            [&messageReader, &policy](std::shared_ptr<MessageWriter<T>> writer)
            {
                auto readerProxy = std::make_shared<MessageReaderProxy>(messageReader->Id(), policy);
                readerProxy->Activate();

                writer->Proxies().Add(messageReader->Id(), readerProxy);

                auto writerProxy = std::make_shared<MessageWriterProxy>(writer->Id(), writer->config().delegate());
                messageReader->Proxies().Add(writer->Id(), writerProxy);
            }
        );

        return messageReader;
    }

    std::shared_ptr<MessageWriter<T>> Create(MessageWriterPolicy policy, const WriterId& writerId)
    {
        Locker locker(this);

        auto messageWriter = std::make_shared<MessageWriter<T>>(this->Id(), writerId, policy);
        messageWriter->Initialize();

        this->context()->Writers().Add(messageWriter->Id(), messageWriter);

        messageWriter->Channel().Connect(this->context().operator->());

        this->context()->Readers().ForEach(
            [&messageWriter, &policy](std::shared_ptr<MessageReader<T>> reader)
            {
                auto writerProxy = std::make_shared<MessageWriterProxy>(messageWriter->Id(), policy);
                writerProxy->Activate();

                reader->Proxies().Add(messageWriter->Id(), writerProxy);

                auto readerProxy = std::make_shared<MessageReaderProxy>(reader->Id(), reader->config().delegate());
                messageWriter->Proxies().Add(reader->Id(), readerProxy);
            }
        );

        return messageWriter;
    }

    // --------------------------------------------
    // Access state and set state
    // --------------------------------------------

    /**
     * Before attaching reader and writer then attach "router for remote distribution of messages"
     */
    bool AddReader(std::shared_ptr<MessageReader<T>> reader)
    {
        return this->context()->Readers().Add(reader->Id(), reader);
    }

    bool AddWriter(std::shared_ptr<MessageWriter<T>> writer)
    {
        return this->context()->Writers().Add(writer->Id(), writer);
    }

    std::shared_ptr<MessageWriter<T>> RemoveWriter(WriterId writerId)
    {
        return this->context()->Writers().Remove(writerId);
    }

    std::shared_ptr<MessageReader<T>> RemoveReader(ReaderId readerId)
    {
        return this->context()->Readers().Remove(readerId);
    }

    std::shared_ptr<MessageWriter<T>> FindWriter(WriterId writerId)
    {
        return this->context()->Writers().Find(writerId);
    }

    std::shared_ptr<MessageReader<T>> FindReader(ReaderId readerId)
    {
        return this->context()->Readers().Find(readerId);
    }

    // --------------------------------------------
    // Activate and deactivate
    // --------------------------------------------

    bool IsActive() const override
    {
        return this->context()->IsActive();
    }

    bool Activate() override
    {
        return this->context()->Activate();
    }

    bool Deactivate() override
    {
        return this->context()->Deactivate();
    }

    // --------------------------------------------
    // Pull-based communication
    // --------------------------------------------

    Message<T> Request(ReaderId id, int n)
    {
        return this->context()->Request(id, n);
    }

    // --------------------------------------------
    // Interface ReactorMethods - write data
    // --------------------------------------------

    virtual bool Complete()
    {
        return this->context()->Complete();
    }

    virtual MessageFuture<T> Next(std::pair<MessagePolicy, DataHolder<T>> data)
    {
        return this->context()->Next(data);
    }

    virtual bool Error(GeneralException exception)
    {
        return this->context()->Error(exception);
    }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    virtual bool Initialize()
    {
        return this->context()->Initialize();
    }

    virtual bool IsInitialized() const
    {
        return this->context()->IsInitialized();
    }

    // ---------------------------------------------
    // Print data
    // ---------------------------------------------

    std::string toString() const
    {
        std::stringstream ostr;
        ostr << "MessageChannel(" << this->context()->toString();
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageChannel<T>& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageChannel<T>>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
