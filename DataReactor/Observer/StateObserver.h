#pragma once

#include"DataReactor/IncludeExtLibs.h"

namespace Reactor {

/**
 * @brief The StateEvents class
 */
class StateEvents
{
public:
    virtual void OnEntry()      = 0;
    virtual void OnExit()       = 0;
    virtual void OnTransition() = 0;
};

/**
 * @brief The StateObserver class
 */
class StateObserver
    : public StateEvents
      , public ObserverSlot<StateEvents>
{
};

}
