#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Commands.h"
#include"RxCommand/Export.h"

namespace Reactor {

class DLL_STATE ComputationAlgorithms
{
public:
    static IList<CommandBase::Ptr> findNReadyCommands(int nReadyCommands, const CommandsGroup& commands);

    /**
     * Finds one ready command in FIFO order, but only if no other command is executing.
     */
    static IList<CommandBase::Ptr> findReadyCommands(const CommandsGroup& commands);
};

}
