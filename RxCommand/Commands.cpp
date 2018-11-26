#include"RxCommand/Commands.h"

namespace Reactor
{

CommandsGroup::CommandsGroup()
    : Templates::ContextObjectShared<CommandsGroupPolicy, CommandsGroupState, Status::ExecutionStatus>
      (
          new CommandsGroupPolicy(),
          new CommandsGroupState(),
          new Status::ExecutionStatus()
      )
{}

CommandsGroup::~CommandsGroup()
{}

// ----------------------------------------------------
// Action interface
// ----------------------------------------------------

bool CommandsGroup::IsDone() const
{
    Locker lock(this);

    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(!command->IsDone())
            return false;
    }
    return true;
}

bool CommandsGroup::IsSuspended() const
{
    Locker lock(this);

    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(!command->IsSuspended())
            return false;
    }
    return true;
}

bool CommandsGroup::IsSuccess() const
{
    Locker lock(this);

    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(!command->IsSuccess())
            return false;
    }
    return true;
}

bool CommandsGroup::Cancel()
{
    Locker lock(this);
    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Cancel();
    }
    return true;
}

// ------------------------------------------------------------
// Public functions
// ------------------------------------------------------------

IList<CommandBase::Ptr> CommandsGroup::GetCommands() const
{
    Locker lock(this);
    return this->data()->commands();
}

void CommandsGroup::Add(CommandBase::Ptr command)
{
    Locker lock(this);
    this->data()->commands().push_back(command);
}

void CommandsGroup::Clear()
{
    Locker lock(this);
    this->data()->commands().clear();
}

bool CommandsGroup::Reset()
{
    Locker lock(this);
    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Reset();
    }
    return true;
}

bool CommandsGroup::IsEmpty() const
{
    Locker lock(this);
    return this->data()->commands().empty();
}

size_t CommandsGroup::Size() const
{
    Locker lock(this);
    return this->data()->commands().size();
}

int CommandsGroup::NumTasksExecuting() const
{
    Locker lock(this);
    int numExecutingCommands = 0;
    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(command->IsExecuting())
            numExecutingCommands++;
    }
    return numExecutingCommands;
}

int CommandsGroup::NumTasksPolicyViolated() const
{
    Locker lock(this);
    int numTasksPolicyViolated = 0;
    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(command->IsPolicyViolated())
            ++numTasksPolicyViolated;
    }
    return numTasksPolicyViolated;
}

int CommandsGroup::NumTasksNeverStarted() const
{
    Locker lock(this);
    int numTasksNeverStarted = 0;
    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(command->Status().IsNeverExecuted())
            ++numTasksNeverStarted;
    }
    return numTasksNeverStarted;
}

Duration CommandsGroup::EarliestReadyIn() const
{
    Locker lock(this);

    Duration maxWaitTime = Duration::Infinite();

    for(CommandBase::Ptr command : this->data()->commands())
    {
        maxWaitTime = std::min<Duration>(maxWaitTime, command->ReadyIn());
    }

    return maxWaitTime;
}

bool CommandsGroup::AreCommandsExecuting() const
{
    Locker lock(this);
    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(command->IsExecuting())
            return true;
    }
    return false;
}

std::list<CommandBase::Ptr> CommandsGroup::FindReady()
{
    Locker lock(this);
    std::list<CommandBase::Ptr> readyTasks;

    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(command->IsReady())
            readyTasks.push_back(command);
    }

    return readyTasks;
}

std::list<CommandBase::Ptr> CommandsGroup::TriggerReady()
{
    Locker lock(this);
    std::list<CommandBase::Ptr> readyTasks;

    for(CommandBase::Ptr command : this->data()->commands())
    {
        if(command->IsReady())
        {
            readyTasks.push_back(command);
            command->Trigger();
        }
    }

    return readyTasks;
}

// ----------------------------------------------------
// Trigger interfaces
// ----------------------------------------------------

void CommandsGroup::Trigger()
{
    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Trigger();
    }
}

void CommandsGroup::Resume()
{
    Locker lock(this);

    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Resume();
    }
}

void CommandsGroup::Suspend()
{
    Locker lock(this);
    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Suspend();
    }
}

// ------------------------------------------------------------
// Action functions
// ------------------------------------------------------------

void CommandsGroup::StopTimeoutCommands()
{
    Locker lock(this);
    stopTimeoutCommands(this->data()->commands());
}

void CommandsGroup::StopTimeoutCommands(const IList<CommandBase::Ptr> &commands)
{
    Locker lock(this);
    stopTimeoutCommands(commands);
}

void CommandsGroup::Timeout()
{
    Locker lock(this);
    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Timeout();
    }
}

void CommandsGroup::Shutdown()
{
    Locker lock(this);
    for(CommandBase::Ptr command : this->data()->commands())
    {
        command->Shutdown();
    }
}

// ------------------------------------------------------------
// Private functions
// ------------------------------------------------------------

void CommandsGroup::stopTimeoutCommands(const IList<CommandBase::Ptr> &commands)
{
    for(CommandBase::Ptr command : commands)
    {
        if(command->IsTimeout())
        {
            // TODO: DoTimeout cancels one attempt, Cancel cancels all future attampts
            command->Timeout();
        }
    }
}

}
