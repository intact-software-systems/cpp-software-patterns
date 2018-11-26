#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/ObserverBase.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// EventResultBase
// ----------------------------------------------------

class DLL_STATE EventResultBase
{
public:
    virtual bool WaitFor(int64 msecs = LONG_MAX) const = 0;

    virtual bool IsAccepted() const = 0;
    virtual bool IsReceived() const = 0;

    virtual const Event& GetEvent() const = 0;
};

class Observers;
FORWARD_CLASS_TRAITS(Observers)

// ----------------------------------------------------
// EventResult
// ----------------------------------------------------

/**
 * Wait for event delivery.
 *
 * EventResult enables waiting for an event delivery. ObserverGroup is used to verify
 * when event is received by all observers and set original event status.
 */
class DLL_STATE EventResult
        : public EventResultBase
        , public BaseLib::Observer::ReactiveObserver<std::shared_ptr<ObserverAction>, BaseLib::Exception>
        , public Templates::Function
{
public:
    EventResult(Event::Ptr event, ObserversPtr observerGroup);
    virtual ~EventResult();

    CLASS_TRAITS(EventResult)

    // -----------------------------------
    // Interface ReactiveObserver
    // -----------------------------------

    virtual void OnNext(std::shared_ptr<ObserverAction> received);
    virtual void OnComplete();
    virtual void OnError(BaseLib::Exception exception);

    // -----------------------------------
    // Interface EventResultBase
    // -----------------------------------

    virtual bool WaitFor(int64 msecs = LONG_MAX) const;

    virtual bool IsAccepted() const;
    virtual bool IsReceived() const;

    virtual const Event& GetEvent() const;

    void AddResult(Templates::Any value);

private:
    Event::Ptr   event_;
    ObserversPtr observerGroup_;
};

typedef std::list<BaseLib::Concurrent::EventResult::Ptr> EventResults;

}}
