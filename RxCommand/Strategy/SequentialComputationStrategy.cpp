#include"RxCommand/Strategy/SequentialComputationStrategy.h"
#include"RxCommand/Strategy/ComputationAlgorithms.h"

namespace Reactor
{

IList<CommandBase::Ptr> SequentialComputationStrategy::Perform(int , const CommandsGroup &commands)
{
    return ComputationAlgorithms::findReadyCommands(commands);
}

}
