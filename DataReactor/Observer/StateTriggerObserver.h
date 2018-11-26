#pragma once

#include"DataReactor/IncludeExtLibs.h"

namespace Reactor {

template <typename T>
class StateTriggerEvents
{
public:
    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnComplete(T state) = 0;

    /**
     * @brief OnTriggerNext is called by RxHandler on completion of its task. It notifies the observer
     * about the state that executed and the result through trigger-method.
     */
    virtual void OnTriggerNext(T state) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(T state, BaseLib::Exception exception) = 0;
};

template <typename T>
class StateTriggerObserver
    : public StateTriggerEvents<T>
      , public ObserverSlot<StateTriggerObserver<T> >
{
};

}
