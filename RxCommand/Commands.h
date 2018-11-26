#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/CommandsHandle.h"
#include"RxCommand/Export.h"

namespace Reactor
{

// ----------------------------------------------------
// Base for ScheduledTasks
// ----------------------------------------------------

class DLL_STATE CommandsGroupBase
        : public Templates::Action
        , public Templates::TriggerMethod
        , public Templates::IsEmptyMethod
        , public Templates::IsSuspendedMethod
        , public Templates::ResumeMethod
        , public Templates::SuspendMethod
        , public Templates::ResetMethod<bool>
{
public:
    virtual ~CommandsGroupBase() {}

    CLASS_TRAITS(CommandsGroupBase)

    virtual IList<CommandBase::Ptr> GetCommands() const = 0;

    virtual void Add(CommandBase::Ptr command) = 0;
    virtual void Clear() = 0;

    virtual size_t Size() const = 0;

    virtual std::list<CommandBase::Ptr> FindReady() = 0;
    virtual std::list<CommandBase::Ptr> TriggerReady() = 0;

    virtual int NumTasksExecuting() const = 0;
    virtual int NumTasksPolicyViolated() const = 0;
    virtual int NumTasksNeverStarted() const = 0;

    virtual Duration EarliestReadyIn() const = 0;

    virtual bool AreCommandsExecuting() const = 0;

    virtual void StopTimeoutCommands() = 0;
    virtual void StopTimeoutCommands(const IList<CommandBase::Ptr> &commands) = 0;
    virtual void Timeout() = 0;
    virtual void Shutdown() = 0;
};

class DLL_STATE CommandsGroupState
{
public:
    //typedef std::map<InstanceHandle, CommandBase*> MapCommand;
    //typedef std::pair<InstanceHandle, CommandBase*> PairCommand;
    typedef Collection::IList<CommandBase::Ptr> CommandsList;

public:
    CommandsGroupState()
    {}
    virtual ~CommandsGroupState()
    {}

    const CommandsList& commands() const
    {
        return commands_;
    }

    CommandsList& commands()
    {
        return commands_;
    }

private:
    CommandsList commands_;
};

class DLL_STATE CommandsGroupPolicy
{
public:
    CommandsGroupPolicy()
    {}
    virtual ~CommandsGroupPolicy()
    {}
};

/**
 * TODO: Ability to make immutable?
 * TODO: Make round robin to support FIFO, LIFO, etc and also max concurrent commands.
 * TODO: Use Map<CommandHandle, CommandBase>
 *
 * TODO: Use StateMachineCollections?
 * - Execute n and move "READY" to "EXECUTE" then if failed "EXECUTE" to back-in-line for "READY" or "FAILED".
 * - Ex: Concurrent::StateMachineCollections<ExecutionState, CommandBase::Ptr> stateCollections_;
 *
 * TODO: Introduce ExecutionStatus and Policy for Commands as a group?
 *
 * Support multiple executions
 *  - Save status in-between new executions, can I use memento pattern in all status implementations?
 *  - Store status in a "collection for states" use the same in a cache
 *
 */
class DLL_STATE CommandsGroup
        : public CommandsGroupBase
        , public Templates::NotifyableShared<CommandsGroup>
        , public Templates::ContextObjectShared<CommandsGroupPolicy, CommandsGroupState, Status::ExecutionStatus>
{
private:
    typedef MutexTypeLocker<CommandsGroup> Locker;

public:
    CommandsGroup();
    virtual ~CommandsGroup();

    // ---------------------------------
    // Queue functions
    // TODO: Implement queue functions that returns
    // next ready Command and upon finish executing
    // puts at the back of the queue.
    // ---------------------------------
    //Command<K,V> getNextReady() {
    //return new CommandNoOp<K, V>();
    //}
    //CommandsHandle GetHandle()
    //{
    //  return CommandsHandle::Create(this->HashCode());
    //}

    // ----------------------------------------------------
    // Action interface
    // ----------------------------------------------------

    virtual bool IsDone() const;
    virtual bool IsSuspended() const;
    virtual bool IsSuccess() const;
    virtual bool Cancel();

    // ----------------------------------------------------
    // Public functions
    // ----------------------------------------------------

    virtual IList<CommandBase::Ptr> GetCommands() const;

    virtual void Add(CommandBase::Ptr command);
    virtual void Clear();
    virtual bool Reset();

    virtual bool IsEmpty() const;
    virtual size_t Size() const;

    virtual int      NumTasksExecuting() const;
    virtual int      NumTasksPolicyViolated() const;
    virtual int      NumTasksNeverStarted() const;
    virtual Duration EarliestReadyIn() const;

    virtual bool AreCommandsExecuting() const;

    //Status::ExecutionStatuses Snapshot() const;

    // ----------------------------------------------------
    // Trigger interfaces
    // ----------------------------------------------------

    virtual std::list<CommandBase::Ptr> FindReady();
    virtual std::list<CommandBase::Ptr> TriggerReady();

    virtual void Trigger();
    virtual void Resume();
    virtual void Suspend();

    // ------------------------------------------------------------
    // Action functions
    // ------------------------------------------------------------

    virtual void StopTimeoutCommands();
    virtual void StopTimeoutCommands(const IList<CommandBase::Ptr> &commands);
    virtual void Timeout();
    virtual void Shutdown();

private:

    void stopTimeoutCommands(const IList<CommandBase::Ptr> &commands);
};

}
