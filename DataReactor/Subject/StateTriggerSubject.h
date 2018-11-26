#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Observer/StateTriggerObserver.h"

namespace Reactor
{

template <typename T>
class StateTriggerSubject
        : public StateTriggerObserver<T>
        , public Observable< StateTriggerObserver<T> >
{
public:
    StateTriggerSubject()
        : signalTriggerNext_(new BaseLib::Concurrent::Signaller1<void, T>())
        , signalError_(new BaseLib::Concurrent::Signaller2<void, T, BaseLib::Exception>())
        , signalCompleted_(new BaseLib::Concurrent::Signaller1<void, T>())
    {}
    virtual ~StateTriggerSubject()
    {}

    CLASS_TRAITS(StateTriggerSubject)

    virtual typename SlotHolder::Ptr Connect(StateTriggerObserver<T> *observer)
    {
        typename SlotHolder::Ptr slot = signalError_->template Connect<StateTriggerObserver<T> >(observer, &StateTriggerObserver<T>::OnError);
        observer->AddSlot(observer, slot);

        slot = signalCompleted_->template Connect<StateTriggerObserver<T> >(observer, &StateTriggerObserver<T>::OnComplete);
        observer->AddSlot(observer, slot);

        slot = signalTriggerNext_->template Connect<StateTriggerObserver<T> >(observer, &StateTriggerObserver<T>::OnTriggerNext);
        observer->AddSlot(observer, slot);

        return slot;
    }

    virtual bool Disconnect(StateTriggerObserver<T> *observer)
    {
        signalError_->template Disconnect<StateTriggerObserver<T> >(observer, &StateTriggerObserver<T>::OnError);
        signalCompleted_->template Disconnect<StateTriggerObserver<T> >(observer, &StateTriggerObserver<T>::OnComplete);
        return signalTriggerNext_->template Disconnect<StateTriggerObserver<T> >(observer, &StateTriggerObserver<T>::OnTriggerNext);
    }

    virtual void DisconnectAll()
    {
        signalTriggerNext_->DisconnectAll();
        signalError_->DisconnectAll();
        signalCompleted_->DisconnectAll();
    }

    /**
     * @brief OnTriggerNext is called by RxHandler on completion of its task. It notifies the observer
     * about the state that executed and the result through trigger-method.
     *
     * @param state
     * @param result
     */
    virtual void OnTriggerNext(T state)
    {
        signalTriggerNext_->Signal(state);
    }

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(T state, BaseLib::Exception exception)
    {
        signalError_->Signal(state, exception);
    }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnComplete(T state)
    {
        signalCompleted_->Signal(state);
    }

private:
    typename BaseLib::Concurrent::Signaller1<void, T>::Ptr signalTriggerNext_;
    typename BaseLib::Concurrent::Signaller2<void, T, BaseLib::Exception>::Ptr signalError_;
    typename BaseLib::Concurrent::Signaller1<void, T>::Ptr signalCompleted_;
};

}
