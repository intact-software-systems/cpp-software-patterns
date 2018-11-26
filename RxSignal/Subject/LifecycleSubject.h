#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller0.h"
#include"RxSignal/Signaller1.h"

#include"RxSignal/Observer/LifecycleObserver.h"

namespace BaseLib { namespace Subject
{

template <typename T>
class FactorySubject
        : public Observer::FactoryObserver<T>
        , public Concurrent::Observable<Observer::FactoryObserver<T> >
{
public:
    FactorySubject()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~FactorySubject()
    {}

    virtual void OnCreate(T t)
    {
        signaller_->Signal(t);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::FactoryObserver<T> *observer)
    {
        return signaller_->Connect(observer, &Observer::FactoryObserver<T>::OnCreate);
    }

    virtual bool Disconnect(Observer::FactoryObserver<T> *observer)
    {
        return signaller_->template Disconnect<Observer::FactoryObserver<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

}}

