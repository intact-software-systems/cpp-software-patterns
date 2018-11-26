#pragma once

#include"RxMonitor/IncludeExtLibs.h"
#include"RxMonitor/Export.h"

namespace Reactor
{

// ---------------------------------------------------------------
// CommandMonitorPolicy
// ---------------------------------------------------------------

class DLL_STATE CommandMonitorPolicy
{
public:
    CommandMonitorPolicy() {}
    virtual ~CommandMonitorPolicy() {}

    static CommandMonitorPolicy Default()
    {
        return CommandMonitorPolicy();
    }
};

class CommandMonitor;
FORWARD_CLASS_TRAITS(CommandMonitor)

// ---------------------------------------------------------------
// CommandMonitorState
// ---------------------------------------------------------------

class DLL_STATE CommandMonitorState
{
public:
    CommandMonitorState();
    virtual ~CommandMonitorState();

    bool IsInitialized() const;

    CommandControllerAction<int>::Ptr GetOrCreateController(CommandMonitorPtr parent);

private:
    CommandControllerAction<int>::Ptr controller_;
};

// ---------------------------------------------------------------
// CommandMonitor
// ---------------------------------------------------------------

class DLL_STATE CommandMonitor
        : public Templates::SupervisorSingleton<CommandMonitor>
        , public RxData::ObjectObserver<Reactor::CommandController::Ptr>
        , public RxObserverNew<int>
        , public Templates::ContextObject<CommandMonitorPolicy, CommandMonitorState>
        , public ENABLE_SHARED_FROM_THIS(CommandMonitor)
{
public:
    CommandMonitor();
    virtual ~CommandMonitor();

    CLASS_TRAITS(CommandMonitor)

    CommandMonitor::Ptr GetPtr();

    virtual bool Initialize();
    virtual bool IsInitialized() const;

    // ------------------------------------------------
    // ObjectObserver interface
    // ------------------------------------------------

    virtual bool OnObjectCreated(Reactor::CommandController::Ptr data);
    virtual bool OnObjectDeleted(Reactor::CommandController::Ptr data);
    virtual bool OnObjectModified(Reactor::CommandController::Ptr);

    // ------------------------------------------------
    // RxEventsReturn interface
    // ------------------------------------------------

    virtual bool OnComplete();
    virtual bool OnNext(int);
    virtual bool OnError(GeneralException);
};

}
