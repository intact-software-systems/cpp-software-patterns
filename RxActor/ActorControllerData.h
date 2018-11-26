#pragma once

#include "RxActor/CommonDefines.h"
#include "Supervisor.h"

namespace RxActor {

template <typename T>
struct ActorControllerData
{
    ActorControllerData(ActorGraph* graph, Supervisor supervisor)
        : actorGraph(graph)
        , supervisor(supervisor)
    { }

    ActorGraph* actorGraph;

    Supervisor supervisor;
};

}
