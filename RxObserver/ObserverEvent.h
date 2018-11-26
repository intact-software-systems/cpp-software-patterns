#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

class ObserverAction;
FORWARD_CLASS_TRAITS(ObserverAction)

class EventResult;
FORWARD_CLASS_TRAITS(EventResult)

// ----------------------------------------------------
// ObserverEvent
// ----------------------------------------------------

/**
 * ObserverEvent is provides feedback to the Subject via EventResult upon Accept, Ignore, etc.
 */
class DLL_STATE ObserverEvent : public Concurrent::Event
{
public:
    ObserverEvent(EventResultPtr event, ObserverActionPtr observer);
    virtual ~ObserverEvent();

    CLASS_TRAITS(ObserverEvent)

    virtual void Accept();
    virtual void Ignore();
    virtual void Filter();
    virtual void Reject();

    void SetResult(Templates::Any val);

    template <typename Return>
    Return* Result() const
    {
        return Templates::AnyCast<Return>(&value_);
    }

private:
    EventResultPtr event_;
    ObserverActionPtr observer_;
    Templates::Any value_;
};

typedef std::list<BaseLib::Concurrent::ObserverEvent::Ptr> ObserverEvents;

}}
