#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller1.h"

#include"RxSignal/Observer/QueueObserver.h"

namespace BaseLib { namespace Subject
{

template <typename T>
class QueueSubject1
        : public Observer::QueueObserver1<T>
        , public Concurrent::Observable<Observer::QueueObserver1<T> >
{
public:
    QueueSubject1()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~QueueSubject1()
    {}

    virtual void OnDataIn(T value)
    {
        signaller_->template Signal<Observer::QueueObserver1<T> >(value, &Observer::QueueObserver1<T>::OnDataIn);
    }

    virtual void OnDataOut(T value)
    {
        signaller_->template Signal<Observer::QueueObserver1<T> >(value, &Observer::QueueObserver1<T>::OnDataOut);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::QueueObserver1<T> *observer)
    {
        signaller_->Connect(observer, &Observer::QueueObserver1<T>::OnDataIn);
        return signaller_->Connect(observer, &Observer::QueueObserver1<T>::OnDataOut);
    }

    virtual bool Disconnect(Observer::QueueObserver1<T> *observer)
    {
        return signaller_->template Disconnect<Observer::QueueObserver1<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

}}

