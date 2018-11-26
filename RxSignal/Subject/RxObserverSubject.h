#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller0.h"
#include"RxSignal/Signaller1.h"

#include"RxSignal/Observer/RxObserver.h"

namespace BaseLib {

template <typename T>
class RxObserverSubject
    : public RxEvents<T>
      , public Concurrent::Observable<RxObserver<T> >
{
public:
    RxObserverSubject()
        : onNextSignaller_(new Concurrent::Signaller1<void, T>())
        , onErrorSignaller_(new Concurrent::Signaller1<void, BaseLib::GeneralException>())
        , onCompletedSignaller_(new Concurrent::Signaller0<void>())
    { }

    virtual ~RxObserverSubject()
    { }

    virtual typename Concurrent::SlotHolder::Ptr Connect(RxObserver<T>* observer)
    {
        typename Concurrent::SlotHolder::Ptr slot = onNextSignaller_->template Connect<RxObserver<T> >(observer, &RxObserver<T>::OnNext);
        observer->AddSlot(observer, slot);

        slot = onErrorSignaller_->template Connect<RxObserver<T> >(observer, &RxObserver<T>::OnError);
        observer->AddSlot(observer, slot);

        slot = onCompletedSignaller_->template Connect<RxObserver<T> >(observer, &RxObserver<T>::OnComplete);
        observer->AddSlot(observer, slot);

        return slot;
    }

    virtual bool Disconnect(RxObserver<T>* observer)
    {
        onNextSignaller_->template Disconnect<RxObserver<T> >(observer);
        onErrorSignaller_->template Disconnect<RxObserver<T> >(observer);
        return onCompletedSignaller_->template Disconnect<RxObserver<T> >(observer);
    }

    virtual void DisconnectAll()
    {
        onNextSignaller_->DisconnectAll();
        onErrorSignaller_->DisconnectAll();
        onCompletedSignaller_->DisconnectAll();
    }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnComplete()
    {
        onCompletedSignaller_->Signal();
    }

    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    virtual void OnNext(T t)
    {
        onNextSignaller_->Signal(t);
    }

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(BaseLib::GeneralException exception)
    {
        onErrorSignaller_->Signal(exception);
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr                         onNextSignaller_;
    typename Concurrent::Signaller1<void, BaseLib::GeneralException>::Ptr onErrorSignaller_;
    typename Concurrent::Signaller0<void>::Ptr                            onCompletedSignaller_;
};


template <typename T, typename U>
class NextEvent2Subject
    : public NextEvent2<T, U>
      , public Concurrent::Observable<NextEvent2Observer<T, U>>
{
public:
    NextEvent2Subject()
        : signaller_(new Concurrent::Signaller2<void, T, U>())
    { }

    virtual ~NextEvent2Subject()
    { }

    virtual void OnNext(T t, U u)
    {
        signaller_->Signal(t, u);
    }

    virtual typename SlotHolder::Ptr Connect(NextEvent2Observer<T, U>* observer)
    {
        auto slot = signaller_->template Connect<NextEvent2Observer<T, U>>(observer, &NextEvent2<T, U>::OnNext);
        observer->AddSlot(observer, slot);
        return slot;
    }

    virtual bool Disconnect(NextEvent2Observer<T, U>* observer)
    {
        return signaller_->template Disconnect<NextEvent2Observer<T, U>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    std::shared_ptr<Concurrent::Signaller2<void, T, U>> signaller_;
};

}

