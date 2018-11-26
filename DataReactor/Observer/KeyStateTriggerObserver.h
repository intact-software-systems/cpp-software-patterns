#pragma once

#include"DataReactor/IncludeExtLibs.h"

namespace Reactor {

template <typename K, typename StateType>
class KeyStateTriggerEvents
{
public:
    /**
     * @brief OnTriggerNext is called by RxHandler on completion of its task. It notifies the observer
     * about the state that executed.
     *
     * @param state
     * @param result
     */
    virtual void OnTriggerNext(K key, StateType state) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(K key, StateType state) = 0;

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnComplete(K key, StateType state) = 0;
};

template <typename K, typename StateType>
class KeyStateTriggerObserver
    : public KeyStateTriggerEvents<K, StateType>
      , public ObserverSlot<KeyStateTriggerObserver<K, StateType> >
{
};

}
