#pragma once

#include <RxTransport/CommonDefines.h>

#include "MessageReaderProxies.h"
#include "MessageWriterPolicy.h"
#include "MessageWriterData.h"
#include "MessageWriterFunctions.h"
#include "MessageWriterAlgorithms.h"
#include "MessageWriterPolicyAlgorithms.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
struct MessageWriterSubjects
{
    RxObserverSubject<Message<T>>   channel;
    ChannelFeedbackSubjectForWriter channelFeedback;
    MessageFeedbackSubjectForWriter messageFeedback;
};

template <typename T>
class MessageWriter
    : public MessageWriterBase
      , public Templates::ReactorMethods<bool, Message<T>, BaseLib::GeneralException>
      , public Templates::ContextObjectShared<MessageWriterPolicy, MessageWriterData<T>, MessageWriterStatus, MessageWriterSubjects<T>, MessageWriterFunctions>
      , public std::enable_shared_from_this<MessageWriter<T>>
{
    typedef MessageWriterAlgorithms       MWA;
    typedef MessageWriterPolicyAlgorithms MWPA;

public:
    MessageWriter(ChannelHandle handle, WriterId writerId, MessageWriterPolicy policy = MessageWriterPolicy::Default())
        : MessageWriterBase(writerId)
        , Templates::ContextObjectShared<MessageWriterPolicy, MessageWriterData<T>, MessageWriterStatus, MessageWriterSubjects<T>, MessageWriterFunctions>
            (
                new MessageWriterPolicy(policy),
                new MessageWriterData<T>(handle, writerId, policy.flowControllerPolicy),
                new MessageWriterStatus(),
                new MessageWriterSubjects<T>()
            )
    { }

    virtual ~MessageWriter()
    { }

    // --------------------------------------------
    // Access functions
    // --------------------------------------------

    MessageReaderProxies& Proxies()
    {
        return this->data()->proxies();
    }

    RxObserverSubject<Message<T>>& Channel()
    {
        return this->subject()->channel;
    }

    MessageFeedbackSubjectForWriter& MessageFeedbackChannel()
    {
        return this->subject()->messageFeedback;
    }

    ChannelFeedbackSubjectForWriter& ChannelFeedbackChannel()
    {
        return this->subject()->channelFeedback;
    }

    RxObserverSubject<T>& DataChannel()
    {
        return this->data()->cache().Channel();
    }

    RxData::ObjectAccessProxy<T, InstanceHandle> DataCache()
    {
        return this->data()->cache().DataCache();
    }

    // --------------------------------------------
    // Interface ActivationStatus
    // --------------------------------------------

    virtual bool Activate() override
    {
        Status::ActivationStatus::Activate();
        this->data()->proxies().Activate();
        return this->function()->Activate();
    }

    virtual bool Deactivate() override
    {
        this->function()->Deactivate();
        this->data()->proxies().Deactivate();
        return Status::ActivationStatus::Deactivate();
    }

    // --------------------------------------------
    // Interface Action<bool>
    // --------------------------------------------

    virtual bool Invoke()
    {
        if(!this->IsActive())
        {
            IWARNING() << "Writer is not active: " << this->Id() << " for channel " << this->data()->handle().third();
            return false;
        }

        std::list<Message<T>> messages = this->data()->flowController()->Pull();

        if(!messages.empty())
        {
            MessageStatus processStatus = MWA::Process<T>(messages, this->config()->commDirection, this->data().delegate());

            ASSERT(processStatus.IsStatus());
            if(processStatus.IsStatus())
            {
                MWA::TriggerFollowUp(this->function().delegate(), processStatus);
                MWPA::PolicyViolations<T>(this->data()->proxies(), this->config().delegate(), processStatus, this->data()->cache(), this->subject()->messageFeedback);
            }
        }

        MWPA::CheckPolicy<T>(this->status().delegate(), this->config().delegate(), this->subject()->channelFeedback);

        return true;
    }

    virtual bool IsSuccess() const
    {
        return true;
    }

    virtual bool IsDone() const
    {
        return !Status::ActivationStatus::IsActive();
    }

    virtual bool Cancel()
    {
        return Deactivate();
    }

    // --------------------------------------------
    // Interface ReactorMethods
    // --------------------------------------------

    virtual bool Error(GeneralException)
    {
        return false;
    }

    virtual bool Complete()
    {
        IINFO() << "Writer completed. Deactivating " << this->Id();
        return Deactivate();
    }

    virtual bool Next(Message<T> message)
    {
        if(!IsActive())
        {
            IWARNING() << "Reader is not active: " << this->Id() << " for channel " << this->data()->handle().third();
            return false;
        }

        for(auto subMessage : message.data()->Messages())
        {
            this->status()->template Next<Message<T>>(&message, subMessage->Kind(), 1);
        }

        return this->data()->flowController()->Next(message);
    }

    // ---------------------------------------------------------
    // Processing
    // ---------------------------------------------------------

    bool Process(MessageKind kind)
    {
        if(!IsActive())
        {
            IINFO() << "Writer is not active: " << this->Id();
            return false;
        }

        std::list<Message<T>> messages;

        switch(kind)
        {
            case MessageKind::Data:
                messages = MWA::BuildMessages<T>(MessagePolicy::Default(), this->data().delegate(), ChangeForReaderStatusKind::UNSENT);
                break;
            case MessageKind::AckNack:
                messages = MWA::BuildMessages<T>(MessagePolicy::Default(), this->data().delegate(), ChangeForReaderStatusKind::REQUESTED);
                break;
            case MessageKind::Heartbeat:
                messages = MWA::BuildHeartbeats<T>(MessagePolicy::Default(), this->data().delegate());
                break;
            case MessageKind::Request:
            case MessageKind::Gap:
                IFATAL() << "Misconfigured message writer";
                break;
        }

        return MWA::SendMessages<T>(messages, this->status().delegate(), this->subject()->channel);
    }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    virtual bool Initialize()
    {
        auto action = this->shared_from_this();

        {
            bool createAsSuspended = true;

            auto dataSchedFuture = ThreadPoolFactory::Instance().GetDefault()->Schedule<bool>(
                [action]() { return action->Process(MessageKind::Data); },
                this->config()->data,
                createAsSuspended
            );

            auto heartbeatSchedFuture = ThreadPoolFactory::Instance().GetDefault()->Schedule<bool>(
                [action]() { return action->Process(MessageKind::Heartbeat); },
                this->config()->heartBeat,
                createAsSuspended
            );

            auto nackSchedFuture = ThreadPoolFactory::Instance().GetDefault()->Schedule<bool>(
                [action]() { return action->Process(MessageKind::AckNack); },
                this->config()->nackResponse,
                createAsSuspended
            );

            this->function().Set(
                new MessageWriterFunctions(
                    heartbeatSchedFuture,
                    nackSchedFuture,
                    dataSchedFuture
                )
            );
        }

        return this->data()->flowController()->Set(action);
    }

    virtual bool IsInitialized() const
    {
        return this->data()->flowController()->IsActionSet();
    }

    // ---------------------------------------------
    // Print data
    // ---------------------------------------------

    std::string toString() const
    {
        std::stringstream ostr;
        ostr << "MessageWriter(" << this->Id() << ")" << std::endl << this->data()->proxies();
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageWriter<T>& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageWriter<T>>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
