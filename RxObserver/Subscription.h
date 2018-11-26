#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/SubscriptionBase.h"
#include"RxObserver/ObserverBase.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// --------------------------------------------------
// TODO: Support list of (id, signaller) in every Subscription
// TODO: Support pull last n events
// TODO: Support pull "current submitted event"
// --------------------------------------------------
class DLL_STATE Subscription
        : public BaseLib::Concurrent::SubscriptionBase
{
public:
    Subscription(std::shared_ptr<ObserverAction> observer)
        : observer_(observer)
        , eventIdentification_(observer->EventId())
    {}
    virtual ~Subscription()
    {
        // NB! Since this is a ptr, only use Disconnect from outside the class!
        // Otherwise a deadlock occurs when this is removed from SignalSlot
        // TODO: let signaller implement a lazy disconnect: "Disconnect as soon as you can", to avoid any simultaneous deletes?
    }

    CLASS_TRAITS(Subscription)

    // -----------------------------------------
    // Template functions
    // -----------------------------------------

    template <typename Delegate>
    bool UnsubscribeType(Delegate *observer)
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        DelegateIdentification delegateKey(observer);

        if(observerPtr->Delegate() == delegateKey)
        {
            return observerPtr->Unsubscribe();
        }

        return false;
    }

    template <typename Delegate, typename FunctionPointer>
    bool UnsubscribeType(Delegate *observer, FunctionPointer func)
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        if(observerPtr->EventId() == EventIdentification(observer, func))
        {
            return observerPtr->Unsubscribe();
        }

        return false;
    }

    virtual bool AddFilter(FilterCriterion::Ptr filter)
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        return observerPtr->AddFilter(filter);
    }

    // -----------------------------------------
    // Interface Subscription
    // -----------------------------------------

    virtual bool Unsubscribe()
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        return observerPtr->Unsubscribe();
    }

    virtual bool Block()
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        return observerPtr->Block();
    }

    virtual bool Unblock()
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        return observerPtr->Unblock();
    }

    virtual bool IsBlocked() const
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        return observerPtr->IsBlocked();
    }

    virtual bool IsSubscribed() const
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return false;

        return observerPtr->IsSubscribed();
    }

    virtual ObserverEvents History() const
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return ObserverEvents();

        return observerPtr->History();
    }

    virtual ObserverPolicy Policy() const
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return ObserverPolicy::Default();

        return observerPtr->Policy();
    }

    EventIdentification EventId() const
    {
        return eventIdentification_;
    }

//    virtual const Status::ObserverStatus &StatusConst() const
//    {
//        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
//        if(!observerPtr) return Status::ObserverStatus();

//        return observerPtr->StatusConst();
//    }

    template <typename T>
    std::list<T> History()
    {
        std::shared_ptr<ObserverAction> observerPtr = observer_.lock();
        if(!observerPtr) return std::list<T>();

        std::list<T> history;
        for(ObserverEvent::Ptr event : observerPtr->History())
        {
            T eventData = std::dynamic_pointer_cast<T>(event->Data());
            if(eventData == nullptr) continue;

            history.push_back(eventData);
        }

        return history;
    }

    std::shared_ptr<ObserverAction> Observer() const
    {
        return observer_.lock();
    }

private:
    std::weak_ptr<ObserverAction> observer_;
    EventIdentification eventIdentification_;
};

}}

