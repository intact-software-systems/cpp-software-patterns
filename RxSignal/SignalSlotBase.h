#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Export.h"

namespace BaseLib { namespace Concurrent
{
/** ---------------------------------------------------
Event-handler for return type and one parameter

- Observer calls Attach<Observer>(&obs, &Observer::onUpdate);
- Subject calls Notify(Arg1).

TODO: So this works for any number of arguments, just duplicate code and add more typenames Arg2, Arg3, etc,

    - Can I make it in such a way that I don't have to duplicate so much code?
        For example, typename Arg1 = void, typename Arg2 = void, etc         if(Arg1 == void) then ....
        NO, void default does not work because this is not allowed in C++ void f(void, int, void, double)

TODO:
    - To make implementation safe I need:
        - Automatically call Detach when an Observer goes out-of-scope (is deleted)
    - I am currently not monitoring this object
    - Make signal implementations use DataReactor commands internally. Doing this will enable using
        all command and commandcontroller policies. In addition, it makes it possible to implement
        specialized signallers with replay functionality, etc.
---------------------------------------------------*/

// --------------------------------------------------
// SlotHolder is held by the observer, and is generated when observer connects to a subject.
// SlotHolder is a representation of a "Subscription", where the subscriber is the observer and publisher is the subject.
// Subscription policy - SlotPolicy
// --------------------------------------------------
class DLL_STATE SlotHolder : public ENABLE_SHARED_FROM_THIS(SlotHolder)
{
public:
    virtual ~SlotHolder() {}

    CLASS_TRAITS(SlotHolder)

    SlotHolder::Ptr GetPtr()
    {
        return shared_from_this();
    }

public:
    virtual InstanceHandle Handle() const = 0;

    virtual bool Disconnect() = 0;
    // virtual bool Unsubscribe() = 0;

    virtual void Block() = 0;
    virtual void Unblock() = 0;

    virtual bool IsBlocked() const = 0;
    virtual bool IsAttached() const = 0;
    // virtual bool IsSubscribed() const = 0;
};

// --------------------------------------------------
// Slots or "Subscriptions"
// --------------------------------------------------
class DLL_STATE Slots: public ENABLE_SHARED_FROM_THIS(Slots)
{
public:
    virtual ~Slots() {}

    CLASS_TRAITS(Slots)

    Slots::Ptr GetPtr()
    {
        return shared_from_this();
    }

    virtual bool Disconnect() = 0;
    // virtual bool Unsubscribe() = 0;

    virtual void Block() = 0;
    virtual void Unblock() = 0;

    virtual bool IsBlocked() const = 0;
    virtual bool IsAttached() const = 0;
    // virtual bool IsSubscribed() const = 0;
};

// --------------------------------------------------
// SlotLocker lock(slot_); similar to MutexLocker lock(&mutex);
// --------------------------------------------------
template <typename T>
class SlotTypeLocker
{
public:
    inline explicit SlotTypeLocker(T slot)
        : slot_(slot)
    {
        if(!slot_)
            throw std::runtime_error("SlotTypeLocker::SlotTypeLocker(T): Argument error! T == nullptr");

        slot_->Block();
    }

    inline ~SlotTypeLocker() { slot_->Unblock(); }

private:
    mutable T slot_;

private:
    SlotTypeLocker(const SlotTypeLocker &) {}
    SlotTypeLocker& operator=(const SlotTypeLocker&) { return *this; }
};

typedef SlotTypeLocker<SlotHolder::Ptr>  SlotLocker;

// -----------------------------------------------------
// Callback in signallers
// -----------------------------------------------------

template <typename FUNC>
class Callback : public ENABLE_SHARED_FROM_THIS_T1(Callback, FUNC)
{
public:
    Callback(FUNC *argBase, SlotHolder::Ptr holder)
        : argBase_(argBase)
        , holder_(holder->GetPtr())
    {}

    virtual ~Callback()
    {
        delete argBase_;
        argBase_ = NULL;
    }

    CLASS_TRAITS(Callback)

    typename Callback::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    SlotHolder::Ptr GetSlotHolder() { return holder_->GetPtr(); }
    FUNC*           GetArgBase()    { return argBase_; }

private:
    FUNC*           argBase_;
    SlotHolder::Ptr holder_;
};

// --------------------------------------------------
// Is implemented by all specialized signallers/subjects
// --------------------------------------------------
class DLL_STATE SignallerBase
{
public:
    virtual ~SignallerBase() {}

    CLASS_TRAITS(SignallerBase)

    virtual void Block() = 0;
    virtual void Unblock() = 0;

    virtual bool Disconnect(InstanceHandle id) = 0;
    virtual void DisconnectAll() = 0;

    virtual size_t Size() const = 0;

    virtual bool IsConnected(InstanceHandle id) const = 0;
    virtual bool IsEmpty() const = 0;
    virtual bool IsBlocked() const = 0;
};

// --------------------------------------------------
// Observable is to be implemented by subject.
// TODO: Consider renaming?
// --------------------------------------------------
template <typename T>
class Observable
{
public:
    virtual typename SlotHolder::Ptr Connect(T *observer) = 0;

    virtual bool Disconnect(T *observer) = 0;

    virtual void DisconnectAll() = 0;
};

// --------------------------------------------------
// This was an idea to "Execute around" connect and disconnect of observer to a subject, might not be necessary anymore
// --------------------------------------------------
template <typename OBSERVER, typename SUBJECT = Observable<OBSERVER> >
class ObserverConnector : public Observable<OBSERVER>
{
private:
    typedef std::list<OBSERVER*> ListObservers;

public:
    inline explicit ObserverConnector(OBSERVER *observer, SUBJECT *subject)
        : observers_()
        , subject_(subject)
    {
        observers_.push_back(observer);
        subject_->Connect(observer);
    }

    inline explicit ObserverConnector(SUBJECT *subject)
        : observers_()
        , subject_(subject)
    { }

    inline ~ObserverConnector()
    {
        for(typename ListObservers::iterator it = observers_.begin(), it_end = observers_.end(); it != it_end; ++it)
            subject_->Disconnect(*it);
    }

    virtual typename SlotHolder::Ptr Connect(OBSERVER *observer)
    {
        observers_.push_back(observer);
        return subject_->Connect(observer);
    }

    virtual bool Disconnect(OBSERVER *observer)
    {
        observers_.remove(observer);
        return subject_->Disconnect(observer);
    }

    virtual void DisconnectAll()
    {
        observers_.clear();
        subject_->DisconnectAll();
    }

private:
    ListObservers   observers_;
    SUBJECT         *subject_;

private:
    ObserverConnector(const ObserverConnector &) {}
    ObserverConnector& operator=(const ObserverConnector &) { return *this; }
};

// --------------------------------------------------
// Observer slots (subscriptions), "decorates" the observer to control its subscriptions.
// --------------------------------------------------
template <typename T>
class ObserverSlot
{
private:
    typedef Collection::IMultiMap<T*, typename SlotHolder::Ptr> MapSlotHolders;

public:
    virtual ~ObserverSlot()
    {
        DisconnectAllSlots();
    }

    /**
     * Accessed by subject when connecting (subscribing)
     */
    bool AddSlot(T *observer, typename SlotHolder::Ptr slot)
    {
        return slotHolders_.put(observer, slot) != slotHolders_.end();
    }

protected:

    bool DisconnectSlot(T *observer)
    {
        bool isDisconnected = true;

        typename MapSlotHolders::iterator it = slotHolders_.find(observer);
        for( ; it != slotHolders_.end(); ++it)
        {
            typename SlotHolder::Ptr slot = it->second;

            ASSERT(slot);

            if(it->first == observer)
            {
                isDisconnected = slot->Disconnect();
            }
            else
            {
                IDEBUG() << "No more slots connected";
                break;
            }
        }

        slotHolders_.erase(observer);

        return isDisconnected;
    }

    void DisconnectAllSlots()
    {
        for(typename MapSlotHolders::iterator it = slotHolders_.begin(), it_end = slotHolders_.end(); it != it_end; ++it)
        {
            typename SlotHolder::Ptr slot = it->second;

            ASSERT(slot);

            slot->Disconnect();
        }

        slotHolders_.clear();
    }

    InstanceHandleSet GetHandles(T *observer) const
    {
        InstanceHandleSet handles;

        typename MapSlotHolders::const_iterator it = slotHolders_.find(observer);
        for( ; it != slotHolders_.end(); ++it)
        {
            typename SlotHolder::Ptr slot = it->second;

            ASSERT(slot);

            if(it->first == observer)
            {
                handles.insert(slot->Handle());
            }
            else
            {
                IDEBUG() << "No more slots connected";
                break;
            }
        }

        return handles;
    }

    void Unblock(T *observer)
    {
        blockSignal(observer, false);
    }

    void Block(T *observer)
    {
        blockSignal(observer, true);
    }

public:
    bool IsAttached(T *observer) const
    {
        typename MapSlotHolders::const_iterator it = slotHolders_.find(observer);
        if(it != slotHolders_.end())
        {
            typename SlotHolder::Ptr slot = it->second;

            ASSERT(slot);

            return slot->IsAttached();
        }
        return false;
    }

    bool IsBlocked(T *observer) const
    {
        typename MapSlotHolders::const_iterator it = slotHolders_.find(observer);
        if(it != slotHolders_.end())
        {
            typename SlotHolder::Ptr slot = it->second;

            ASSERT(slot);

            return slot->IsBlocked();
        }
        return false;
    }

    size_t NumSlots() const
    {
        return slotHolders_.size();
    }

private:

    void blockSignal(T *observer, bool block)
    {
        typename MapSlotHolders::iterator it = slotHolders_.find(observer);
        for( ; it != slotHolders_.end(); ++it)
        {
            typename SlotHolder::Ptr slot = it->second;

            ASSERT(slot);

            if(it->first == observer)
            {
                if(block)
                {
                    slot->Block();
                }
                else
                {
                    slot->Unblock();
                }
            }
            else
            {
                break;
            }
        }
    }

private:
    MapSlotHolders slotHolders_;
};

namespace details
{

/**
 * TODO: Support list of (id, signaller) in every SlotHolder
 */
class DLL_STATE SlotHolderImpl : public BaseLib::Concurrent::SlotHolder
{
public:
    SlotHolderImpl(InstanceHandle id, SignallerBase::Ptr signaller)
        : slotId_(id)
        , blocked_(false)
        , signaller_(signaller)
    {}

    virtual ~SlotHolderImpl()
    {
        // NB! Since this is a ptr, only use Disconnect from outside the class!
        // Otherwise a deadlock occurs when this is removed from SignalSlot
        // TODO: let signaller implement a lazy disconnect: "Disconnect as soon as you can", to avoid any simultaneous deletes?
    }

    InstanceHandle Handle() const
    {
        return slotId_;
    }

    virtual bool Disconnect()
    {
        SignallerBase::Ptr sigPtr = signaller_.lock();
        if(!sigPtr) return false;

        return sigPtr->Disconnect(slotId_);
    }

    virtual void Block()
    {
        blocked_ = true;
    }

    virtual void Unblock()
    {
        blocked_ = false;
    }

    virtual bool IsBlocked() const
    {
        return blocked_;
    }

    virtual bool IsAttached() const
    {
        SignallerBase::Ptr sigPtr = signaller_.lock();
        if(!sigPtr) return false;

        return sigPtr->IsConnected(slotId_);
    }

private:
    InstanceHandle              slotId_;
    Templates::ProtectedBool    blocked_;
    SignallerBase::WeakPtr      signaller_;
};

}

}}
