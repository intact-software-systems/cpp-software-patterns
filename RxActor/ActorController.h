#pragma once

#include "ActorProxy.h"
#include "ActorControllerPolicy.h"
#include "ActorControllerData.h"
#include "Supervisor.h"

namespace RxActor {

/**
 * All actors: send and receive capabilities
 *
 * Controller control one or many Actors:
 * - An Actor is a reader of one channel and is triggered iff a channel is written to
 * - An Actor can write to any channel when receiving data
 * - An Actor can create a child Actor
 * - An Actor can mutate its own state
 * - An Actor cannot be executed unless there is input
 * - Actors form an ActorGraph that represents the creation order and hierarchy
 *
 *
 * ActorController
 * - Hold the ActorGraph
 * - Supervise the Actors
 * - Trigger Supervisor strategy upon Actor Error
 * - Convenience send functions through Actors
 *
 *
 * In a state machine:
 * - Actor for input
 * - 1-N actors for output, based on input
 *
 * (InputActor, OutputActor1 ... N) = A state with transition rules
 */
template <typename T>
class ActorController
    : protected Templates::ContextObjectShared<ActorControllerPolicy, ActorControllerData<T>>
{
public:
    ActorController(ActorGraph* actorGraph, ActorControllerPolicy* config)
        : Templates::ContextObjectShared<ActorControllerPolicy, ActorControllerData<T>>
        (
            config, new ActorControllerData<T>(actorGraph, RxActor::Supervisor(std::function<Policy::ReactionKind(BaseLib::Exception)>(), new SupervisorPolicy()))
        )
    { }

    virtual ~ActorController()
    { }

    void Start()
    {
        // iterate through all actors and start (preStart, etc)
    }

    void Stop()
    {
        // iterate through all actors and stop (preStop, etc)
    }

    void Restart()
    {
        // iterate through all actors and restart (preRestart, etc)
    }

    // ---------------------------------------------------------
    // Mutating functions, should I remove them?
    // ---------------------------------------------------------

    virtual ActorProxy<T> Watch(const ActorProxy<T>& actor)
    {
        // Add to watch-list
        return ActorProxy<T>::NoSender();
    }

    virtual ActorProxy<T> Unwatch(const ActorProxy<T>& actor)
    {
        return ActorProxy<T>::NoSender();
    }

    // ---------------------------------------------------------
    // Send functions
    // ---------------------------------------------------------

    MessageFuture<T> SendVia(T value, ActorId actorId)
    {
        // TODO: Find actor in ActorGraph and send through.
        return MessageFuture<T>::Empty();
    }

    // -----------------------------------------------------------
    // Interface ActorObserver<T>
    // -----------------------------------------------------------

    // Feedback (Errors) to controller from Actors
    // - Based on process input in one Actor - Trigger the Actor's associated supervisor strategy Restart/Resume/Stop
    // - Based on feedback from channel


    virtual void OnError(BaseLib::Exception exception)
    {
        Policy::ReactionKind kind = this->data()->supervisor.Decide(exception);

        switch(kind)
        {
            // Depending on all-for-one or one-for-one
            case Policy::ReactionKind::ESCALATE:
                // Escalates the failure to the supervisor of the supervisor, by rethrowing the cause of the failure, i.e.
                break;
            case Policy::ReactionKind::RESTART:
                // Discards the old Actor instance and replaces it with a new, then resumes message processing.
                break;
            case Policy::ReactionKind::RESUME:
                // Resumes message processing for the failed Actor
                break;
            case Policy::ReactionKind::STOP:
                // Stops the Actor
                break;
        }
    }

    // -----------------------------------------------------------
    // CRUD towards Actors - Create, Read, Update, Delete
    // NOTE! Not a good idea to allow the ActorGraph to be mutated while active
    // RULE: Cannot mutate ActorGraph after creation. Only mutate status of Actors.
    // -----------------------------------------------------------

//    virtual void OnActorCreated(Actor<T>* parent, Actor<T>* child)
//    {
//        // Update ActorGraph
//    }
//
//    virtual void OnActorDeleted(Actor<T>* parent, Actor<T>* child)
//    {
//        // Update ActorGraph
//    }
};

}
