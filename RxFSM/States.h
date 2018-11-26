#pragma once

#include"RxFSM/State.h"
#include"RxFSM/Export.h"

namespace Reactor
{

template <typename StateType, typename Return, typename Input>
class States : public Collection::IMap<StateType, Reactor::State<StateType, Return, Input>>
{
public:
    States() : Collection::IMap<StateType, Reactor::State<StateType, Return, Input>>()
    { }

    // What are typical query functions?

    int NumStatesActive() const
    {
        int counter = 0;
        for(std::pair<StateType, Reactor::State<StateType, Return, Input>> entry : *this)
        {
            if(entry.second.Status().CurrentState() != StateStatusKind::INACTIVE)
            {
                ++counter;
            }
        }
        return 0;
    }
};

}
