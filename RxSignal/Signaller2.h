#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Signaller.h"

namespace BaseLib { namespace Concurrent
{

/* ---------------------------------------------------
Event-handler for return type and one parameter

- Observer calls Attach<Observer>(&obs, &Observer::onUpdate);
- Subject calls Notify(Arg1).
- Instead of duplicating code use C++11 variadic templates.
- Monitoring this object by using factory functions.
- Configurability. Use QoS to configure object, but what to configure?
    - Policy on priority of signalled observers
- Fault-tolerance, the function pointers are unsafe, ust std::function

TODO: Instead of using TwoArgBase use TwoArgFunctionPointerInvoker or FutureTask
---------------------------------------------------*/

template <typename Return, typename Arg1, typename Arg2>
class Signaller2
        : public Signaller
        , public ENABLE_SHARED_FROM_THIS_T3(Signaller2, Return, Arg1, Arg2)
{
protected:
    typedef typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr CallbackPtr;
    typedef typename Collection::IMap<InstanceHandle, CallbackPtr>     SlotMap;

public:
    Signaller2()
    { }

    virtual ~Signaller2()
    {
        DisconnectAll();
    }

    CLASS_TRAITS(Signaller2)

    typename Signaller2::Ptr GetPtr()
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
    typename SlotHolder::Ptr Connect(Class* observer, Return (Class::*member)(Arg1, Arg2))
    {
        return Signaller::connect<Return, Signaller2::Ptr, SlotMap, Class, Arg1, Arg2>(GetPtr(), slotMap_, observer, member);
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
        return Signaller::disconnect<Return, SlotMap, Class, Arg1, Arg2>(slotMap_, observer);
    }

    template <typename Class>
    bool Disconnect(Class *observer, Return (Class::*member)(Arg1, Arg2))
    {
        return Signaller::disconnect<Return, SlotMap, Class, Arg1, Arg2>(slotMap_, observer, member);
    }

    void Signal(Arg1 arg1, Arg2 arg2)
    {
        Signaller::signal<Return, SlotMap, Arg1, Arg2>(slotMap_, arg1, arg2);
    }

    template <typename Class>
    void Signal(Arg1 arg1, Arg2 arg2, Return (Class::*member)(Arg1, Arg2))
    {
        Signaller::signal<Return, SlotMap, Class, Arg1, Arg2>(slotMap_, arg1, arg2, member);
    }

    void AsynchSignal(Arg1 arg1, Arg2 arg2)
    {
        Signaller::asynchSignal<Return, Signaller2::Ptr, Arg1, Arg2>(GetPtr(), arg1, arg2);
    }

    template <typename Class>
    void AsynchSignal(Arg1 arg1, Arg2 arg2, Return (Class::*member)(Arg1, Arg2))
    {
        Signaller::asynchSignal<Return, Signaller2::Ptr, Class, Arg1, Arg2>(GetPtr(), arg1, arg2, member);
    }

private:
    SlotMap 	slotMap_;
};

}}
