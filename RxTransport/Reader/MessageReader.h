#pragma once

#include "RxTransport/CommonDefines.h"
#include "MessageReaderFunctions.h"
#include "MessageReaderData.h"
#include "MessageReaderAlgorithms.h"
#include "MessageReaderPolicyAlgorithms.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
struct MessageReaderSubjects
{
    RxObserverSubject<Message<T>>   channel;
    ChannelFeedbackSubjectForReader channelFeedback;
    MessageFeedbackSubjectForReader messgeFeedback;
};

template <typename T>
class MessageReader
    : public MessageReaderBase
      , public Templates::ReactorMethods<bool, Message<T>, BaseLib::GeneralException>
      , public Templates::ContextObjectShared<MessageReaderPolicy, MessageReaderData<T>, MessageReaderStatus, MessageReaderSubjects<T>, MessageReaderFunctions>
      , public std::enable_shared_from_this<MessageReader<T>>
{
    typedef MessageReaderAlgorithms       MRA;
    typedef MessageReaderPolicyAlgorithms MRPA;

public:
    MessageReader(ChannelHandle handle, ReaderId readerId, MessageReaderPolicy policy = MessageReaderPolicy::Default())
        : MessageReaderBase(readerId)
        , Templates::ContextObjectShared<MessageReaderPolicy, MessageReaderData<T>, MessageReaderStatus, MessageReaderSubjects<T>, MessageReaderFunctions>
            (
                new MessageReaderPolicy(policy),
                new MessageReaderData<T>(handle, readerId, policy.flowControllerPolicy),
                new MessageReaderStatus(),
                new MessageReaderSubjects<T>()
            )
    { }

    virtual ~MessageReader()
    { }

    // --------------------------------------------
    // Access functions
    // --------------------------------------------

    MessageWriterProxies& Proxies()
    {
        return this->data()->proxies();
    }

    RxObserverSubject<Message<T>>& MessageChannel()
    {
        return this->subject()->channel;
    }

    MessageFeedbackSubjectForReader& MessageFeedbackChannel()
    {
        return this->subject()->messgeFeedback;
    }

    ChannelFeedbackSubjectForReader& ChannelFeedbackChannel()
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
            IWARNING() << "Reader is not active: " << this->Id() << " for channel " << this->data()->handle().third();
            return false;
        }

        std::list<Message<T>> messages = this->data()->flowController()->Pull();

        if(!messages.empty())
        {
            MessageStatus processStatus = MRA::Process<T>(messages, this->data().delegate());

            if(processStatus.IsStatus())
            {
                MRA::TriggerFollowUp(this->function().delegate(), processStatus);
                MRPA::PolicyViolations<T>(this->data()->proxies(), processStatus, this->data()->cache(), this->subject()->messgeFeedback);
            }
        }

        MRPA::CheckPolicy<T>(this->status().delegate(), this->config().delegate(), this->subject()->channelFeedback);

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
    // Interface RxObserver
    // --------------------------------------------

    virtual bool Complete()
    {
        IINFO() << "Reader completed. Deactivating " << this->Id();
        return Deactivate();
    }

    virtual bool Next(Message<T> message)
    {
        for(auto subMessage : message.data()->Messages())
        {
            this->status()->NextState(subMessage->Kind(), 1);
        }

        return this->data()->flowController()->Next(message);
    }

    virtual bool Error(GeneralException)
    {
        return true;
    }

    // ---------------------------------------------------------
    // Process method
    // ---------------------------------------------------------

    bool Process(MessageKind kind)
    {
        if(!IsActive())
        {
            IWARNING() << "Reader is not active: " << this->Id() << " for channel " << this->data()->handle().third();
            return false;
        }

        std::list<Message<T>> messages = MRA::BuildMessages<T>(MessagePolicy::Default(), this->data().delegate(), kind);

        return MRA::SendMessages<T>(messages, this->status().delegate(), this->subject()->channel);
    }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    virtual bool Initialize()
    {
        auto action = this->shared_from_this();

        {
            bool createAsSuspended = true;

            auto ackNackTask = ThreadPoolFactory::Instance().GetDefault()->Schedule<bool>(
                [action]() { return action->Process(MessageKind::AckNack); },
                this->config()->heartBeatResponseTask,
                createAsSuspended
            );

            auto requestTask = ThreadPoolFactory::Instance().GetDefault()->Schedule<bool>(
                [action]() { return action->Process(MessageKind::Request); },
                this->config()->requestTask,
                createAsSuspended
            );

            this->function().Set(new MessageReaderFunctions(ackNackTask, requestTask));
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
        ostr << "MessageReader(" << this->Id() << "): " << std::endl << this->data()->proxies();
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageReader<T>& reader)
    {
        ostr << reader.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageReader<T>>& reader)
    {
        ostr << reader->toString();
        return ostr;
    }
};

}}
