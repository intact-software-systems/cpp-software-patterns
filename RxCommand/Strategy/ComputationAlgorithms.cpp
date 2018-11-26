#include "RxCommand/Strategy/ComputationAlgorithms.h"

namespace Reactor
{

// TODO: Starvation!!!
IList<CommandBase::Ptr> ComputationAlgorithms::findNReadyCommands(int nReadyCommands, const CommandsGroup &commands)
{
    IList<CommandBase::Ptr> readyCommands;

    if(nReadyCommands <= 0)
        return readyCommands;

    for (CommandBase::Ptr command : commands.GetCommands())
    {
        if(!command->IsExecuting() && command->IsReady())
        {
            readyCommands.push_back(command);

            nReadyCommands--;

            if(nReadyCommands <= 0) break;
        }
    }

    return readyCommands;
}

/**
 * Finds one ready command in FIFO order, but only if no other command is executing.
 */
IList<CommandBase::Ptr> ComputationAlgorithms::findReadyCommands(const CommandsGroup &commands)
{
    IList<CommandBase::Ptr> readyCommands;

    if(commands.AreCommandsExecuting())
        return readyCommands;

    for(CommandBase::Ptr command : commands.GetCommands())
    {
        if(!command->IsExecuting() && command->IsReady())
        {
            readyCommands.push_back(command);
            return readyCommands;
        }
    }

    return readyCommands;
}

}
