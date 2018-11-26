#ifndef BaseLib_Concurrent_Observer_ObserverBase_h_IsIncluded
#define BaseLib_Concurrent_Observer_ObserverBase_h_IsIncluded

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/ObserverEvent.h"
#include"RxObserver/ObserverPolicy.h"
#include"RxObserver/EventFilterCriterion.h"

#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

typedef std::pair<Templates::Any, Templates::Any> EventIdentification;
typedef Templates::Any DelegateIdentification;

// ---------------------------------------------------------
// ObserverBase
// ---------------------------------------------------------

class DLL_STATE ObserverBase
        : public Templates::UnsubscribeMethod<bool>
        , public Templates::IsSubscribedMethod
        , public Templates::BlockMethod
        , public Templates::UnblockMethod
        , public Templates::IsBlockedMethod
        , public Templates::HistoryMethod<ObserverEvents>
        , public Templates::PolicyMethod<ObserverPolicy>
        , public Templates::StatusConstMethod<Status::ObserverStatus>
{
public:
    virtual ~ObserverBase() {}

    CLASS_TRAITS(ObserverBase)

    virtual bool AddFilter(FilterCriterion::Ptr filter) = 0;
};

// ---------------------------------------------------------
// Observer a sort of "future" subscriber that is "Triggered" or executed with new arguments from publisher/subject.
// The variation lies in the arguments, the future is the same until unsubscribed
// TODO: Ability to Subscribe again?
// ---------------------------------------------------------
class DLL_STATE Observer
        : public ObserverBase
        , public Templates::StatusMethod<Status::ObserverStatus>
        , public BaseLib::Observer::ReactiveObserver<ObserverEvent::Ptr, BaseLib::Exception>
        , public BaseLib::Subject::FilterOutSubject1<ObserverEvent::Ptr>
{
public:
    virtual ~Observer() {}

    CLASS_TRAITS(Observer)

    virtual DelegateIdentification  Delegate() const = 0;
    virtual EventIdentification     EventId() const = 0;         // Combines (delegate, function)
};

// ---------------------------------------------------------
// ObserverAction
// ---------------------------------------------------------

class Subject;
FORWARD_CLASS_TRAITS(Subject)

class DLL_STATE ObserverAction
        : public Observer
        , public Templates::Action0<bool>
{
public:
    virtual ~ObserverAction() {}

    CLASS_TRAITS(ObserverAction)

    virtual void SetSubject(SubjectPtr subject) = 0;

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const ObserverAction &observer)
    {
        ostr << "[Observer: "
             << "(" << observer.EventId().first.type().name() << "," << observer.EventId().second.type().name() << ")"
             << ", "
             << observer.Delegate().type().name()
             << ", "
             << observer.StatusConst()
             << "]";

        return ostr;
    }

};

}}

#endif
