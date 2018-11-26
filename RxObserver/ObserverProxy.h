#pragma once

#include"RxObserver/EventSource.h"

#include"RxObserver/Subject.h"
#include"RxObserver/SubjectFactory.h"
#include"RxObserver/ObserverNoOp.h"

namespace BaseLib { namespace Concurrent
{

template <typename EventType, typename EventSourceType>
class ObserverProxy
        : public Templates::LockableType<ObserverProxy<EventType, EventSourceType> >
{
protected:
    typedef ObserverProxy<EventType,EventSourceType>    Proxy;
    typedef MutexTypeLocker<Proxy>                      Locker;

    typedef std::shared_ptr<EventType>                  EventTypePtr;
    typedef std::weak_ptr<EventType>                    EventTypeWeakPtr;
    typedef std::shared_ptr<SubjectGroup<EventType>>    SubjectGroupPtr;
    typedef std::shared_ptr<EventSourceType>            EventSourcePtr;

public:
    ObserverProxy(EventSourceType *eventSource, SubjectDescription description)
        : observer_()
        , policy_(ObserverPolicy::Default())
        , eventSource_(eventSource)
        , subscription_(Subscriptions::Empty())
        , description_(description)
        , eventObserver_(nullptr)
    { }

    virtual ~ObserverProxy()
    {
        subscription_->Unsubscribe();
        Disconnect(eventObserver_);
    }

    CLASS_TRAITS(ObserverProxy)

    friend class Templates::DoubleCheckedLocking;

    Subscriptions::Ptr Subscribe(EventTypePtr observer, ObserverPolicy policy = ObserverPolicy::Default())
    {
        if(!subject() || observer == nullptr) return Subscriptions::Empty();

        Locker lock(this);

        observer_ = observer;
        policy_ = policy;

        return subscribe(observer, policy);
    }

    Subscriptions::Ptr Resubscribe()
    {
        EventTypePtr observer = observer_.lock();
        if(observer == nullptr) return Subscriptions::Empty();

        Locker lock(this);
        return subscribe(observer, policy_);
    }

    bool Unsubscribe()
    {
        Locker lock(this);

        return subscription_->Unsubscribe();
    }

    bool IsSubscribed() const
    {
        Locker lock(this);
        return subscription_->IsSubscribed();
    }

    bool Block()
    {
        Locker lock(this);
        return subscription_->Block();
    }

    bool Unblock()
    {
        Locker lock(this);
        return subscription_->Unblock();
    }

    bool IsBlocked() const
    {
        Locker lock(this);
        return subscription_->IsBlocked();
    }

    ObserverEvents History() const
    {
        Locker lock(this);
        return subscription_->History();
    }

    bool AddFilter(FilterCriterion::Ptr filter)
    {
        Locker lock(this);
        return subscription_->AddFilter(filter);
    }

    ObserverPolicy Policy() const
    {
        return policy_;
    }

//    const Status::ObserverStatus &StatusConst() const
//    {
//        return subscription_->StatusConst();
//    }

    // -----------------------------------------
    // Admin functions
    // -----------------------------------------

    SubjectGroupPtr subject()
    {
        bool isInitialized = Templates::DoubleCheckedLocking::Initialize<Proxy>(this);
        if(!isInitialized)
        {
            IFATAL() << "Failed to initialize subject group!";
            return SubjectGroupPtr();
        }

        ASSERT(subject_ != nullptr);

        return subject_;
    }

    bool Initialize()
    {
        subject_ = Concurrent::SubjectFactory::Instance().Find<EventType>(description_);

        if(subject_ != nullptr)
        {
            eventSource_->Initialize(subject_);
        }

        return subject_ != nullptr;
    }

    bool IsInitialized() const
    {
        return subject_ != nullptr;
    }

    // -----------------------------------------
    // Subject "subject" admin, i.e., filter, etc
    // -----------------------------------------

    bool Connect(BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr> *obs)
    {
        Locker lock(this);

        eventObserver_ = obs;

        if(eventObserver_ != nullptr)
        {
            subscription_->template Connect<BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr>>(eventObserver_);
        }

        return obs != nullptr;
    }

    bool Disconnect(BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr> *obs)
    {
        Locker lock(this);

        if(obs != nullptr)
        {
            subscription_->template Disconnect<BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr>>(obs);
        }

        return obs != nullptr;
    }

private:

    Subscriptions::Ptr subscribe(EventTypePtr observer, ObserverPolicy policy = ObserverPolicy::Default())
    {
        if(!subject() || observer == nullptr) return Subscriptions::Empty();

        if(!subscription_->IsSubscribed())
        {
            subscription_ = eventSource_->Subscribe(observer, policy);

            if(eventObserver_ != nullptr)
                subscription_->template Connect<BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr>>(eventObserver_);
        }
        else
        {
            IFATAL() << "Could not subscribe!";
        }

        return subscription_;
    }

private:
    EventTypeWeakPtr observer_;
    ObserverPolicy policy_;

    EventSourcePtr eventSource_;

    Subscriptions::Ptr      subscription_;
    SubjectDescription      description_;

    BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr> *eventObserver_;

    SubjectGroupPtr subject_;
};

}}
