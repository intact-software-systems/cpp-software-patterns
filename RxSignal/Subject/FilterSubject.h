#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller1.h"

#include"RxSignal/Observer/FilterObserver.h"

namespace BaseLib { namespace Subject
{

template <typename T>
class FilterInSubject1
        : public Observer::FilterInObserver1<T>
        , public Concurrent::Observable<Observer::FilterInObserver1<T> >
{
public:
    FilterInSubject1()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~FilterInSubject1()
    {}

    void OnFilteredIn(const std::list<T> &values)
    {
        for(T t : values)
        {
            OnFilteredIn(t);
        }
    }

    virtual void OnFilteredIn(T value)
    {
        signaller_->template Signal<Observer::FilterInObserver1<T> >(value, &Observer::FilterInObserver1<T>::OnFilteredIn);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::FilterInObserver1<T> *observer)
    {
        return signaller_->Connect(observer, &Observer::FilterInObserver1<T>::OnFilteredIn);
    }

    virtual bool Disconnect(Observer::FilterInObserver1<T> *observer)
    {
        return signaller_->template Disconnect<Observer::FilterInObserver1<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

template <typename T>
class FilterOutSubject1
        : public Observer::FilterOutObserver1<T>
        , public Concurrent::Observable<Observer::FilterOutObserver1<T> >
{
public:
    FilterOutSubject1()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~FilterOutSubject1()
    {}

    void OnFilteredOut(const std::list<T> &values)
    {
        for(T t : values)
        {
            OnFilteredOut(t);
        }
    }

    virtual void OnFilteredOut(T value)
    {
        signaller_->template Signal<Observer::FilterOutObserver1<T> >(value, &Observer::FilterOutObserver1<T>::OnFilteredOut);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::FilterOutObserver1<T> *observer)
    {
        return signaller_->Connect(observer, &Observer::FilterOutObserver1<T>::OnFilteredOut);
    }

    virtual bool Disconnect(Observer::FilterOutObserver1<T> *observer)
    {
        return signaller_->template Disconnect<Observer::FilterOutObserver1<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

template <typename T>
class FilterSubject1
        : public Observer::FilterObserver1<T>
        , public Concurrent::Observable<Observer::FilterObserver1<T> >
{
public:
    FilterSubject1()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~FilterSubject1()
    {}

    void OnFilteredOut(const std::list<T> &values)
    {
        for(T t : values)
        {
            OnFilteredOut(t);
        }
    }

    void OnFilteredIn(const std::list<T> &values)
    {
        for(T t : values)
        {
            OnFilteredIn(t);
        }
    }

    virtual void OnFilteredIn(T value)
    {
        signaller_->template Signal<Observer::FilterObserver1<T> >(value, &Observer::FilterObserver1<T>::OnFilteredIn);
    }

    virtual void OnFilteredOut(T value)
    {
        signaller_->template Signal<Observer::FilterObserver1<T> >(value, &Observer::FilterObserver1<T>::OnFilteredOut);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::FilterObserver1<T> *observer)
    {
        signaller_->Connect(observer, &Observer::FilterObserver1<T>::OnFilteredIn);
        return signaller_->Connect(observer, &Observer::FilterObserver1<T>::OnFilteredOut);
    }

    virtual bool Disconnect(Observer::FilterObserver1<T> *observer)
    {
        return signaller_->template Disconnect<Observer::FilterObserver1<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

}}
