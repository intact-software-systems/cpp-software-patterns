#include"RxCommand/CommandFactory.h"

namespace Reactor
{

ReactorConfig::CommandControllerAccess CommandFactoryState::executors() const {
    return ReactorConfig::GetCommandControllerAccess();
}

CommandFactory::CommandFactory()
{ }

CommandFactory::~CommandFactory()
{ }

}
