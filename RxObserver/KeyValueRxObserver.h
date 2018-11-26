#pragma once

#include "RxObserver/IncludeExtLibs.h"
#include "RxObserver/EventSource.h"
#include "RxObserver/EventBase.h"
#include "RxObserver/SubjectDescription.h"

namespace BaseLib {

template <typename K, typename V>
class KeyValueRxEvents
{
public:
    virtual ~KeyValueRxEvents()
    { }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual bool OnComplete(K key) = 0;

    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    virtual bool OnNext(K key, V value) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual bool OnError(K key, GeneralException) = 0;
};


template <typename K, typename V>
class KeyValueRxEventsSource
    : public Concurrent::EventSource<KeyValueRxEvents<K, V>>
      , public Templates::FactoryFunction0<KeyValueRxEventsSource<K, V>>
{
private:
    typedef KeyValueRxEventsSource<K, V>                      Source;
    typedef KeyValueRxEvents<K, V>                            EventType;
    typedef std::shared_ptr<KeyValueRxEvents<K, V>>           EventTypePtr;
    typedef typename Concurrent::SubjectGroup<EventType>::Ptr SubjectGroupPtr;

public:
    virtual ~KeyValueRxEventsSource()
    { }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    Concurrent::Event::Ptr Complete(K key)
    {
        return subject_->template NextEvent<Source, bool, K>(key, &EventType::OnComplete, this);
    }

    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    Concurrent::Event::Ptr Next(K key, V value)
    {
        return subject_->template NextEvent<Source, bool, K, V>(key, value, &EventType::OnNext, this);
    }

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    Concurrent::Event::Ptr Error(K key, GeneralException exception)
    {
        return subject_->template NextEvent<Source, bool, K, GeneralException>(
            key,
            exception,
            &EventType::OnError,
            this
        );
    }

    /**
     * Subscribe called by observer
     */
    virtual Concurrent::Subscriptions::Ptr Subscribe(
        EventTypePtr observer,
        Concurrent::ObserverPolicy policy = Concurrent::ObserverPolicy::Default()) const
    {
        Concurrent::Subscriptions::Ptr subscriptions(new Concurrent::Subscriptions());
        Concurrent::Subscription::Ptr  subscription;

        ASSERT(IsInitialized());

        // ---------------------------------------
        // Subscribe to EventType
        // ---------------------------------------
        subscription = subject_->template Subscribe<bool, K>(observer, &EventType::OnComplete, policy);
        subscriptions->Add(subscription);

        subscription = subject_->template Subscribe<bool, K, V>(observer, &EventType::OnNext, policy);
        subscriptions->Add(subscription);

        subscription = subject_->template Subscribe<bool, K, GeneralException>(observer, &EventType::OnError, policy);
        subscriptions->Add(subscription);

        return subscriptions;
    }

    virtual bool Initialize(SubjectGroupPtr subject)
    {
        subject_ = subject;

        subject_->template Create<bool, K>(&EventType::OnComplete);
        subject_->template Create<bool, K, V>(&EventType::OnNext);
        subject_->template Create<bool, K, GeneralException>(&EventType::OnError);

        return subject_ != nullptr;
    }

    virtual bool IsInitialized() const
    {
        return subject_ != nullptr;
    }

    static Concurrent::SubjectDescription DefaultDescription()
    {
        return Concurrent::SubjectDescription("KeyValueRxEvents");
    }

private:
    SubjectGroupPtr subject_;
};


template <typename K, typename V>
class KeyValueRxObserver
    : public KeyValueRxEvents<K, V>
      , public Concurrent::ObserverEventType<KeyValueRxEvents<K, V>, KeyValueRxEventsSource<K, V>>
{
public:
    KeyValueRxObserver(Concurrent::SubjectDescription description = KeyValueRxEventsSource<K, V>::DefaultDescription())
        : Concurrent::ObserverEventType<KeyValueRxEvents<K, V>, KeyValueRxEventsSource<K, V>>
        (
            KeyValueRxEventsSource<K, V>::CreatePtr(),
            description
        )
    { }

    virtual ~KeyValueRxObserver()
    { }
};

}
