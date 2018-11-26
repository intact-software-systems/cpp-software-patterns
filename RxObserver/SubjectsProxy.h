#pragma once

#include"RxObserver/Subjects.h"
#include"RxObserver/SubjectFactory.h"

namespace BaseLib { namespace Concurrent
{

/**
 * TODO: When a subject is activated or deactivated, notify the observers.
 * TODO: Store function references. Use std::function.
 *  - std::list<EventType*> events_;
 *
 * TODO: When a subject is done, call Complete. How to detect done when multiple subjects?
 *      - I need to keep track of active subjects.
 *      - SubjectManager/Monitor/Supervisor that observers activate/deactivate and removes when done.
 *
 * TODO: When a subject has an error, call Error. Monitor this also.
 */
template <typename EventType, typename EventSourceType>
class SubjectsProxy
        : public Templates::LockableType<SubjectsProxy<EventType, EventSourceType>, Mutex>
{
protected:
    typedef SubjectsProxy<EventType, EventSourceType>   Proxy;
    typedef MutexTypeLocker<Proxy>                      Locker;
    typedef std::shared_ptr<EventType>                  EventTypePtr;
    typedef typename SubjectGroup<EventType>::Ptr       SubjectGroupPtr;

public:
    SubjectsProxy(EventSourceType *eventSource, SubjectDescription description, SubjectPolicy policy = SubjectPolicy::Default())
        : eventSource_(eventSource)
        , description_(description)
        , policy_(policy)
    {
        Initialize();
    }
    virtual ~SubjectsProxy()
    { }

    CLASS_TRAITS(SubjectsProxy)

    friend class Templates::DoubleCheckedLocking;

public:

    EventSourceType& On()
    {
        Templates::DoubleCheckedLocking::Initialize<Proxy>(this);

        return *eventSource_;
    }

    const EventSourceType& On() const
    {
        Templates::DoubleCheckedLocking::Initialize<Proxy>(this);

        return *eventSource_;
    }

    Subscriptions::Ptr Subscribe(EventTypePtr observer, ObserverPolicy policy = ObserverPolicy::Default())
    {
        Templates::DoubleCheckedLocking::Initialize<Proxy>(this);

        Subscriptions::Ptr subscriptions = eventSource_->Subscribe(observer, policy);

        {
            Locker lock(this);
            subscriptionsList_.push_back(subscriptions);
        }

        return subscriptions;
    }

    bool Unsubscribe(EventTypePtr observer)
    {
        return subject()->Unsubscribe(observer);
    }

//    Subscription::Ptr Subscribe(std::shared_ptr<ObserverAction> observer)
//    {
//        return subject()->Subscribe(observer);
//    }

    bool Block()
    {
        return subject()->Block();
    }

    bool Unblock()
    {
        return subject()->Unblock();
    }

    bool IsBlocked() const
    {
        return subject()->IsBlocked();
    }

    bool Unsubscribe()
    {
        Locker lock(this);
        for(Subscriptions::Ptr subscriptions : subscriptionsList_)
        {
            subscriptions->Unsubscribe();
        }
        return true;
    }

    bool IsEmpty() const
    {
        return subject()->IsEmpty();
    }

    int Size() const
    {
        return subject()->Size();
    }

    SubjectPolicy Policy() const
    {
        return subject()->Policy();
    }

    Events History() const
    {
        return subject()->History();
    }

    SubjectDescription Id() const
    {
        return subject()->Description();
    }

    bool AddFilter(FilterCriterion::Ptr filter)
    {
        for(auto subject : subject()->GetSubjects())
        {
            subject->AddFilter(filter);
        }
        return true;
    }

    template <typename FunctionPointer>
    typename Subject::Ptr GetSubject(FunctionPointer *event) const
    {
        return subject()->Find(SubjectId(event));
    }

    // -----------------------------------------
    // Admin functions
    // -----------------------------------------

//    typename SubjectGroup<EventType>::Ptr subject() const
//    {
//        Templates::DoubleCheckedLocking::Initialize<Proxy>(this);

//        return subject_;
//    }

    SubjectGroupPtr subject() const
    {
        //Templates::DoubleCheckedLocking::Initialize<Proxy>(this);

        return subject_;
    }


    bool Initialize()
    {
        subject_ = SubjectFactory::Instance().Create<EventType>(description_, policy_);

        eventSource_->Initialize(subject_);

        return subject_ != nullptr;
    }

    bool IsInitialized() const
    {
        return subject_ != nullptr;
    }

    template <typename EventObserver>
    void Connect(EventObserver *obs)
    {
        subject()->template ConnectObserver<EventObserver>(obs);
    }

    template <typename EventObserver>
    void Disconnect(EventObserver *obs)
    {
        subject()->template DisconnectObserver<EventObserver>(obs);
    }

private:
    std::shared_ptr<EventSourceType> eventSource_;

    SubjectDescription  description_;
    SubjectPolicy       policy_;

    SubjectGroupPtr subject_;

    std::list<Subscriptions::Ptr> subscriptionsList_;
};

}}
