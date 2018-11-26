#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Observer/KeyStateTriggerObserver.h"
#include"DataReactor/Reactor/AsyncScheduler.h"
#include"DataReactor/Reactor/SyncScheduler.h"
#include"DataReactor/Handler/RxHandlerChain.h"

#include"DataReactor/Export.h"

namespace Reactor
{

class DLL_STATE RxSchedulerPolicy
{
public:
    RxSchedulerPolicy(Policy::Timeout timeout = Policy::Timeout::FromMilliseconds(2000))
        : timeout_(timeout)
    {}
    virtual ~RxSchedulerPolicy()
    {}

    const Policy::Timeout& timeout() const
    {
        return timeout_;
    }

    static RxSchedulerPolicy Default()
    {
        return RxSchedulerPolicy();
    }

private:
    Policy::Timeout timeout_;
};

/**
 * @brief The RxSchedulerConfig class
 */
class DLL_STATE RxSchedulerConfig
{
public:
    RxSchedulerConfig(RxSchedulerPolicy rxSchedulerPolicy)
        : rxSchedulerPolicy_(rxSchedulerPolicy)
        , defaultAsyncSchedulerPolicy_(AsyncSchedulerPolicy::Default())
        , defaultSyncSchedulerPolicy_(SyncSchedulerPolicy::Default())
    {}
    virtual ~RxSchedulerConfig()
    {}

    RxSchedulerPolicy rxSchedulerPolicy() const
    {
        return rxSchedulerPolicy_;
    }

    AsyncSchedulerPolicy defaultAsyncSchedulerPolicy() const
    {
        return defaultAsyncSchedulerPolicy_;
    }

    SyncSchedulerPolicy defaultSyncSchedulerPolicy() const
    {
        return defaultSyncSchedulerPolicy_;
    }

private:
    RxSchedulerPolicy       rxSchedulerPolicy_;
    AsyncSchedulerPolicy    defaultAsyncSchedulerPolicy_;
    SyncSchedulerPolicy     defaultSyncSchedulerPolicy_;
};

/**
 * @brief The RxSchedulerState class
 *
 * TODO: Use cache to store schedulers.
 */
template <typename K, typename StateType>
class RxSchedulerState
{
public:
    typedef IMap<K, typename AsyncEventScheduler<K,StateType>::Ptr > AsyncSchedulers;
    typedef IMap<StateType, typename SyncScheduler<K, StateType>::Ptr > SyncSchedulers;

public:
    RxSchedulerState(typename RxHandlerChain<StateType>::Ptr handlerChain)
        : handlerChain_(handlerChain)
    {}
    RxSchedulerState()
    {}
    virtual ~RxSchedulerState()
    {}

    typename RxHandlerChain<StateType>::Ptr HandlerChain() const
    {
        return handlerChain_;
    }

    void SetHandlerChain(typename RxHandlerChain<StateType>::Ptr handlerChain)
    {
        handlerChain_ = handlerChain;
    }

    AsyncSchedulers& GetAsyncSchedulers()
    {
        return asyncSchedulers_;
    }

    SyncSchedulers& GetSyncSchedulers()
    {
        return syncSchedulers_;
    }

private:
    typename RxHandlerChain<StateType>::Ptr handlerChain_;
    AsyncSchedulers asyncSchedulers_;
    SyncSchedulers syncSchedulers_;
};

/**
 * @brief The RxScheduler class
 *
 * SyncSchedulers notifies this about events identified with tuple (K, StateType)
 * AsyncSchdulers are tied to the SyncScheduler through tuple (K, StateType)
 *
 * TODO: Define a set of allowed states, ex, "ON" "OFF" "PAUSE". Define state machine code of the form: GetNext(current-state, current-status, policy) = next-state
 *
 * next-state is associated with a command which executes, and upon finish, next state is associated with a command, etc, until final state is reached.
 *
 * Use Templates::NextStateMethod
 */
template <typename K, typename StateType>
class RxScheduler
        : public Reactor::KeyStateTriggerObserver<K,StateType>
        , public Templates::ContextObject<RxSchedulerConfig, RxSchedulerState<K, StateType> >
{
public:
    RxScheduler(RxSchedulerPolicy policy = RxSchedulerPolicy::Default())
        : Templates::ContextObject<RxSchedulerConfig, RxSchedulerState<K, StateType> >
          (
              RxSchedulerConfig(policy), RxSchedulerState<K, StateType>()
          )
    {}
    virtual ~RxScheduler()
    {}

    CLASS_TRAITS(RxScheduler)

    // -------------------------------------------------------
    // public class functions
    // -------------------------------------------------------

    /**
     * @brief SetHandlerChain Duplicate handler chain to each instance of AsyncScheduler
     */
    void SetHandlerChain(typename RxHandlerChain<StateType>::Ptr handlerChain)
    {
        this->state().SetHandlerChain(handlerChain);
    }

    typename AsyncEventScheduler<K,StateType>::Ptr GetOrCreateAsyncScheduler(K key)
    {
        return getOrCreateAsyncScheduler(key);
    }

    typename SyncScheduler<K, StateType>::Ptr GetOrCreateSyncScheduler(
            typename Templates::EventSelectorMethod<ISet<K>, K>::Ptr eventObserver,
            StateType state)
    {
        return getOrCreateSyncScheduler(eventObserver, state);
    }

    // -------------------------------------------------------
    // Interface Reactor::KeyStateTriggerObserver<K,StateType>
    // -------------------------------------------------------

    /**
     * @brief OnTriggerNext Trigger AsyncScheduler task
     */
    virtual void OnTriggerNext(K key, StateType state)
    {
        // --------------------------------------------------------
        // If asynch scheduler does not exist create one for key.
        // Identify next state(s) in RxHandlerChain
        // trigger the states through trigger function
        // --------------------------------------------------------

        typename AsyncEventScheduler<K, StateType>::Ptr scheduler = getOrCreateAsyncScheduler(key);
        scheduler->OnTriggerNext(state);
    }

    virtual void OnError(K key, StateType )
    {
        this->state().GetAsyncSchedulers().erase(key);
    }

    virtual void OnComplete(K key, StateType )
    {
        this->state().GetAsyncSchedulers().erase(key);
    }

private:

    // -------------------------------------------------------
    // private class functions
    // -------------------------------------------------------

    typename AsyncEventScheduler<K,StateType>::Ptr getOrCreateAsyncScheduler(K key)
    {
        if(this->state().GetAsyncSchedulers().containsKey(key))
        {
            return this->state().GetAsyncSchedulers().get(key);
        }

        // ----------------------------------------------------------------------------
        // Create one AsyncScheduler for every key
        // Initialize AsyncScheduler with predefined chain of events stored in this RxScheduler...
        // ----------------------------------------------------------------------------
        typename AsyncEventScheduler<K,StateType>::Ptr asyncScheduler(
                    new AsyncEventScheduler<K,StateType>(
                        key,
                        this->config().defaultAsyncSchedulerPolicy()
                        )
                    );

        asyncScheduler->SetHandlerChain(this->state().HandlerChain()->Copy());
        asyncScheduler->Connect(this);

        this->state().GetAsyncSchedulers().put(key, asyncScheduler);

        return asyncScheduler;
    }

    typename SyncScheduler<K, StateType>::Ptr getOrCreateSyncScheduler(
            typename Templates::EventSelectorMethod<ISet<K>, K>::Ptr eventObserver,
            StateType state)
    {
        if(this->state().GetSyncSchedulers().containsKey(state))
        {
            return this->state().GetSyncSchedulers().get(state);
        }

        typename SyncScheduler<K, StateType>::Ptr syncScheduler(
                    new SyncScheduler<K, StateType>(
                        eventObserver,
                        state,
                        this->config().defaultSyncSchedulerPolicy()
                        )
                    );

        syncScheduler->Connect(this);

        this->state().GetSyncSchedulers().put(state, syncScheduler);

        return syncScheduler;
    }
};

}
