#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Signaller.h"

namespace BaseLib { namespace Concurrent
{

/** ---------------------------------------------------
Event-handler for return type and one parameter

- Observer calls Attach<Observer>(&obs, &Observer::onUpdate);
- Subject calls Notify(Arg1).
- Instead of duplicating code use C++11 variadic templates.
- Monitoring this object by using factory functions.
- Configurability. Use QoS to configure object, but what to configure?
    - Policy on priority of signalled observers
    - Policy on async or sync
    - Policy on queue length
    - Policy on feedback upon failure to signal
- Fault-tolerance, the function pointers are unsafe, ust std::function
- The subject or reactor uses signallers

API:
- Block function - suspend signalling until Unblock
- Unblock function
- Complete/Disconnect function - No more values can be signalled. Is supported through Disconnect

Policy support in subject:
- Replay last n signals to connecting observer (History + Replay)
- Sequential or Parallel signalling to observers (Composition) - Use sequential scheduling thread pool, or control execution here.
- Async or Sync signalling to observers (Invocation)
- Store n last signal values and signal to observers upon completion
- Filter based on time (TimebasedFilter) -
- Signal speed (Interval)
- Filter based on content

Policy support in observer:
- Request last n signals to connecting observer
- Signal speed (Interval)
- Filter based on content
- Deadline - expecting value in configurable intervals. Use thread-pool's monitoring of policy violation and call-back upon interval violations.

Reactor:
- To support policy in both subject and observer I need a "mediator" which is the reactor.

Status and fault tolerance
- Strategy if failing to signal - fail safe, fail fast, continue, report

Impl:
- Observer and Observers
- Subscription and Subscriptions
- Subject and Subjects
- Observer ability to iterate through available values
- Reactor - manage observers, control signalling
- Use publisher subscriber vocabulary (Subscribe and Unsubscribe, etc)

Lib:
- Either in BaseLib or DataReactor
- Separate ObserverLib?
- Main difference from Command is that observer is notified whenever subject have new arguments
- What is the difference between observer and the thread pool implementation with scheduled futures?
  -
---------------------------------------------------*/
template <typename Return, typename Arg1>
class Signaller1
        : public Signaller
        , public ENABLE_SHARED_FROM_THIS_T2(Signaller1, Return, Arg1)
{
protected:
    typedef typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr CallbackPtr;
    typedef typename Collection::IMap<InstanceHandle, CallbackPtr>     SlotMap;

public:
    Signaller1()
    { }
    virtual ~Signaller1()
    {
        DisconnectAll();
    }

    CLASS_TRAITS(Signaller1)

    typename Signaller1::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual bool IsConnected(InstanceHandle id) const
    {
        MutexTypeLocker<SlotMap> lock(&slotMap_);
        return slotMap_.find(id) != slotMap_.end();
    }

    virtual bool IsEmpty() const
    {
        MutexTypeLocker<SlotMap> lock(&slotMap_);
        return slotMap_.empty();
    }

    virtual size_t Size() const
    {
        MutexTypeLocker<SlotMap> lock(&slotMap_);
        return slotMap_.size();
    }

    template <typename Class>
    typename SlotHolder::Ptr Connect(Class* observer, Return (Class::*member)(Arg1))
    {
        return Signaller::connect<Return, Signaller1::Ptr, SlotMap, Class, Arg1>(GetPtr(), slotMap_, observer, member);
    }

    virtual bool Disconnect(InstanceHandle id)
    {
        return Signaller::disconnectHandle<SlotMap>(slotMap_, id);
    }

    virtual void DisconnectAll()
    {
        Signaller::disconnectAll<SlotMap>(slotMap_);
    }

    template <typename Class>
    bool Disconnect(Class *observer)
    {
        return Signaller::disconnect<Return, SlotMap, Class, Arg1>(slotMap_, observer);
    }

    template <typename Class>
    bool Disconnect(Class *observer, Return (Class::*member)(Arg1))
    {
        return Signaller::disconnect<Return, SlotMap, Class, Arg1>(slotMap_, observer, member);
    }

    void Signal(Arg1 arg1)
    {
        Signaller::signal<Return, SlotMap, Arg1>(slotMap_, arg1);
    }

    template <typename Class>
    void Signal(Arg1 arg1, Return (Class::*member)(Arg1))
    {
        Signaller::signal<Return, SlotMap, Class, Arg1>(slotMap_, arg1, member);
    }

    void AsynchSignal(Arg1 arg1)
    {
        Signaller::asynchSignal<Signaller1::Ptr, Arg1>(GetPtr(), arg1);
    }

    template <typename Class>
    void AsynchSignal(Arg1 arg1, Return (Class::*member)(Arg1))
    {
        Signaller::asynchSignal<Return, Signaller1::Ptr, Class, Arg1>(GetPtr(), arg1, member);
    }

private:
    SlotMap slotMap_;
};

}}
