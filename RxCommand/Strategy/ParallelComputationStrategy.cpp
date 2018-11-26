#include"RxCommand/Strategy/ParallelComputationStrategy.h"
#include"RxCommand/Strategy/ComputationAlgorithms.h"

namespace Reactor
{

IList<CommandBase::Ptr> ParallelComputationStrategy::Perform(int nReadyCommands, const CommandsGroup &commands)
{
    return ComputationAlgorithms::findNReadyCommands(nReadyCommands, commands);
}

}
