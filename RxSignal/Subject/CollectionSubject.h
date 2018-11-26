#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller1.h"

#include"RxSignal/Observer/CollectionObserver.h"

namespace BaseLib { namespace Subject
{

template <typename T>
class CollectionSubject1
        : public Observer::CollectionObserver1<T>
        , public Concurrent::Observable<Observer::CollectionObserver1<T> >
{
public:
    CollectionSubject1()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~CollectionSubject1()
    {}

    virtual void OnDataIn(T value)
    {
        signaller_->template Signal<Observer::CollectionObserver1<T> >(value, &Observer::CollectionObserver1<T>::OnDataIn);
    }

    virtual void OnDataOut(T value)
    {
        signaller_->template Signal<Observer::CollectionObserver1<T> >(value, &Observer::CollectionObserver1<T>::OnDataOut);
    }

    virtual void OnDataModified(T value)
    {
        signaller_->template Signal<Observer::CollectionObserver1<T> >(value, &Observer::CollectionObserver1<T>::OnDataModified);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::CollectionObserver1<T> *observer)
    {
        signaller_->Connect(observer, &Observer::CollectionObserver1<T>::OnDataIn);
        signaller_->Connect(observer, &Observer::CollectionObserver1<T>::OnDataOut);
        return signaller_->Connect(observer, &Observer::CollectionObserver1<T>::OnDataModified);
    }

    virtual bool Disconnect(Observer::CollectionObserver1<T> *observer)
    {
        return signaller_->template Disconnect<Observer::CollectionObserver1<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

}}

