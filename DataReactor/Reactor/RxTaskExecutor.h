#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Handler/RxHandler.h"
#include"DataReactor/Subject/StateTriggerSubject.h"
#include"DataReactor/Condition/StateCondition.h"
#include"DataReactor/Export.h"

namespace Reactor
{

/**
 * @brief The HandlerPolicy class
 */
class DLL_STATE RxTaskExecutorPolicy
{};

template <typename StateType>
class RxTaskExecutorState
{
public:
    RxTaskExecutorState(
            typename TransitionHandler<StateType>::Ptr handler,
            TransitionCondition::Ptr condition)
        : handler_(handler)
        , condition_(condition)
    {}
    virtual ~RxTaskExecutorState()
    {}

    const StateType& State() const
    {
        return handler_->To();
    }

    typename TransitionHandler<StateType>::Ptr Handler() const
    {
        return handler_;
    }

    TransitionCondition::Ptr Condition()
    {
        return condition_;
    }

private:
    typename TransitionHandler<StateType>::Ptr handler_;
    TransitionCondition::Ptr condition_;
};

/**
 * Strategy performs the algorithm to "Read data from socket" "Create datagrams" "Deserialize data"
 * TriggerCondition performs the check "Is the condition to trigger 'create datagram' met?"
 * TriggerCondition holds the GuardCondition and a simple conditional check based on the Return value.
 * Return value can be a number "Number of bytes available in buffer" or "Number of datagrams created"
 *
 * This object is thread safe and "stateless" in the sense that only attached commands are executed,
 * hence it can be run by many threads concurrently.
 */
template <typename StateType>
class RxTaskExecutor
        : public Runnable
        , public StateTriggerSubject<StateType>
        , public Templates::ContextObject<RxTaskExecutorPolicy, RxTaskExecutorState<StateType> >
{
public:
    RxTaskExecutor(
            typename TransitionHandler<StateType>::Ptr handler,
            TransitionCondition::Ptr condition = TransitionCondition::Ptr())
        : Templates::ContextObject<RxTaskExecutorPolicy, RxTaskExecutorState<StateType> >
          (
              RxTaskExecutorPolicy(), RxTaskExecutorState<StateType>(handler, condition)
          )
    { }
    virtual ~RxTaskExecutor()
    { }

    CLASS_TRAITS(RxTaskExecutor)

    /**
     * @brief run
     *
     * TODO: The executed handler is triggered asynchronously and notifies completion through callbacks.
     */
    virtual void run()
    {
        try
        {
            this->state().Handler()->run();

            if(this->state().Handler()->IsDone())
            {
                StateTriggerSubject<StateType>::OnComplete(this->state().State());
            }
        }
        catch(BaseLib::GeneralException &exception)
        {
            StateTriggerSubject<StateType>::OnError(this->state().State(), exception);
        }
    }
};

}
