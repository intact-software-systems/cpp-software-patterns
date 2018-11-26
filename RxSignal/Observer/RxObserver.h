#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/CommonDefines.h"

namespace BaseLib {

template <typename T>
class RxEvents
{
public:
    virtual ~RxEvents()
    { }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnComplete() = 0;

    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    virtual void OnNext(T t) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(GeneralException exception) = 0;
};


template <typename T>
class RxObserver
    : public RxEvents<T>
      , public Concurrent::ObserverSlot<RxObserver<T> >
{
};


template <typename T, typename U>
class NextEvent2
{
public:
    virtual ~NextEvent2()
    { }

    virtual void OnNext(T t, U u) = 0;
};


template <typename T, typename U>
class NextEvent2Observer
    : public NextEvent2<T, U>
      , public Concurrent::ObserverSlot<NextEvent2Observer<T, U>>
{
};

}
