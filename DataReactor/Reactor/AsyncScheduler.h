#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Reactor/RxTaskExecutor.h"
#include"DataReactor/Observer/StateTriggerObserver.h"
#include"DataReactor/Subject/KeyStateTriggerSubject.h"
#include"DataReactor/Handler/RxHandlerChain.h"
#include"DataReactor/Handler/RxHandler.h"
#include"DataReactor/Condition/StateCondition.h"

#include"DataReactor/Export.h"

namespace Reactor
{

/**
 * @brief The AsyncSchedulerPolicy class
 */
class DLL_STATE AsyncSchedulerPolicy
{
public:
    AsyncSchedulerPolicy(Policy::Timeout timeout = Policy::Timeout::FromMilliseconds(2000))
        : timeout_(timeout)
    {}
    virtual ~AsyncSchedulerPolicy()
    {}

    const Policy::Timeout& timeout() const
    {
        return timeout_;
    }

    void setTimeout(const Policy::Timeout &timeout)
    {
        timeout_ = timeout;
    }

    static AsyncSchedulerPolicy Default()
    {
        return AsyncSchedulerPolicy();
    }

private:
    Policy::Timeout timeout_;
};

/**
 * @brief The AsyncSchedulerState class
 */
template <typename K, typename StateType>
class AsyncSchedulerState
{
public:
    typedef IMap<StateType, RxTaskExecutor<StateType>*> TaskExecutors;

public:
    AsyncSchedulerState(K key)
        : key_(key)
    {}
    virtual ~AsyncSchedulerState()
    {}

    const K& Key() const
    {
        return key_;
    }

    void SetHandlerChain(typename RxHandlerChain<StateType>::Ptr handlerChain)
    {
        handlerChain_ = handlerChain;
    }

    typename RxHandlerChain<StateType>::Ptr HandlerChain()
    {
        return handlerChain_;
    }

    TaskExecutors& Executors()
    {
        return taskExecutors_;
    }

private:
    K key_;

    typename RxHandlerChain<StateType>::Ptr handlerChain_;

    TaskExecutors taskExecutors_;
};

/**
 * @brief The AsyncScheduler class has one instance for every unique key. It is reponsible for executing a chain
 * of handlers attached to it.
 *
 * TODO:
 *  DONE - I need to represent the transitions between states. DONE: Implemented StateTransition
 *  - I need to store the state machine. Alternatives: Directed acyclic graph, undirected graph,
 *  - I need to control the state machine. Use state machine collections?
 *  - I need to control the number of threads
 *  - Should handlers be executed concurrently? Probably yes
 *
 * TODO:
 * - Instead of parameterizing with K, use AnyKey? Alternative without key?
 *
 */
template <typename K, typename StateType>
class AsyncEventScheduler
        : public Runnable
        , public RxScheduleSubject<Runnable*>
        , public StateTriggerObserver<StateType>
        , public KeyStateTriggerSubject<K, StateType>
        , public Templates::ContextObject<AsyncSchedulerPolicy, AsyncSchedulerState<K, StateType> >
{
public:
    AsyncEventScheduler(K key, AsyncSchedulerPolicy policy = AsyncSchedulerPolicy::Default())
        : Templates::ContextObject<AsyncSchedulerPolicy, AsyncSchedulerState<K, StateType> >
          (
              policy, AsyncSchedulerState<K, StateType>(key)
          )
    {}
    virtual ~AsyncEventScheduler()
    {}

    CLASS_TRAITS(AsyncEventScheduler)

    void AddHandler(StateType state, typename TransitionHandler<StateType>::Ptr handler)
    {
        addHandler(state, handler);
    }

    /**
     * TODO: I have to populate the commands in trigger handler with KEY.
     *
     * Perhaps I can find out when I implement "aggregate results" through Composition policy
     */
    void SetHandlerChain(typename RxHandlerChain<StateType>::Ptr handlerChain)
    {
        this->state().SetHandlerChain(handlerChain);

        for(typename RxHandlerChain<StateType>::HandlerMap::iterator
            it = this->state().HandlerChain()->Handlers().begin(),
            it_end = this->state().HandlerChain()->Handlers().end();
            it != it_end; ++it)
        {
            StateVertex<StateType, typename TransitionHandler<StateType>::Ptr > &stateVertex = it->second;
            addHandler(stateVertex.Key(), stateVertex.Value());
        }
    }

//    void SetStateMachine(typename StateMachineChain<StateType>::Ptr stateMachine)
//    {

//    }

    const K& Key() const
    {
        return this->state().Key();
    }

    bool operator == (const AsyncEventScheduler<K, StateType> &async)
    {
        return this->state().Key() == async.state().Key();
    }

    // -------------------------------------------------------------------------
    // Interface StateTriggerObserver
    // -------------------------------------------------------------------------

    virtual void OnComplete(StateType state)
    {
        // TODO: Instead of Key, use this? Then operator == can be used. See DataAccess
        KeyStateTriggerSubject<K,StateType>::OnComplete(this->state().Key(), state);
    }

    virtual void OnTriggerNext(StateType state)
    {
        processTriggerNext(state);
    }

    virtual void OnError(StateType state, BaseLib::Exception )
    {
        KeyStateTriggerSubject<K,StateType>::OnError(this->state().Key(), state);
    }

private:

    // -------------------------------------------------------------------------
    // private functions
    // -------------------------------------------------------------------------

    void processTriggerNext(StateType state)
    {
        typename RxHandlerChain<StateType>::HandlerMap::iterator it = this->state().HandlerChain()->Handlers().find(state);

        // -- debug --
        ASSERT(it != this->state().HandlerChain()->Handlers().end());
        // -- debug --

        StateVertex<StateType, typename TransitionHandler<StateType>::Ptr > &stateVertex = it->second;

        for(typename StateVertex<StateType, typename TransitionHandler<StateType>::Ptr> ::AdjacencyList::iterator
            it = stateVertex.Neighbors().begin(),
            it_end = stateVertex.Neighbors().end();
            it != it_end;
            it++)
        {
            StateType neighbor = *it;
            triggerHandler(neighbor);
        }
    }

    void addHandler(StateType state, typename TransitionHandler<StateType>::Ptr handler)
    {
        RxTaskExecutor<StateType>* taskExecutor = new RxTaskExecutor<StateType>(handler);

        // --------------------------------------
        // Note: Create communication pattern
        // rxhandler -> taskExecutor -> asyncscheduler
        // --------------------------------------
        taskExecutor->Connect(this);
        handler->Connect(taskExecutor);

        // Note: Initialization with thread pool must be done from outside already.
        //handler->Init(rxThreadPool);

        RxScheduleSubject::OnInit(taskExecutor);

        this->state().Executors().put(state, taskExecutor);
    }

    void triggerHandler(StateType state)
    {
        ASSERT(this->state().Executors().containsKey(state));

        RxScheduleSubject::OnReady(this->state().Executors().at());
    }
};

}
