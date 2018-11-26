#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Signaller.h"

namespace BaseLib { namespace Concurrent {

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class Signaller3
    : public Signaller
      , public std::enable_shared_from_this<Signaller3<Return, Arg1, Arg2, Arg3>>
{
    typedef typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr CallbackPtr;
    typedef typename Collection::IMap<InstanceHandle, CallbackPtr>                      SlotMap;
    typedef Signaller3<Return, Arg1, Arg2, Arg3>                                        TheSignaller;

public:
    Signaller3()
    { }

    virtual ~Signaller3()
    {
        DisconnectAll();
    }

    CLASS_TRAITS(Signaller3)

    typename Signaller3::Ptr GetPtr()
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
    typename SlotHolder::Ptr Connect(Class* observer, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        return Signaller::connect<Return, std::shared_ptr<TheSignaller>, SlotMap, Class, Arg1, Arg2, Arg3>(GetPtr(), slotMap_, observer, member);
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
    bool Disconnect(Class* observer)
    {
        return Signaller::disconnect<Return, SlotMap, Class, Arg1, Arg2, Arg3>(slotMap_, observer);
    }

    template <typename Class>
    bool Disconnect(Class* observer, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        return Signaller::disconnect<Return, SlotMap, Class, Arg1, Arg2, Arg3>(slotMap_, observer, member);
    }

    void Signal(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        Signaller::signal<Return, SlotMap, Arg1, Arg2, Arg3>(slotMap_, arg1, arg2, arg3);
    }

    template <typename Class>
    void Signal(Arg1 arg1, Arg2 arg2, Arg3 arg3, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        Signaller::signal<Return, SlotMap, Class, Arg1, Arg2, Arg3>(slotMap_, arg1, arg2, arg3, member);
    }

    void AsynchSignal(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        Signaller::asynchSignal<Return, Signaller3::Ptr, Arg1, Arg2, Arg3>(GetPtr(), arg1, arg2, arg3);
    }

    template <typename Class>
    void AsynchSignal(Arg1 arg1, Arg2 arg2, Arg3 arg3, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        Signaller::asynchSignal<Return, Signaller3::Ptr, Class, Arg1, Arg2, Arg3>(GetPtr(), arg1, arg2, arg3, member);
    }

private:
    SlotMap slotMap_;
};

}}
