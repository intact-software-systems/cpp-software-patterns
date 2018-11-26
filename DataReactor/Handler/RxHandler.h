#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Subject/StateTriggerSubject.h"

namespace Reactor
{

/**
* @brief The RxHandler class is implemented by specific handler types.
*
* Rename to TransitionHandler
*/
template<typename StateType>
class TransitionHandler
        : public Runnable
        , public Templates::ExecuteMethod1<bool, RxThreadPool::Ptr>
        , public Templates::InitTypeMethod<RxThreadPool::Ptr>
        , public Templates::IsTriggeredMethod
        , public Templates::IsDoneMethod
        , public StateTriggerSubject<StateType>
{
public:
    virtual ~TransitionHandler()
    { }

    CLASS_TRAITS(TransitionHandler)

    /**
    * The handler transitions to state
    */
    virtual const StateType &To() const = 0;
};


template<typename StateType>
class TransitionHandlerNoOp : public TransitionHandler<StateType>
{
public:
    TransitionHandlerNoOp(StateType state)
        : state_(state)
    {
    }

    virtual ~TransitionHandlerNoOp()
    {
    }

    CLASS_TRAITS(TransitionHandlerNoOp)

    virtual void run()
    {
    }

    virtual bool Execute(RxThreadPool::Ptr)
    {
        return false;
    }

    virtual bool Init(RxThreadPool::Ptr)
    {
        return false;
    }

    /**
    * @return the state represented by the handler
    */
    virtual const StateType &To() const
    {
        return state_;
    }

    virtual bool IsTriggered() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return true;
    }

private:
    StateType state_;
};

}
