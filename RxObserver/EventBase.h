#pragma once

#include"RxObserver/Subjects.h"
#include"RxObserver/SubjectsProxy.h"
#include"RxObserver/ObserverProxy.h"
#include"RxObserver/ObserverEvent.h"

#include"RxSignal/Observer/FilterObserver.h"

namespace BaseLib { namespace Concurrent
{

template <typename EventType, typename T>
class SubjectEventsType
        : public BaseLib::Observer::FilterOutObserver1<BaseLib::Concurrent::Event::Ptr>
{
private:
    typedef SubjectsProxy<EventType, T> Proxy;

public:
    SubjectEventsType(T *eventSource, SubjectDescription description, SubjectPolicy policy)
        : proxy_(eventSource, description, policy)
    {
        proxy_.template Connect<SubjectEventsType<EventType, T>>(this);
    }
    virtual ~SubjectEventsType()
    {
        proxy_.template Disconnect<SubjectEventsType<EventType, T>>(this);
    }

    T& event()
    {
        return proxy_.On();
    }

    const T& event() const
    {
        return proxy_.On();
    }

    Proxy& state()
    {
        return proxy_;
    }

    const Proxy& state() const
    {
        return proxy_;
    }

    // -----------------------------------------
    // FilterOutObserver1 interface
    // -----------------------------------------

    virtual void OnFilteredOut(Concurrent::Event::Ptr )
    {
        IINFO() << "Event filtered out";
    }

    //virtual void OnSubscribe(std::shared_ptr<ObserverAction> ) {}
    //virtual void OnUnsubscribe(std::shared_ptr<ObserverAction> ) {}
    //virtual void OnUnsubscribe() { }
    //virtual void OnBlock(std::shared_ptr<ObserverAction> ) { }
    //virtual void OnUnblock(std::shared_ptr<ObserverAction> ) { }
    //virtual void OnBackpressureDrop(Event::Ptr) { }

private:
    Proxy proxy_;
};


template <typename EventType, typename T>
class ObserverEventType
        : public BaseLib::Observer::FilterOutObserver1<ObserverEvent::Ptr>
{
private:
    typedef ObserverProxy<EventType, T> Proxy;

public:
    ObserverEventType(T *eventSource, SubjectDescription description)
        : proxy_(eventSource, description)
    {
        proxy_.Connect(this);
    }

    virtual ~ObserverEventType()
    {
        proxy_.Disconnect(this);
    }

    Proxy& event()
    {
        return proxy_;
    }

    const Proxy& event() const
    {
        return proxy_;
    }

    // -----------------------------------------
    // FilterOutObserver1 interface
    // -----------------------------------------

    virtual void OnFilteredOut(Concurrent::ObserverEvent::Ptr )
    {
        IINFO() << "Event filtered out";
    }

    //virtual void OnActive(Concurrent::Subject::Ptr ) {}
    //virtual void OnInactive(Concurrent::Subject::Ptr ) {}
    //virtual void OnUnsubscribe() { }
    //virtual void OnBlock(Concurrent::Subject::Ptr ) { }
    //virtual void OnUnblock(Concurrent::Subject::Ptr ) { }
    //virtual void OnBackpressureDrop(Event::Ptr) { }

private:
    Proxy proxy_;
};

}}
