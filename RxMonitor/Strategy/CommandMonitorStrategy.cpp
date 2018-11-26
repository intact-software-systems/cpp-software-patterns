#include"RxMonitor/Strategy/CommandMonitorStrategy.h"

namespace Reactor { namespace Strategy
{
int MonitorAllCommandControllers::Perform()
{
    return monitorControllers();
}

int MonitorAllCommandControllers::monitorControllers()
{
    IINFO() << "monitoring controllers";

    for (std::pair<CommandController::Ptr, CommandController::Ptr> entry : ReactorConfig::GetCommandControllerAccess().getObjects())
    {
        CommandController::Ptr commandController = entry.first;

        if(commandController->IsDone())
        {
            ReactorConfig::GetCommandControllerAccess().Dispose(entry.first);
            //CommandFactory.getOrCreateCommandControllerCache().setExpired(objectRoot.getKey());
            //updatedControllers.put(commandController, commandController);
        }
        else if(commandController->IsExecuting())
        {
            //log.info("Command controller: " + commandController);

            // -------------------------------------
            // check commands for policy violations
            // If policy violated then execute configurable response:
            // - logging of violation
            // - TODO: generate alarm events
            // -------------------------------------
            // TODO: Temporarily comment out due to lack of control of printouts
            for (CommandBase::Ptr command : commandController->GetCommands().GetCommands())
            {
                if(command->IsPolicyViolated())
                {
                    IWARNING() << "Command policy violated " << command->GetName() << " attached to controller"; // + commandController;
                }
            }
        }
    }

    if(ReactorConfig::GetCommandControllerAccess().getObjects().size() > 1000)
    {
        IINFO() << "rx CommandControllers are filling up " << ReactorConfig::GetCommandControllerAccess().getObjects().size();
    }

    return 0;
}
}}
