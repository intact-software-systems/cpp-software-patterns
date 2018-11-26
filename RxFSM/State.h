#pragma once

#include"RxFSM/CommonDefines.h"
#include"RxFSM/StateCommand.h"
#include"RxFSM/Export.h"

namespace Reactor
{

// ----------------------------------------------------
// NextState
// ----------------------------------------------------

//class StateMachinePolicy;
//template <typename StateType>
//class NextState : public Templates::NextStateMethod<StateType, StateMachineStatus, StateMachinePolicy>
//{ };

// TODO: Should a command know if it is initial, final? Store this in transitions table instead?
//enum class DLL_STATE StateKind
//{
//    INITIAL,
//    REGULAR,
//    FINAL
//};

// ----------------------------------------------------
// ActionType
// ----------------------------------------------------
//
//enum class DLL_STATE ActionType
//{
//    ENTRY,
//    EXIT,
//    INPUT,
//    TRANSITION
//};

// ----------------------------------------------------
// StatePolicy
// ----------------------------------------------------

class DLL_STATE StatePolicy
{
public:
    StatePolicy(Policy::Timeout timeout = Policy::Timeout::FromMinutes(5))
        : stateTimeout_(timeout)
        , attempt_(Policy::Attempt::Default())
        , interval_(Policy::Interval::FromMinutes(2))
        , retryInterval_(Policy::Interval::FromMinutes(2))
        , deadline_(Policy::Deadline::FromSeconds(60))
    { }

    virtual ~StatePolicy()
    { }

    const Policy::Timeout& Timeout() const
    {
        return stateTimeout_;
    }

    const Policy::Attempt& attempt() const
    {
        return attempt_;
    }

    const Policy::Interval& interval() const
    {
        return interval_;
    }

    const Policy::Interval& retryInterval() const
    {
        return retryInterval_;
    }

    const Policy::Deadline& deadline() const
    {
        return deadline_;
    }

private:
    // How long can we be in state?
    const Policy::Timeout  stateTimeout_;
    const Policy::Attempt  attempt_;
    const Policy::Interval interval_;
    const Policy::Interval retryInterval_;
    const Policy::Deadline deadline_;
};

// ----------------------------------------------------
// StateData: Should I use DataAccessController?
// ----------------------------------------------------

template <typename StateType, typename Return, typename Input>
class StateData
{
public:
    StateData(
        StateType vertex,
        StateDescription description,
        CommandControllerPolicy policy,
        RxThreadPool::Ptr threadPool
    )
        : vertex_(vertex)
        , description_(description)
        , entry_(new StateCommandController<Return>(policy, threadPool))
        , exit_(new StateCommandController<Return>(policy, threadPool))
        , input_(new StateCommandController<Input>(policy, threadPool))
        , transition_(new StateCommandController<Return>(policy, threadPool))
    { }

    virtual ~StateData()
    { }

    typename StateCommandController<Return>::Ptr EntryActions() const
    {
        return entry_;
    }

    typename StateCommandController<Return>::Ptr ExitActions() const
    {
        return exit_;
    }

    typename StateCommandController<Input>::Ptr InputActions() const
    {
        return input_;
    }

    typename StateCommandController<Return>::Ptr TransitionActions() const
    {
        return transition_;
    }

    const StateType& Vertex() const
    {
        return vertex_;
    }

    const StateDescription& Description() const
    {
        return description_;
    }

private:
    StateType        vertex_;
    StateDescription description_;

    typename StateCommandController<Return>::Ptr entry_;
    typename StateCommandController<Return>::Ptr exit_;
    typename StateCommandController<Input>::Ptr  input_;
    typename StateCommandController<Return>::Ptr transition_;
};

// ----------------------------------------------------
// State
// ----------------------------------------------------

template <typename StateType, typename Return, typename Input>
class State
    : protected Templates::ContextObjectShared<StatePolicy, StateData<StateType, Return, Input>, StateStatus>
{
public:
    State(StateType vertex, StateDescription description, CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
        : Templates::ContextObjectShared<StatePolicy, StateData<StateType, Return, Input>, StateStatus>
        (
            new StatePolicy(),
            new StateData<StateType, Return, Input>(vertex, description, policy, threadPool),
            new StateStatus(Duration::FromMinutes(1))
        )
    { }

    virtual ~State()
    { }

    typename StateCommandController<Return>::Ptr EntryActions() const
    {
        return this->data()->EntryActions();
    }

    typename StateCommandController<Return>::Ptr ExitActions() const
    {
        return this->data()->ExitActions();
    }

    typename StateCommandController<Input>::Ptr InputActions() const
    {
        return this->data()->InputActions();
    }

    typename StateCommandController<Return>::Ptr TransitionActions() const
    {
        return this->data()->TransitionActions();
    }

    const StateType& Vertex() const
    {
        return this->data()->Vertex();
    }

    const StateDescription& Description() const
    {
        return this->data()->Description();
    }

    const StateStatus& Status() const
    {
        return this->status().operator*();
    }

    StateStatus& Status()
    {
        return this->status().operator*();
    }

    // ----------------------------------------------------
    // Operators
    // ----------------------------------------------------

    bool operator==(const State<StateType, Return, Input>& that)
    {
        return this->data()->Vertex() == that.data()->Vertex();
    }

    bool operator!=(const State<StateType, Return, Input>& that)
    {
        return this->data()->Vertex() != that.data()->Vertex();
    }
};

}
