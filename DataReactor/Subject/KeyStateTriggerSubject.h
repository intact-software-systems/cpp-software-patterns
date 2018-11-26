#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Observer/KeyStateTriggerObserver.h"

namespace Reactor
{

/**
 * @brief The KeyStateTriggerSubject class
 */
template <typename K, typename StateType>
class KeyStateTriggerSubject
        : public KeyStateTriggerEvents<K, StateType>
        , public Observable< KeyStateTriggerObserver<K, StateType> >
{
public:
    KeyStateTriggerSubject()
        : signalTriggerNext_(new BaseLib::Concurrent::Signaller2<void, K, StateType>())
        , signalError_(new BaseLib::Concurrent::Signaller2<void, K, StateType>())
        , signalCompleted_(new BaseLib::Concurrent::Signaller2<void, K, StateType>())
    {}
    virtual ~KeyStateTriggerSubject()
    {}

    CLASS_TRAITS(KeyStateTriggerSubject)

    virtual SlotHolder::Ptr Connect(KeyStateTriggerObserver<K, StateType> *observer)
    {
        typename SlotHolder::Ptr slot = signalError_->template Connect<KeyStateTriggerObserver<K, StateType> >(observer, &KeyStateTriggerObserver<K, StateType>::OnError);
        observer->AddSlot(observer, slot);

        slot = signalCompleted_->template Connect<KeyStateTriggerObserver<K, StateType> >(observer, &KeyStateTriggerObserver<K, StateType>::OnComplete);
        observer->AddSlot(observer, slot);

        slot = signalTriggerNext_->template Connect<KeyStateTriggerObserver<K, StateType> >(observer, &KeyStateTriggerObserver<K, StateType>::OnTriggerNext);
        observer->AddSlot(observer, slot);

        return slot;
    }

    virtual bool Disconnect(KeyStateTriggerObserver<K, StateType> *observer)
    {
        signalError_->template Disconnect<KeyStateTriggerObserver<K, StateType> >(observer, &KeyStateTriggerObserver<K, StateType>::OnError);
        signalCompleted_->template Disconnect<KeyStateTriggerObserver<K, StateType> >(observer, &KeyStateTriggerObserver<K, StateType>::OnComplete);
        return signalTriggerNext_->template Disconnect<KeyStateTriggerObserver<K, StateType> >(observer, &KeyStateTriggerObserver<K, StateType>::OnTriggerNext);
    }

    virtual void DisconnectAll()
    {
        signalError_->DisconnectAll();
        signalCompleted_->DisconnectAll();
        signalTriggerNext_->DisconnectAll();
    }

    /**
     * @brief OnTriggerNext is called by RxHandler on completion of its task. It notifies the observer
     * about the state that executed and the result through trigger-method.
     *
     * @param state
     * @param result
     */
    virtual void OnTriggerNext(K key, StateType state)
    {
        signalTriggerNext_->Signal(key, state);
    }

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(K key, StateType state)
    {
        signalError_->Signal(key, state);
    }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnComplete(K key, StateType state)
    {
        signalCompleted_->Signal(key, state);
    }

private:
    typename BaseLib::Concurrent::Signaller2<void, K, StateType>::Ptr signalTriggerNext_;
    typename BaseLib::Concurrent::Signaller2<void, K, StateType>::Ptr signalError_;
    typename BaseLib::Concurrent::Signaller2<void, K, StateType>::Ptr signalCompleted_;
};

}
