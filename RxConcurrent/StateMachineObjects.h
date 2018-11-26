#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/StateMachineMethods.h"

namespace BaseLib { namespace Concurrent {

/**
 * @brief The StateMachineObjectBase class
 */
class StateMachineObjectBase
{
    virtual bool ExecuteNext() = 0;
};

/**
 * @brief The StateMachineMethods class
 *
 * TODO: typename LOCK = NullReadWriteLock - makes this synchronized or not
 * TODO: virtual bool ExecutePrevious()? Is it necessary?
 * TODO: Implement a StateMachineController?
 *
 */
template <typename StateType>
class StateMachineObjects
        : public StateMachineMethods<StateType>
        , public StateMachineObjectBase
{
private:
    typedef StateMachineMethods<StateType> SM;

public:
    StateMachineObjects()
        : StateMachineMethods<StateType>()
    {}
    virtual ~StateMachineObjects()
    {}

    CLASS_TRAITS(StateMachineObjects)

    /**
     * true if finished, otherwise false
     */
    virtual bool ExecuteNext()
    {
        if(SM::isDone())
        {
            // -- debug --
            IDEBUG() << "State machine reached final state";
            // -- debug --

            return false;
        }

        // ------------------------------------
        // Set start state
        // ------------------------------------
        if(!SM::currentState_)
        {
            SM::currentState_ = SM::getStartState();

            // -- debug --
            ASSERT(SM::currentState_);
            // -- debug --
        }

        // ------------------------------------
        // Execute the currentState_ by calling its run()
        // ------------------------------------
        bool isExecuted = SM::executeTask(SM::currentState_);

        // -- debug --
        ASSERT(isExecuted);
        // -- debug --

        if(SM::isFinalState(SM::currentState_->GetStateType()))
        {
            // -- debug --
            IDEBUG() << "Final state reached!";
            // -- debug --

            SM::setDone(true);
        }
        else // not finished, get next state
        {
            SM::currentState_ = SM::getState(SM::currentState_->GetNextState());
        }

        return SM::isDone();
    }

    /**
     * @brief ResetToStart
     */
    void ResetToStart()
    {
        SM::currentState_ = SM::getStartState();
        SM::setDone(false);
    }

    /**
     * @brief SetNextState
     * @param stateType
     * @return
     */
    bool SetNextState(StateType stateType)
    {
        typename State<StateType>::Ptr nextState = SM::getState(stateType);
        if(nextState == NULL)
        {
            IDEBUG() << "State not found " << stateType;
            return false;
        }

        SM::currentState_ = nextState;

        return true;
    }
};

}}
