#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/SignalSlotBase.h"

namespace BaseLib { namespace Observer {
template <typename T>
class RxScheduleEvents
{
public:
    /**
     * @brief OnInit Notifies the scheduler that the task should be added but not triggered.
     */
    virtual void OnInit(T value) = 0;

    /**
     * @brief OnSchedule Notifies the scheduler that the task should be triggered msecs into the future.
     */
    virtual void OnSchedule(T value, int msecs) = 0;

    /**
     * @brief OnReady Notifies the scheduler that the task is ready to run.
     */
    virtual void OnReady(T value) = 0;

    /**
     * @brief OnDispose Notifies the scheduler that the task can be removed.
     */
    virtual void OnDispose(T value) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnAbort(T value, BaseLib::GeneralException exception) = 0;

    /**
     * @brief The Type enum is used by Reactors to identify events.
     */
    enum class DLL_STATE Type
    {
        INIT,
        SCHEDULE,
        READY,
        DISPOSE,
        ABORT
    };
};


template <typename T>
class RxScheduleObserver
        : public RxScheduleEvents<T>
        , public Concurrent::ObserverSlot< RxScheduleObserver<T> >
{ };

}}
