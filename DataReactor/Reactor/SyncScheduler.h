#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Handler/RxHandler.h"
#include"DataReactor/Condition/StateCondition.h"
#include"DataReactor/Subject/KeyStateTriggerSubject.h"
#include"DataReactor/Export.h"

namespace Reactor
{

class DLL_STATE SyncSchedulerPolicy
{
public:
    SyncSchedulerPolicy(Policy::Timeout timeout = Policy::Timeout::FromMilliseconds(2000))
        : timeout_(timeout)
    {}
    virtual ~SyncSchedulerPolicy()
    {}

    const Policy::Timeout& timeout() const
    {
        return timeout_;
    }

    static SyncSchedulerPolicy Default()
    {
        return SyncSchedulerPolicy();
    }

private:
    Policy::Timeout timeout_;
};

/**
 * @brief The SyncSchedulerState class
 */
template <typename K, typename StateType>
class SyncSchedulerState
{
public:
    SyncSchedulerState(typename BaseLib::Templates::EventSelectorMethod<ISet<K>, K>::Ptr selector, StateType state)
        : eventSelector_(selector)
        , state_(state)
    {}
    virtual ~SyncSchedulerState()
    {}

    typename BaseLib::Templates::EventSelectorMethod<ISet<K>, K>::Ptr& Selector()
    {
        return eventSelector_;
    }

    const StateType& State() const
    {
        return state_;
    }

    RxData::ObjectAccessProxy<K,K> Access()
    {
        return RxData::ObjectAccessProxy<K,K>(cache(), GetTypeName());
    }

    typename RxData::ObjectHome<K,K>::Ptr Home()
    {
        return cache()->template getOrCreateHome<K,K>(GetTypeName());
    }

private:
    std::string GetTypeName() const
    {
        return BaseLib::Utility::GetTypeName<K>();
    }

    RxData::Cache::Ptr cache()
    {
        return RxData::CacheFactory::Instance().getOrCreateCache(GetTypeName());
    }

private:
    typename BaseLib::Templates::EventSelectorMethod<ISet<K>, K>::Ptr eventSelector_;
    StateType       state_;
};

/**
 * @brief The SyncScheduler class
 *
 * TODO:
 * - Is it possible to split the implementation of this class to avoid so many template types?
 *
 * - IDEA: Could the SyncScheduler be a special case of an RxHandler?
 *         - RxObserver<StateType, TaskResult> where
 *         - StateType == StateType
 *         - K = TaskResult
 *         - EventObserver is the only special type
 *
 * - Perhaps RxHandler is the base event handler and this SyncScheduler is a long-running RxTriggerHandler or something?
 *      - A handler - handles events
 *      - An event listener - trigger events
 *      - This is an event listener.
 *      - This is an external event listener, i.e., Network I/O, etc.
 *
 * Application:
 *  - SyncSchedulere, i.e., EventSelector, can be an observer to the value K (AbstractSocket::Ptr) populated by socket acceptor
 *
 */
template <typename K, typename StateType>
class SyncScheduler
        : public TransitionHandler<StateType>
        , public KeyStateTriggerSubject<K, StateType>
        , public RxData::ObjectObserver<K>
        , public Templates::ContextObject<SyncSchedulerPolicy, SyncSchedulerState<K, StateType> >
{
public:
    SyncScheduler(typename Templates::EventSelectorMethod<ISet<K>, K>::Ptr eventSelector,
                  StateType state,
                  SyncSchedulerPolicy policy = SyncSchedulerPolicy::Default())
        : Templates::ContextObject<SyncSchedulerPolicy, SyncSchedulerState<K, StateType> >
          (
              policy,
              SyncSchedulerState<K, StateType>(eventSelector, state)
          )
    {
        this->state().Home()->Connect(this);
    }

    virtual ~SyncScheduler()
    {
        this->state().Home()->Disconnect(this);
    }

    CLASS_TRAITS(SyncScheduler)

    // --------------------------------------------------------
    // Interface RxHandler<StateType>
    // --------------------------------------------------------

    virtual void run()
    {
        // --------------------------------------------------------
        // TODO: Move this to a Command like "LongRunningCommand"?
        // IDEA: Use CommandController - links up with rx scheduler
        //       Attach command with "long running task" qos policy.
        //       Use a SyncCommandController to trigger the individual event listeners based on i/o "ready events"
        // --------------------------------------------------------

        try
        {
            ISet<K> events = this->state().Selector()->Select(this->config().timeout().InMillis());

            triggerEvents(events);
        }
        catch(BaseLib::GeneralException exception)
        {
            KeyStateTriggerSubject<K,StateType>::OnError(this->state().State(), exception);
        }
    }

    virtual bool Execute(RxThreadPool::Ptr )
    {
        return false; //this->GetState().Executor()->Execute(threadPool);
    }

    virtual bool Init(RxThreadPool::Ptr )
    {
        //this->GetState().Executor()->Connect(threadPool.get());
        return true;
    }

    virtual const StateType& To() const
    {
        return this->state().State();
    }

    virtual bool IsTriggered() const
    {
        return true;
    }

    virtual bool IsDone() const
    {
        return false;
    }

    // --------------------------------------------------------
    // Interface ObjectObserver<K>
    // --------------------------------------------------------

    virtual bool OnObjectCreated(K )
    {
        // Populate selector?
        return true;
    }

    virtual bool OnObjectDeleted(K )
    {
        // Remove from selector
        return true;
    }

    virtual bool OnObjectModified(K )
    {
        // Update selector
        return true;
    }

private:

    void triggerEvents(const ISet<K> &events)
    {
        if(events.empty()) return;

        for(typename ISet<K>::const_iterator it = events.begin(), it_end = events.end(); it != it_end; ++it)
        {
            KeyStateTriggerSubject<K,StateType>::OnTriggerNext(*it, this->state().State());
        }
    }
};

}
