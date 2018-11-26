#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/FutureTasks.h"

namespace BaseLib { namespace Concurrent {

/**
 * @brief The StateBase class
 */
class StateBase : public Runnable
{
public:
    CLASS_TRAITS(StateBase)
};

/**
 * @brief The State class
 *
 * TODO: Decouple the execution from the State representation?
 * TODO: Representing the state as an adjacency list is not needed as long as the next state is decided by the
 * user implementation of the state function and its return value.
 *
 * Ex: StateExecutor : public Runnable which takes as input the State
 */
template <typename StateType>
class State : public StateBase
            , public ENABLE_SHARED_FROM_THIS_T1(State, StateType)
{
public:
    State(StateType stateType)
        : stateType_(stateType)
    {}
    virtual ~State()
    {}

    CLASS_TRAITS(State)

    virtual void run()
    {
        futureTasks_.run();

        InstanceHandleSet handleSet = futureTasks_.GetHandlesWithReturnType<StateType>();
        for(InstanceHandleSet::iterator it = handleSet.begin(), it_end = handleSet.end(); it != it_end; ++it)
        {
            ASSERT(futureTasks_.Result<StateType>(*it).first.IsReady());

            nextState_ = futureTasks_.Result<StateType>(*it).second;
        }
    }

    typename State::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    template <typename Class>
    typename State::Ptr AddAction(Class* action, StateType (Class::*member)() )
    {
        futureTasks_.AddTask<StateType, Class>(action, member);
        return GetPtr();
    }

    template <typename Class, typename Arg1>
    typename State::Ptr AddAction(Class* action, StateType (Class::*member)(Arg1), Arg1 arg1)
    {
        futureTasks_.AddTask<StateType, Class, Arg1>(action, member, arg1);
        return GetPtr();
    }

    template <typename Class, typename Arg1, typename Arg2>
    typename State::Ptr AddAction(Class* action, StateType (Class::*member)(Arg1, Arg2), Arg1 arg1, Arg2 arg2)
    {
        futureTasks_.AddTask<StateType, Class, Arg1, Arg2>(action, member, arg1, arg2);
        return GetPtr();
    }

    typename State::Ptr SetNextState(StateType nextState)
    {
        nextState_ = nextState;
        return GetPtr();
    }

    StateType GetStateType() const
    {
        return stateType_;
    }

    StateType GetNextState() const
    {
        return nextState_;
    }

    bool IsDone() const
    {
        return futureTasks_.IsDone();
    }

    FutureTasks& Actions()
    {
        return futureTasks_;
    }

    const FutureTasks& Actions() const
    {
        return futureTasks_;
    }

private:
    /**
     * @brief futureTasks_ holds the action to execute, return-value is event
     */
    FutureTasks futureTasks_;

    /**
     * @brief stateType_
     */
    StateType   stateType_;

    /**
     * @brief nextState_
     */
    StateType   nextState_;
};

/**
 * @brief The StateMachineMethods class represents a state machine and also has builtin
 * a sequential execution of a finite state machine.
 *
 * TODO: Separate the execution from the representation to achieve a parallell execution of the finite state machine.
 *
 * TODO: typename LOCK = NullReadWriteLock - makes this synchronized or not
 */
template <typename StateType>
class StateMachineMethods : public Runnable
                          , public ENABLE_SHARED_FROM_THIS_T1(StateMachineMethods, StateType)
{
private:
    typedef std::map<StateType, typename State<StateType>::Ptr >     ListOfStates;
    typedef std::pair<StateType, typename State<StateType>::Ptr >    PairState;
    typedef BaseLib::Collection::ISet<StateType>                     SetStateTypes;

public:
    /**
     * @brief StateMachineMethods
     */
    StateMachineMethods()
        : isDone_(false)
    {}

    /**
     * @brief ~StateMachineMethods
     */
    virtual ~StateMachineMethods()
    {}

    CLASS_TRAITS(StateMachineMethods)

public:

    /**
     * @brief GetPtr
     * @return
     */
    typename StateMachineMethods::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    /**
     * @brief run
     */
    virtual void run()
    {
        // ------------------------------------
        // Pick first state
        // ------------------------------------

        currentState_ = getStartState();

        // -- debug --
        ASSERT(currentState_);
        // -- debug --

        if(!currentState_)
        {
            ICRITICAL() << "State machine is flawed! Cannot find start state!";
            return;
        }

        // ------------------------------------
        // while not final state reached
        //      Run current state
        //      return value == next state
        //      pick next state
        // ------------------------------------

        while(isDone() == false)
        {
            currentState_->run();

            currentState_ = getState(currentState_->GetNextState());

            // -- debug --
            ASSERT(currentState_);
            // -- debug --

            if(!currentState_)
            {
                ICRITICAL() << "State machine is flawed! Cannot find next state!";
                break;
            }
            else if(isFinalState(currentState_->GetNextState()))
            {
                IDEBUG() << "Final state reached!";
                break;
            }
        }
    }

    /**
     * @brief CreateState
     * @param type
     * @return
     */
    typename State<StateType>::Ptr CreateStartState(StateType type)
    {
        typename State<StateType>::Ptr state = getOrCreateState(type);
        startState_ = type;

        return state->GetPtr();
    }

    /**
     * @brief CreateState
     * @param type
     * @return
     */
    typename State<StateType>::Ptr CreateFinalState(StateType type)
    {
        typename State<StateType>::Ptr state = getOrCreateState(type);
        this->finalStates_.insert(type);

        return state->GetPtr();
    }

    /**
     * @brief CreateState
     * @param type
     * @return
     */
    typename State<StateType>::Ptr GetOrCreateState(StateType type)
    {
        return getOrCreateState(type);
    }

    /**
     * @brief SetStartState
     * @param type
     */
    void SetStartState(StateType type)
    {
        startState_ = type;
    }

    /**
     * @brief AddFinalState
     * @param type
     * @return
     */
    typename StateMachineMethods::SetStateTypes& AddFinalState(StateType type)
    {
        finalStates_.insert(type);
        return finalStates_;
    }

    /**
     * @brief IsDone
     * @return
     */
    bool IsDone() const
    {
        return isDone();
    }

    /**
     * @brief SetDone
     * @param done
     */
    void SetDone(bool done)
    {
        this->setDone(done);
    }

    /**
     * @brief CurrentStateType
     * @return
     */
    StateType CurrentStateType() const
    {
        if(currentState_ == NULL)
        {
            currentState_ = getStartState();

            // -- debug --
            ASSERT(currentState_);
            // -- debug --
        }

        return currentState_->GetStateType();
    }

protected:

    /**
     * @brief executeNextTask
     */
    bool executeTask(typename State<StateType>::Ptr state)
    {
        // -- debug --
        ASSERT(state);
        if(!state) return false;
        // -- debug --

        state->run();

        return true;
    }

    /**
     * @brief getState
     * @param stateType
     * @return
     */
    typename State<StateType>::Ptr getState(StateType stateType) const
    {
        if(states_.find(stateType) == states_.end())
        {
            IWARNING() << "Cannot find state type " << stateType;
            return typename State<StateType>::Ptr();
        }

        return states_.at(stateType);
    }

    /**
     * @brief getStartState
     * @return
     */
    typename State<StateType>::Ptr getStartState() const
    {
        return getState(getStartStateType());
    }

    /**
     * @brief isFinalState
     * @param stateType
     * @return
     */
    bool isFinalState(StateType stateType) const
    {
        if(this->finalStates_.contains(stateType))
            return true;

        return false;
    }

    /**
     * @brief getOrCreateState
     * @param type
     * @return
     */
    typename State<StateType>::Ptr getOrCreateState(StateType type)
    {
        typename State<StateType>::Ptr state;

        if(states_.find(type) != states_.end())
        {
            state = states_.at(type);
        }
        else
        {
            state = typename State<StateType>::Ptr(new State<StateType>(type));

            typename std::pair<typename ListOfStates::iterator, bool> inserted = states_.insert(PairState(type, state));

            ASSERT(inserted.second);
        }

        ASSERT(state);

        return state->GetPtr();
    }

    //Transition CreateTransition(State::Ptr a, State::Ptr b)
    //{
    //}

    /**
     * @brief getStartState
     * @return
     */
    StateType getStartStateType() const
    {
        return startState_;
    }

    /**
     * @brief isDone
     * @return
     */
    bool isDone() const
    {
        return isDone_;
    }

    /**
     * @brief setDone
     * @param finished
     */
    void setDone(bool done)
    {
        isDone_ = done;
    }

protected:
    /**
     * @brief currentState_
     */
    typename State<StateType>::Ptr currentState_;


private:
    /**
     * @brief states_
     */
    ListOfStates states_;

    /**
     * @brief startState_
     */
    StateType startState_;

    /**
     * @brief finalStates_
     */
    SetStateTypes finalStates_;

    /**
     * @brief isDone_
     */
    bool isDone_;
};

}}
