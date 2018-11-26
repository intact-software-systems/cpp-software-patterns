#pragma once

#include <RxTransport/Reader/MessageReaders.h>
#include <RxTransport/Writer/MessageWriters.h>
#include"RxTransport/CommonDefines.h"

#include"MessageControllerPolicy.h"
#include"MessageControllerData.h"
#include"MessageControllerAlgorithms.h"
#include"MessageBuilder.h"

namespace BaseLib { namespace Concurrent {

/**
 * May represent one writer, publisher or group that writes and reads messages.
 *
 * speed(writer) <= speed(reader)
 *
 * Reactive streams = Dynamic push/pull (dynamic adjustment)
 *
 * I need a channel between controllers for sending messages (like in RTPS)
 *
 * controller <--> controller
 *
 * Support flows (unicast, multicast, broadcast)
 *  - Message(from,to), Message(from, to-group), Message(from, to-all)
 *
 *
 * Scheduling
 * - I need to support more advanced scheduling patterns.
 * - See existing scheduler approach and expand FlowControllerAction
 *
 * Support list of policies (Interval, Attempt, etc) and schedule action accordingly
 *
 * TODO: MessageController seems to be a MessageRouter currently -> Don't really need the OnNext, On... functions....
 */
template <typename T>
class MessageController
    : public MessageControllerBase
      , public RxObserver<Message<T>>
      , public Templates::ReactorMethods<MessageFuture<T>, std::pair<MessagePolicy, DataHolder<T>>, BaseLib::GeneralException>
      , public Templates::ContextObjectShared<MessageControllerPolicy, MessageControllerData<T>, SequenceNumberCounter>
      , public std::enable_shared_from_this<MessageController<T>>
{
    typedef MessageControllerAlgorithms MCA;

public:
    MessageController(ChannelHandle handle, MessageControllerPolicy policy)
        : MessageControllerBase(handle)
        , Templates::ContextObjectShared<MessageControllerPolicy, MessageControllerData<T>, SequenceNumberCounter>
            (
                new MessageControllerPolicy(policy),
                new MessageControllerData<T>(handle, FlowControllerPolicy::Default()),
                new SequenceNumberCounter(Duration::FromMinutes(10), 100)
            )
    { }

    virtual ~MessageController()
    { }

    // --------------------------------------------
    // Access state and set state
    // --------------------------------------------

    MessageReaders<T>& Readers()
    {
        return this->data()->readers();
    }

    MessageWriters<T>& Writers()
    {
        return this->data()->writers();
    }

    // --------------------------------------------
    // Interface ActivationStatus
    // --------------------------------------------

    virtual bool Activate() override
    {
        this->data()->readers().Activate();
        this->data()->writers().Activate();
        return Status::ActivationStatus::Activate();
    }

    virtual bool Deactivate() override
    {
        this->data()->readers().Deactivate();
        this->data()->writers().Deactivate();
        return Status::ActivationStatus::Deactivate();
    }

    // --------------------------------------------
    // Interface Action<bool>
    // --------------------------------------------

    virtual bool Invoke()
    {
        std::list<Message<T>> messages = this->data()->flowController()->Pull();
        return !messages.empty()
               ? MCA::RouteMessages<T>(messages, this->data().delegate())
               : false;
    }

    virtual bool IsSuccess() const
    {
        return true;
    }

    virtual bool IsDone() const
    {
        return !IsActive();
    }

    virtual bool Cancel()
    {
        return Deactivate();
    }

    // --------------------------------------------
    // Pull-based communication
    // --------------------------------------------

    Message<T> Request(ReaderId id, int n)
    {
        if(!IsActive()) return Message<T>::Empty();

        return MCA::PullRequest<T>(id, n, this->data()->flowController(), this->data()->handle());
    }

    // --------------------------------------------
    // Interface ReactorMethods
    // --------------------------------------------

    virtual bool Complete()
    {
        IINFO() << "Controller completed " << this->Id();
        return Deactivate();
    }

    virtual MessageFuture<T> Next(std::pair<MessagePolicy, DataHolder<T>> data)
    {
        if(!IsActive()) return MessageFuture<T>::Empty();

        return MCA::RouteData<T>(data.first, data.second, this->data().delegate(), this->status().delegate());
    }

    virtual bool Error(GeneralException exception)
    {
        IWARNING() << "Received exception " << this->Id() << " :" << exception.msg();
        return false;
    }

    // --------------------------------------------
    // Interface RxObserver
    // --------------------------------------------

    virtual void OnComplete()
    {
        IINFO() << "Controller completed " << this->Id();
        Deactivate();
    }

    virtual void OnNext(Message<T> message)
    {
        if(!IsActive()) return;

        this->data()->flowController()->Next(message);
    }

    virtual void OnError(GeneralException exception)
    {
        IWARNING() << "Received exception " << this->Id() << " :" << exception.msg();
    }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    virtual bool Initialize()
    {
        auto action = this->shared_from_this();
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
        ostr << "MessageController<T>(" << this->Id() << ")" << std::endl << this->data()->readers() << this->data()->writers();
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageController<T>& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageController<T>>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
