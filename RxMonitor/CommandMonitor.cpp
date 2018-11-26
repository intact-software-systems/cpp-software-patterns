#include"RxMonitor/CommandMonitor.h"
#include"RxMonitor/Strategy/CommandMonitorStrategy.h"

namespace Reactor
{

// ---------------------------------------------------------------
// CommandMonitorState
// ---------------------------------------------------------------

CommandMonitorState::CommandMonitorState()
{ }

CommandMonitorState::~CommandMonitorState()
{ }

bool CommandMonitorState::IsInitialized() const
{
    return controller_ != nullptr;
}

CommandControllerAction<int>::Ptr CommandMonitorState::GetOrCreateController(CommandMonitorPtr parent)
{
    if(controller_ == nullptr)
    {
        controller_ = CommandFactory::Instance().CreateCommandController<int>(CommandControllerPolicy::Default(), RxThreadPoolFactory::MonitorPool());
        controller_->RxSubject().state().Subscribe(parent, ObserverPolicy::Default());
    }

    return controller_;
}

// ---------------------------------------------------------------
// CommandMonitor
// ---------------------------------------------------------------

CommandMonitor::CommandMonitor()
    : Templates::ContextObject<CommandMonitorPolicy, CommandMonitorState>
      (
          CommandMonitorPolicy::Default(),
          CommandMonitorState()
      )
{ }

CommandMonitor::~CommandMonitor()
{ }

CommandMonitor::Ptr CommandMonitor::GetPtr()
{
    return shared_from_this();
}

bool CommandMonitor::Initialize()
{
    CommandActions<int>::Ptr command(new CommandActions<int>(
                                         CommandPolicy::ForeverIndividual(
                                             Policy::Interval::RunEveryMs(1000),
                                             Policy::Interval::RunEveryMs(1000),
                                             Policy::Timeout::FromMilliseconds(200))));

    Templates::StrategyAction0Invoker<int>::Ptr invoker(new Templates::StrategyAction0Invoker<int>(Reactor::Strategy::MonitorAllCommandControllers::InstancePtr()));

    command->Add(invoker);

    ReactorConfig::GetCommandControllerHome()->Connect(this);

    this->state().GetOrCreateController(this->GetPtr())->Add(command);
    this->state().GetOrCreateController(this->GetPtr())->Execute();

    return this->state().IsInitialized();
}

bool CommandMonitor::IsInitialized() const
{
    return this->state().IsInitialized();
}

// ------------------------------------------
// Interface ObjectObserver<Reactor::CommandController::Ptr>
// ------------------------------------------

bool CommandMonitor::OnObjectCreated(CommandController::Ptr )
{
    //data->Connect(this->state().ThreadPool().get());
    return true;
}

bool CommandMonitor::OnObjectDeleted(CommandController::Ptr )
{
    //data->Disconnect(this->state().ThreadPool().get());
    return true;
}

bool CommandMonitor::OnObjectModified(CommandController::Ptr )
{
    return true;
}

// ------------------------------------------------
// RxEventsReturn interface
// ------------------------------------------------

bool CommandMonitor::OnComplete()
{
    return true;
}

bool CommandMonitor::OnNext(int )
{
    return true;
}

bool CommandMonitor::OnError(GeneralException)
{
    return true;
}

}

