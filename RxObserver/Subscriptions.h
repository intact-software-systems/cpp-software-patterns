#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Subscription.h"
#include"RxObserver/ObserverBase.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

class DLL_STATE SubscriptionsBase
        : public Templates::UnsubscribeMethod<bool>
        , public Templates::IsSubscribedMethod
        , public Templates::BlockMethod
        , public Templates::UnblockMethod
        , public Templates::IsBlockedMethod
        , public Templates::ComparableImmutableType<Templates::AnyKey>
        , public ENABLE_SHARED_FROM_THIS(SubscriptionsBase)
{
public:
    virtual ~SubscriptionsBase() {}

    CLASS_TRAITS(SubscriptionsBase)

    SubscriptionsBase::Ptr GetPtr()
    {
        return shared_from_this();
    }

    virtual ObserverEvents History() const = 0;

    virtual void Add(Subscription::Ptr sub) = 0;
    virtual bool Remove(Subscription::Ptr sub) = 0;

    virtual void Clear() = 0;

    virtual size_t Length() const = 0;
    virtual bool IsEmpty() const = 0;

    virtual bool AddFilter(FilterCriterion::Ptr filter) = 0;
};

// --------------------------------------------------
// TODO: Support list of (id, signaller) in every Subscriptions
// TODO: Support pull last n events
// TODO: Mutex class to protect subscriptions_
// --------------------------------------------------

class DLL_STATE Subscriptions
        : public BaseLib::Concurrent::SubscriptionsBase
        , public BaseLib::Templates::LockableType<Subscriptions>
{
public:
    Subscriptions() {}
    virtual ~Subscriptions() {}

    CLASS_TRAITS(Subscriptions)

    // -----------------------------------------
    // Template functions
    // -----------------------------------------

    template <typename DelegateType>
    bool UnsubcribeType(DelegateType *delegate)
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            bool isUnsubscribed = sub->UnsubscribeType<DelegateType>(delegate);
            if(isUnsubscribed) return true;
        }

        return false;
    }

    template <typename DelegateType, typename FunctionPointer>
    bool UnsubscribeType(DelegateType *delegate, FunctionPointer func)
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            bool isUnsubscribed = sub->UnsubscribeType<DelegateType, FunctionPointer>(delegate, func);
            if(isUnsubscribed) return true;
        }

        return false;
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            bool isAdded = sub->AddFilter(filter);
            if(isAdded) return true;
        }
        return false;
    }

    template <typename EventObserver>
    void Connect(EventObserver *obs)
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            std::shared_ptr<ObserverAction> observer = sub->Observer();
            if(observer != nullptr)
            {
                observer->Connect(obs);
            }
            else
            {
                IWARNING() << "Observer is null!";
            }
        }
    }

    template <typename EventObserver>
    void Disconnect(EventObserver *obs)
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            std::shared_ptr<ObserverAction> observer = sub->Observer();
            if(observer != nullptr)
            {
                observer->Disconnect(obs);
            }
            else
            {
                IWARNING() << "Observer is null!";
            }
        }
    }

    // -----------------------------------------
    // Subscriptions interface
    // -----------------------------------------

    virtual void Add(Subscription::Ptr sub)
    {
        Locker locker(this);
        subscriptions_.push_back(sub);
    }

    virtual bool Remove(Subscription::Ptr sub)
    {
        Locker locker(this);
        size_t sz = subscriptions_.size();

        subscriptions_.remove(sub);

        return subscriptions_.size() == sz;
    }

    virtual void Clear()
    {
        Locker locker(this);
        subscriptions_.clear();
    }

    virtual size_t Length() const
    {
        Locker locker(this);
        return subscriptions_.size();
    }

    virtual bool IsEmpty() const
    {
        Locker locker(this);
        return subscriptions_.empty();
    }

    // -----------------------------------------
    // Interface Subscription
    // -----------------------------------------

    virtual bool Unsubscribe()
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
            sub->Unsubscribe();

        return !subscriptions_.empty();
    }

    virtual bool Block()
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
            sub->Block();

        return !subscriptions_.empty();
    }

    virtual bool Unblock()
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
            sub->Unblock();

        return !subscriptions_.empty();
    }

    virtual bool IsBlocked() const
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            if(!sub->IsBlocked())
                return false;
        }

        return !subscriptions_.empty();
    }

    virtual bool IsSubscribed() const
    {
        Locker locker(this);
        for(Subscription::Ptr sub : subscriptions_)
        {
            if(!sub->IsSubscribed())
                return false;
        }

        return !subscriptions_.empty();
    }

    virtual ObserverEvents History() const
    {
        Locker locker(this);
        ObserverEvents allEvents;

        for(Subscription::Ptr sub : subscriptions_)
        {
            ObserverEvents events = sub->History();
            allEvents.merge(events);
        }

        return allEvents;
    }

    template <typename T>
    std::list<T> History() const
    {
        Locker locker(this);
        std::list<T> history;
        for(ObserverEvent::Ptr event : this->History())
        {
            T eventData = std::dynamic_pointer_cast<T>(event->Data());
            if(eventData == nullptr) continue;

            history.push_back(eventData);
        }

        return history;
    }

public:
    static Subscriptions::Ptr Empty()
    {
        return std::make_shared<Subscriptions>();
    }

private:
    Collection::IList<Subscription::Ptr> subscriptions_;
};

}}
