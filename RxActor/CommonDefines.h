#pragma once

#include"RxActor/IncludeExtLibs.h"
#include"RxActor/Export.h"

namespace RxActor {

// ----------------------------------------------------------------
// Forward declarations
// ----------------------------------------------------------------

/**
 * Holds the logic for executing the "actor logic", and supervisor policies for its actor children.
 *
 * Has the ability to: Send data to any other Actor, create child actors, change its own behavior based on input.
 */
//template <typename T>
//class Actor;

/**
 * Attached to an actor and supervises its children based on policies, i.e., restart policy (timing, counters).
 *
 * Idea: Attach an actor to a supervisor instance, like a command is attached to a command-controller.
 *
 * External API: Restart(), Resume(), Escalate(), Stop().
 */
class Supervisor;

/**
 * From Akka doc: An actor system is a hierarchical group of actors which share common configuration, e.g. dispatchers, deployments,
 * remote capabilities and addresses. It is also the entry point for creating or looking up actors.
 *
 * ActorSystem is similar to a factory, for example, CacheFactory.
 */
class ActorSystem;

/**
 * Lookup of ActorSystem instances
 */
class ActorSystems;

/**
 *  Compose an actor system. An ActorComposer should support:
 *  - Content-based routing
 *  - Message filter on content, etc
 *  - Dynamic router
 *  - Message transformation
 *  - Selective consumers (based on routers).
 *
 *  For example: composer.actor(a).routeTo(b).or(c).or(d).criteria(routerFunction);
 */
template <typename T>
class ActorComposer;

/**
 * Stores actors for a given system represented as an undirected graph. See boost graph
 *
 * class ActorGraph<Actor<T>, Channel<T>>, where Actor<T> is Vertex, and Channel<T> is Edge
 */
class ActorGraph;

/**
 * ActorProxy is a proxy for the Actor, without logic
 */
template <typename T>
class ActorProxy;

// ----------------------------------------------------------------
// Base classes
// ----------------------------------------------------------------

class ActorBase
    : public Status::ActivationStatus
      , public Templates::InitializeMethod
{
public:
    virtual ~ActorBase()
    { }

    virtual void OnPreStart()                                = 0;
    virtual void OnPostStop()                                = 0;
    virtual void OnPreRestart(BaseLib::Exception exception)  = 0;
    virtual void OnPostRestart(BaseLib::Exception exception) = 0;
};

template <typename T>
class Actor : public ActorBase
{
public:
    virtual ~Actor()
    { }

    virtual MessageFuture<T> Send(T value) = 0;

    virtual ActorProxy<T> Sender() const = 0;
    virtual ActorProxy<T> Self() const   = 0;

    virtual void Unhandled(T) = 0;
};

// ----------------------------------------------------------------
// Typedefs and enums
// ----------------------------------------------------------------

/**
 * AllForOneStrategy applies the fault handling Directive (Resume, Restart, Stop) specified in the Decider to all children when one fails,
 * as opposed to OneForOneStrategy that applies it only to the child actor that failed.
 */
enum class SupervisorStrategy
{
    OneForOne,
    AllForOne
};

typedef std::function<Policy::ReactionKind(BaseLib::Exception)> DeciderAlgorithm;
typedef Status::EventStatusTracker<Policy::ReactionKind, int>   SupervisorStatus;

/**
 * ActorPath should take the place of ChannelId in ChannelHandle
 */
typedef ChannelId      ActorPath;
typedef BaseLib::GUUID ActorId;

typedef std::shared_ptr<ActorBase> ActorBasePtr;

/**
 * An actor's status
 */
enum class ActorStatusKind
{
    NO = 'N',
    STARTING = 'S',
    RUNNING = 'R',
    SUSPENDED = 'P',
    RESUMED = 'M',
    FAILURE = 'E',
    HALTED = 'H',
};

typedef Status::EventStatusTracker<ActorStatusKind, int> ActorStatus;

}
