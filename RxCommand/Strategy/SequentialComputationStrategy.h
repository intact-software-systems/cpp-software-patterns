#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Commands.h"
#include"RxCommand/Strategy/ComputationStrategy.h"
#include"RxCommand/Export.h"

namespace Reactor {

class DLL_STATE SequentialComputationStrategy
    : public ComputationStrategy
      , public StaticSingleton<SequentialComputationStrategy>
{
public:
    virtual ~SequentialComputationStrategy()
    { }

    virtual IList<CommandBase::Ptr> Perform(int, const CommandsGroup& commands);
};

}
