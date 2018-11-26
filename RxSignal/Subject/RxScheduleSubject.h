#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller0.h"
#include"RxSignal/Signaller1.h"
#include"RxSignal/Signaller2.h"

#include"RxSignal/Observer/RxScheduleObserver.h"

namespace BaseLib
{

template <typename T>
class RxScheduleSubject
        : public BaseLib::Observer::RxScheduleObserver<T>
        , public BaseLib::Concurrent::Observable< BaseLib::Observer::RxScheduleObserver<T> >
{
public:
    RxScheduleSubject()
        : signalInit_(new BaseLib::Concurrent::Signaller1<void, T>())
        , signalSchedule_(new BaseLib::Concurrent::Signaller2<void, T, int>())
        , signalReady_(new BaseLib::Concurrent::Signaller1<void, T>())
        , signalAbort_(new BaseLib::Concurrent::Signaller2<void, T, BaseLib::GeneralException>())
        , signalDispose_(new BaseLib::Concurrent::Signaller1<void, T>())
    {}
    virtual ~RxScheduleSubject()
    {}

    CLASS_TRAITS(RxScheduleSubject)

    virtual BaseLib::Concurrent::SlotHolder::Ptr Connect(BaseLib::Observer::RxScheduleObserver<T> *observer)
    {
        BaseLib::Concurrent::SlotHolder::Ptr slot = signalInit_->template Connect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnInit);
        observer->AddSlot(observer, slot);

        slot = signalSchedule_->template Connect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnSchedule);
        observer->AddSlot(observer, slot);

        slot = signalAbort_->template Connect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnAbort);
        observer->AddSlot(observer, slot);

        slot = signalDispose_->template Connect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnDispose);
        observer->AddSlot(observer, slot);

        slot = signalReady_->template Connect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnReady);
        observer->AddSlot(observer, slot);

        return slot;
    }

    virtual bool Disconnect(BaseLib::Observer::RxScheduleObserver<T> *observer)
    {
        signalInit_->template Disconnect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnInit);
        signalSchedule_->template Disconnect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnSchedule);
        signalAbort_->template Disconnect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnAbort);
        signalDispose_->template Disconnect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnDispose);
        return signalReady_->template Disconnect<BaseLib::Observer::RxScheduleObserver<T> >(observer, &BaseLib::Observer::RxScheduleObserver<T>::OnReady);
    }

    virtual void DisconnectAll()
    {
        signalInit_->DisconnectAll();
        signalSchedule_->DisconnectAll();
        signalAbort_->DisconnectAll();
        signalDispose_->DisconnectAll();
        signalReady_->DisconnectAll();
    }

    /**
     * @brief OnInit Notifies the scheduler that the task should be added but not triggered.
     */
    virtual void OnInit(T value)
    {
        signalInit_->Signal(value);
    }

    /**
     * @brief OnSchedule Notifies the scheduler that the task should be triggered msecs into the future.
     */
    virtual void OnSchedule(T value, int msecs)
    {
        signalSchedule_->Signal(value, msecs);
    }

    /**
     * @brief OnReady Notifies the scheduler that the task is ready to run.
     */
    virtual void OnReady(T state)
    {
        signalReady_->Signal(state);
    }

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnAbort(T state, BaseLib::GeneralException exception)
    {
        signalAbort_->Signal(state, exception);
    }

    /**
     * @brief OnCompleted Notifies the Observer that the Observable has finished sending push-based notifications.
     * The Observable will not call this closure if it calls onError.
     */
    virtual void OnDispose(T state)
    {
        signalDispose_->Signal(state);
    }

private:
    typename BaseLib::Concurrent::Signaller1<void, T>::Ptr signalInit_;
    typename BaseLib::Concurrent::Signaller2<void, T, int>::Ptr signalSchedule_;
    typename BaseLib::Concurrent::Signaller1<void, T>::Ptr signalReady_;
    typename BaseLib::Concurrent::Signaller2<void, T, BaseLib::GeneralException>::Ptr signalAbort_;
    typename BaseLib::Concurrent::Signaller1<void, T>::Ptr signalDispose_;
};

}
