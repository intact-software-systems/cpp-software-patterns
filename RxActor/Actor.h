#pragma once

#include "CommonDefines.h"
#include "Supervisor.h"
#include "Processor.h"
#include "ActorData.h"
#include "ActorPolicy.h"

namespace RxActor { namespace details {

/**
 * All actors: send and receive capabilities
 *
 * Many to one: Register this actor in RxDiscovery using ActorPath as locator (or similar). Other Actors can lookup this actor via ActorPath.
 * One to Many: Publisher to Subscribers
 * Many to Many: PubSub to PubSubs
 *
 * Is the communication technique an actor trait or a channel trait?
 *
 * Does an Actor need an ActorId?
 * If yes, ActorId == UUID, map to, ActorPath and vice-versa
 *
 * All actors: Create other actors - Use ActorSystem?
 *
 * TODO API:
 *  - Become(Processor<T> p) -> Means that actor is mutable. Avoid
 *  - Execute(ChannelHandle h)
 *  - OnUnhandledDo(std::function<void (T)> f)
 */
template <typename T>
class Actor
    : public RxActor::Actor<T>
      , protected Templates::ContextObjectShared<ActorPolicy, ActorData<T>>
{
public:
    Actor(ChannelHandle handle, ActorId actorId, std::shared_ptr<Processor<T>> processor, Supervisor supervisor, ActorPolicy policy)
        : Templates::ContextObjectShared<ActorPolicy, ActorData<T>>
        (
            new ActorPolicy(policy), new ActorData<T>(processor, handle, actorId, supervisor)
        )
    { }

    virtual ~Actor()
    { }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    /**
     * Create on Actor to be extended, and one Actor to be used with lambdas?
     */
    virtual bool Initialize()
    {
        this->data()->channel_->subscriber()->OnDataDo(
            [=](T value)
            {
                // TODO: Get sender from channel?
                //this->data()->channel_->subscriber()->Sender();

                if(this->data()->processor_)
                {
                    try
                    {
                        this->data()->processor_->Process(this, value);
                    }
                    catch(Exception e)
                    {
                        // TODO: Feedback to supervisor ActorController
                    }
                }
                else
                {
                    Unhandled(value);
                }
            }
        );

        return true;
    }

    virtual bool IsInitialized() const
    {
        return true;
    }

    // ---------------------------------------------------------
    // Send functions
    // ---------------------------------------------------------

    virtual MessageFuture<T> Send(T value)
    {
        return this->data()->channel_->publisher()->Next(value);
    }

    // ---------------------------------------------------------
    // Various functions
    // ---------------------------------------------------------

    const Supervisor& SupervisorStrategy()
    {
        return this->data()->supervisor_;
    }

    virtual ActorProxy<T> Sender() const
    {
        return ActorProxy<T>::NoSender();
    }

    virtual ActorProxy<T> Self() const
    {
        return ActorProxy<T>(this->data()->handle_, this->data()->actorId_);
    }

    virtual void Unhandled(T)
    {
        IWARNING() << "Unhandled value ";
    }

    virtual Policy::Deadline ReceiveDeadline() const
    {
        return Policy::Deadline::FromSeconds(5);
    }

    // ---------------------------------------------------------
    // Lifecycle methods that is called when an actor is started,
    // stopped or restarted.
    // ---------------------------------------------------------

    virtual void OnPreStart()
    {
        this->data()->processor_->OnPreStart();
    }

    virtual void OnPostStop()
    {
        this->data()->processor_->OnPostStop();
    }

    virtual void OnPreRestart(BaseLib::Exception e)
    {
        this->data()->processor_->OnPreRestart(e);
    }

    virtual void OnPostRestart(BaseLib::Exception e)
    {
        this->data()->processor_->OnPostRestart(e);
    }
};

}}



