#pragma once

#include <RxObserver/EventSource.h>
#include <RxObserver/ObserverPolicy.h>
#include <RxObserver/EventBase.h>

namespace BaseLib
{

template <typename T>
class RxEventsReturn
{
public:
    virtual ~RxEventsReturn() {}

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual bool OnComplete() = 0;

    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    virtual bool OnNext(T ) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual bool OnError(GeneralException ) = 0;
};

template <typename T>
class RxEventsSource
        : public Concurrent::EventSource<RxEventsReturn<T>>
        , public Templates::FactoryFunction0<RxEventsSource<T>>
{
private:
    typedef RxEventsSource<T>                       Source;
    typedef RxEventsReturn<T>                       EventType;
    typedef std::shared_ptr<RxEventsReturn<T>>      EventTypePtr;
    typedef typename Concurrent::SubjectGroup<EventType>::Ptr   SubjectGroupPtr;

public:
    virtual ~RxEventsSource()
    {
    }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    Concurrent::Event::Ptr Complete()
    {
        return subject_->template NextEvent<Source, bool>(&EventType::OnComplete, this);
    }

    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    Concurrent::Event::Ptr Next(T t)
    {
        return subject_->template NextEvent<Source, bool, T>(t, &EventType::OnNext, this);
    }

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    Concurrent::Event::Ptr Error(GeneralException exception)
    {
        return subject_->template NextEvent<Source, bool, GeneralException>(exception, &EventType::OnError, this);
    }

    /**
     * Subscribe called by observer
     */
    virtual Concurrent::Subscriptions::Ptr Subscribe(EventTypePtr observer, Concurrent::ObserverPolicy policy = Concurrent::ObserverPolicy::Default()) const
    {
        Concurrent::Subscriptions::Ptr subscriptions(new Concurrent::Subscriptions());
        Concurrent::Subscription::Ptr subscription;

        ASSERT(IsInitialized());
        ASSERT(observer);

        // ---------------------------------------
        // Subscribe to RxEvents
        // ---------------------------------------
        subscription = subject_-> template Subscribe<bool, T>(observer, &EventType::OnNext, policy);
        subscriptions->Add(subscription);

        subscription = subject_-> template Subscribe<bool, GeneralException>(observer, &EventType::OnError, policy);
        subscriptions->Add(subscription);

        subscription = subject_-> template Subscribe<bool>(observer, &EventType::OnComplete, policy);
        subscriptions->Add(subscription);

        return subscriptions;
    }

    virtual bool Initialize(SubjectGroupPtr subject)
    {
        ASSERT(subject);

        subject_ = subject;

        subject_->template Create<bool, T>(&EventType::OnNext);
        subject_->template Create<bool>(&EventType::OnComplete);
        subject_->template Create<bool, GeneralException>(&EventType::OnError);

        return subject_ != nullptr;
    }

    virtual bool IsInitialized() const
    {
        return subject_ != nullptr;
    }

    static Concurrent::SubjectDescription DefaultDescription()
    {
        return Concurrent::SubjectDescription("RxEvents");
    }

private:
    SubjectGroupPtr subject_;
};

template <typename T>
class RxObserverNew
        : public RxEventsReturn<T>
        , public Concurrent::ObserverEventType<RxEventsReturn<T>, RxEventsSource<T>>
{
public:
    RxObserverNew(Concurrent::SubjectDescription description = RxEventsSource<T>::DefaultDescription())
        : Concurrent::ObserverEventType<RxEventsReturn<T>, RxEventsSource<T>>(RxEventsSource<T>::CreatePtr(), description)
    { }
    virtual ~RxObserverNew()
    { }
};

}

