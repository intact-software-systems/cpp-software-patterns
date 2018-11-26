#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Signaller.h"

namespace BaseLib { namespace Concurrent
{

// Replace function pointers with std::function

template <typename Return>
class Signaller0
        : public Signaller
        , public std::enable_shared_from_this<Signaller0<Return>>
{
protected:
    typedef typename Callback<Templates::InvokeMethod0<Return> >::Ptr CallbackPtr;
    typedef typename Collection::IMap<InstanceHandle, CallbackPtr>     SlotMap;

public:
    Signaller0()
    { }
    virtual ~Signaller0()
    {
        DisconnectAll();
    }

    CLASS_TRAITS(Signaller0)

    typename Signaller0::Ptr GetPtr()
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
    typename SlotHolder::Ptr Connect(Class* observer, Return (Class::*member)())
    {
        return Signaller::connect<Return, Signaller0::Ptr, SlotMap, Class>(GetPtr(), slotMap_, observer, member);
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
        return Signaller::disconnect<Return, SlotMap, Class>(slotMap_, observer);
    }

    template <typename Class>
    bool Disconnect(Class *observer, Return (Class::*member)())
    {
        return Signaller::disconnect<Return, SlotMap, Class>(slotMap_, observer, member);
    }

    void Signal()
    {
        Signaller::signal<Return, SlotMap>(slotMap_);
    }

    template <typename Class>
    void Signal(Return (Class::*member)())
    {
        Signaller::signal<Return, SlotMap, Class>(slotMap_, member);
    }

    void AsynchSignal()
    {
        Signaller::asynchSignal<Return, Signaller0::Ptr>(GetPtr());
    }

    template <typename Class>
    void AsynchSignal(Return (Class::*member)())
    {
        Signaller::asynchSignal<Return, Signaller0::Ptr, Class>(GetPtr(), member);
    }

private:
    SlotMap 	slotMap_;
};

}}

