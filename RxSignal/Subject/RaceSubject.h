#pragma once

#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller0.h"
#include"RxSignal/Signaller1.h"

#include"RxSignal/Observer/RaceObserver.h"

namespace BaseLib { namespace Subject
{

/**
 * @brief The TakeYourMarkSubject class
 */
class TakeYourMarkSubject
        : public Observer::TakeYourMarkObserver
        , public Concurrent::Observable<Observer::TakeYourMarkObserver>
{
public:
    TakeYourMarkSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~TakeYourMarkSubject()
    {}

    virtual void OnTakeYourMark()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(BaseLib::Observer::TakeYourMarkObserver *observer)
    {
        return signaller_->Connect(observer, &BaseLib::Observer::TakeYourMarkObserver::OnTakeYourMark);
    }

    virtual bool Disconnect(BaseLib::Observer::TakeYourMarkObserver *observer)
    {
        return signaller_->Disconnect<BaseLib::Observer::TakeYourMarkObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The GetSetSubject class
 */
class GetSetSubject
        : public Observer::GetSetObserver
        , public Concurrent::Observable<Observer::GetSetObserver>
{
public:
    GetSetSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~GetSetSubject()
    {}

    virtual void OnGetSet()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::GetSetObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::GetSetObserver::OnGetSet);
    }

    virtual bool Disconnect(Observer::GetSetObserver *observer)
    {
        return signaller_-> template Disconnect<Observer::GetSetObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }


private:
    Concurrent::Signaller0<void>::Ptr signaller_;

};

/**
 * @brief The StartSubject class
 */
class StartSubject
        : public Observer::StartObserver
        , public Concurrent::Observable<Observer::StartObserver>
{
public:
    StartSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~StartSubject()
    {}

    virtual void OnStart()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::StartObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::StartObserver::OnStart);
    }

    virtual bool Disconnect(Observer::StartObserver *observer)
    {
        return signaller_->Disconnect<Observer::StartObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The StopSubject class
 */
class StopSubject
        : public Observer::StopObserver
        , public Concurrent::Observable<Observer::StopObserver>
{
public:
    StopSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~StopSubject()
    {}

    virtual void OnStop()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::StopObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::StopObserver::OnStop);
    }

    virtual bool Disconnect(Observer::StopObserver *observer)
    {
        return signaller_->Disconnect<Observer::StopObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }


private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The AbortSubject class
 */
class AbortSubject
        : public Observer::AbortObserver
        , public Concurrent::Observable<Observer::AbortObserver>
{
public:
    AbortSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~AbortSubject()
    {}

    virtual void OnAbort()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::AbortObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::AbortObserver::OnAbort);
    }

    virtual bool Disconnect(Observer::AbortObserver *observer)
    {
        return signaller_->Disconnect<Observer::AbortObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }


private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The SuccessSubject class
 */
class SuccessSubject
        : public Observer::SuccessObserver
        , public Concurrent::Observable<Observer::SuccessObserver>
{
public:
    SuccessSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~SuccessSubject()
    {}

    virtual void OnSuccess()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::SuccessObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::SuccessObserver::OnSuccess);
    }

    virtual bool Disconnect(Observer::SuccessObserver *observer)
    {
        return signaller_->Disconnect<Observer::SuccessObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }


private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The FailureSubject class
 */
class FailureSubject
        : public Observer::FailureObserver
        , public Concurrent::Observable<Observer::FailureObserver>
{
public:
    FailureSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~FailureSubject()
    {}

    virtual void OnFailure()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::FailureObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::FailureObserver::OnFailure);
    }

    virtual bool Disconnect(Observer::FailureObserver *observer)
    {
        return signaller_->Disconnect<Observer::FailureObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    Concurrent::Signaller0<void>::Ptr signaller_;

};

/**
 * @brief The ErrorSubject class
 */
class ErrorSubject
        : public Observer::ErrorObserver
        , public Concurrent::Observable<Observer::ErrorObserver>
{
public:
    ErrorSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~ErrorSubject()
    {}

    virtual void OnError()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::ErrorObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::ErrorObserver::OnError);
    }

    virtual bool Disconnect(Observer::ErrorObserver *observer)
    {
        return signaller_->Disconnect<Observer::ErrorObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The ErrorSubject class
 */
template <typename T>
class ErrorSubject1
        : public Observer::ErrorObserver1<T>
        , public Concurrent::Observable<Observer::ErrorObserver1<T> >
{
public:
    ErrorSubject1()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~ErrorSubject1()
    {}

    virtual void OnError(T t)
    {
        signaller_->Signal(t);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::ErrorObserver1<T> *observer)
    {
        return signaller_->Connect(observer, &Observer::ErrorObserver1<T>::OnError);
    }

    virtual bool Disconnect(Observer::ErrorObserver *observer)
    {
        return signaller_-> template Disconnect<Observer::ErrorObserver1<T> >(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};


/**
 * @brief The CompletedSubject class
 */
class CompleteSubject
        : public Observer::CompleteObserver
        , public Concurrent::Observable<Observer::CompleteObserver>
{
public:
    CompleteSubject()
        : signaller_(new Concurrent::Signaller0<void>())
    {}
    virtual ~CompleteSubject()
    {}

    virtual void OnComplete()
    {
        signaller_->Signal();
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::CompleteObserver *observer)
    {
        return signaller_->Connect(observer, &Observer::CompleteObserver::OnComplete);
    }

    virtual bool Disconnect(Observer::CompleteObserver *observer)
    {
        return signaller_->Disconnect<Observer::CompleteObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

/**
 * @brief The CompletedSubject class
 */
template <typename T>
class OneArgCompleteSubject
        : public Observer::OneArgCompleteObserver<T>
        , public Concurrent::Observable<Observer::OneArgCompleteObserver<T> >
{
public:
    OneArgCompleteSubject()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~OneArgCompleteSubject()
    {}

    virtual void OnComplete(T t)
    {
        signaller_->Signal(t);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::OneArgCompleteObserver<T> *observer)
    {
        return signaller_->Connect(observer, &Observer::OneArgCompleteObserver<T>::OnComplete);
    }

    virtual bool Disconnect(Observer::OneArgCompleteObserver<T> *observer)
    {
        return signaller_->template Disconnect<Observer::OneArgCompleteObserver<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

/**
 * @brief The NextSubject class
 */
template <typename T>
class NextSubject
        : public Observer::NextObserver<T>
        , public Concurrent::Observable<Observer::NextObserver<T> >
{
public:
    NextSubject()
        : signaller_(new Concurrent::Signaller1<void, T>())
    {}
    virtual ~NextSubject()
    {}

    virtual void OnNext(T t)
    {
        signaller_->Signal(t);
    }

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::NextObserver<T> *observer)
    {
        return signaller_->Connect(observer, &Observer::NextObserver<T>::OnNext);
    }

    virtual bool Disconnect(Observer::NextObserver<T> *observer)
    {
        return signaller_->template Disconnect<Observer::NextObserver<T>>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};

/**
 * @brief The RaceSubject class
 */
class RaceSubject
        : public TakeYourMarkSubject
        , public GetSetSubject
        , public StartSubject
        , public StopSubject
        , public AbortSubject
        , public SuccessSubject
        , public FailureSubject
        , public ErrorSubject
        , public CompleteSubject
{ };


/**
 * @brief The ReactiveSubject class
 */
template <typename T, typename Error>
class ReactiveSubject
        : public ErrorSubject1<Error>
        , public CompleteSubject
        , public NextSubject<T>
{
public:
    virtual ~ReactiveSubject() {}

    virtual Concurrent::SlotHolder::Ptr Connect(Observer::NextObserver<T> *observer)
    {
        ErrorSubject1<Error>::Connect(observer);
        CompleteSubject::Connect(observer);
        return NextSubject<T>::Connect(observer);
    }

    virtual bool Disconnect(Observer::NextObserver<T> *observer)
    {
        ErrorSubject1<Error>::Disconnect(observer);
        CompleteSubject::Disconnect(observer);
        return NextSubject<T>::Disconnect(observer);
    }

    virtual void DisconnectAll()
    {
        ErrorSubject1<Error>::DisconnectAll();
        CompleteSubject::DisconnectAll();
        NextSubject<T>::DisconnectAll();
    }
};

}}
