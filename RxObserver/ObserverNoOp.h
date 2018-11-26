#pragma once

#include"RxObserver/ObserverBase.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

class DLL_STATE ObserverNoOp
        : public ObserverAction
        , public StaticSingletonPtr<ObserverNoOp>
{
public:
    ObserverNoOp()
    {}
    virtual ~ObserverNoOp()
    {}

    CLASS_TRAITS(ObserverNoOp)

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual void OnError(BaseLib::Exception)
    { }

    virtual void OnComplete()
    { }

    virtual void OnNext(ObserverEvent::Ptr)
    { }

    virtual const Status::ObserverStatus &StatusConst() const
    {
        static Status::ObserverStatus status;
        return status;
    }

    virtual ObserverPolicy Policy() const
    {
        return ObserverPolicy::Default();
    }

    virtual ObserverEvents History() const
    {
        return ObserverEvents();
    }

    virtual bool IsBlocked() const
    {
        return false;
    }

    virtual bool Unblock()
    {
        return false;
    }

    virtual bool Block()
    {
        return false;
    }

    virtual bool IsSubscribed() const
    {
        return false;
    }

    virtual bool Unsubscribe()
    {
        return false;
    }

    virtual bool Invoke()
    {
        return false;
    }

    virtual Status::ObserverStatus &Status()
    {
        static Status::ObserverStatus status;
        return status;
    }

    virtual bool AddFilter(FilterCriterion::Ptr)
    {
        return false;
    }

    virtual DelegateIdentification Delegate() const
    {
        return DelegateIdentification::Empty();
    }

    virtual EventIdentification EventId() const
    {
        return EventIdentification();
    }

    virtual void SetSubject(SubjectPtr)
    {

    }
};

}}
