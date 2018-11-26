#pragma once

#include"RxFSM/IncludeExtLibs.h"
#include"RxFSM/Export.h"

namespace Reactor
{

typedef BaseLib::Templates::NameDescription StateDescription;
typedef BaseLib::Templates::NameDescription StateMachineDescription;

// ----------------------------------------------------
// StateStatus
// ----------------------------------------------------

enum class DLL_STATE StateStatusKind
{
    INACTIVE,
    ENTERING,
    ENTERED,
    INPUT,
    INPUTTED,
    TRANSITION,
    EXITED,
};

typedef Status::EventStatusTracker<StateStatusKind, int> StateStatus;

static std::ostream& operator<<(std::ostream& ostr, const StateStatusKind& kind)
{
    switch(kind)
    {
#define CASE(t) case t: ostr << #t; break;
        CASE(StateStatusKind::INACTIVE)
        CASE(StateStatusKind::ENTERING)
        CASE(StateStatusKind::ENTERED)
        CASE(StateStatusKind::INPUT)
        CASE(StateStatusKind::INPUTTED)
        CASE(StateStatusKind::TRANSITION)
        CASE(StateStatusKind::EXITED)
#undef CASE
        default:
            std::cerr << "Missing operator<< case for StateStatusKind" << std::endl;
            break;
    }
    return ostr;
}

static std::ostream& operator<<(std::ostream& ostr, const StateStatus& status)
{
    ostr << "["
    << status.CurrentState()
    << ", "
    << status.PreviousState()
    << "]";

    return ostr;
}

// ----------------------------------------------------
// StateMachineStatus
// ----------------------------------------------------

enum class DLL_STATE StateMachineStatusKind
{
    WAIT,
    VIRTUAL_INPUT,
    INPUT_ACTION_CONDITION,
    EXECUTE_INPUT_ACTION,
    TRANSITION_CONDITION,
    EXECUTE_EXIT_ACTION,
    CHANGE_STATE,
    EXECUTE_ENTRY_ACTION
};

typedef Status::EventStatusTracker<StateMachineStatusKind, int> StateMachineStatus;


static std::ostream& operator<<(std::ostream& ostr, const StateMachineStatusKind& kind)
{
    switch(kind)
    {
#define CASE(t) case t: ostr << #t; break;
        CASE(StateMachineStatusKind::WAIT)
        CASE(StateMachineStatusKind::VIRTUAL_INPUT)
        CASE(StateMachineStatusKind::INPUT_ACTION_CONDITION)
        CASE(StateMachineStatusKind::EXECUTE_INPUT_ACTION)
        CASE(StateMachineStatusKind::TRANSITION_CONDITION)
        CASE(StateMachineStatusKind::EXECUTE_EXIT_ACTION)
        CASE(StateMachineStatusKind::CHANGE_STATE)
        CASE(StateMachineStatusKind::EXECUTE_ENTRY_ACTION)
#undef CASE
        default:
            std::cerr << "Missing operator<< case for StateStatusKind" << std::endl;
            break;
    }
    return ostr;
}

static std::ostream& operator<<(std::ostream& ostr, const StateMachineStatus& status)
{
    ostr << "["
    << status.CurrentState()
    << ", "
    << status.PreviousState()
    << "]";

    return ostr;
}

}
