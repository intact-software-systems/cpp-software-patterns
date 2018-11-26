#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/ObserverEvent.h"
#include"RxObserver/ObserverPolicy.h"
#include"RxObserver/EventFilterCriterion.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// --------------------------------------------------
// Subscription is held by the observer, and is generated when observer connects to a subject.
// Subscription is a representation of a "subscription", where the subscriber is the observer and publisher is the subject.
// Subscription policy - SlotPolicy
// --------------------------------------------------
class DLL_STATE SubscriptionBase
        : public Templates::UnsubscribeMethod<bool>
        , public Templates::IsSubscribedMethod
        , public Templates::BlockMethod
        , public Templates::UnblockMethod
        , public Templates::IsBlockedMethod
        , public Templates::HistoryMethod<ObserverEvents>
        , public Templates::PolicyMethod<ObserverPolicy>
        , public ENABLE_SHARED_FROM_THIS(SubscriptionBase)
{
public:
    virtual ~SubscriptionBase()
    {}

    CLASS_TRAITS(SubscriptionBase)

    SubscriptionBase::Ptr GetPtr()
    {
        return shared_from_this();
    }

    virtual ObserverEvents History() const = 0;

    virtual bool AddFilter(FilterCriterion::Ptr filter) = 0;
};

// --------------------------------------------------
// SlotLocker lock(slot_); similar to MutexLocker lock(&mutex);
// --------------------------------------------------

template <typename T>
class SubscriptionTypeBlocker
{
public:
    inline explicit SubscriptionTypeBlocker(T slot)
        : slot_(slot)
    {
        if(!slot_)
            throw std::runtime_error("SubscriptionTypeBlocker::SubscriptionTypeBlocker(T): Argument error! T == nullptr");

        slot_->Block();
    }

    inline ~SubscriptionTypeBlocker() { slot_->Unblock(); }

private:
    mutable T slot_;

private:
    SubscriptionTypeBlocker(const SubscriptionTypeBlocker &) {}
    SubscriptionTypeBlocker& operator=(const SubscriptionTypeBlocker&) { return *this; }
};

typedef SubscriptionTypeBlocker<SubscriptionBase::Ptr>  SubscriptionLocker;

}}
