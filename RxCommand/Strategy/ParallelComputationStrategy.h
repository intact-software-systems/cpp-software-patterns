#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Commands.h"
#include"RxCommand/Strategy/ComputationStrategy.h"
#include"RxCommand/Export.h"

namespace Reactor {

class DLL_STATE ParallelComputationStrategy
    : public ComputationStrategy
      , public StaticSingleton<ParallelComputationStrategy>
{
public:
    virtual ~ParallelComputationStrategy()
    { }

    virtual IList<CommandBase::Ptr> Perform(int nReadyCommands, const CommandsGroup& commands);
};

}