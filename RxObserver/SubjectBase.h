#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Subscription.h"
#include"RxObserver/SubjectPolicy.h"
#include"RxObserver/SubjectId.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// --------------------------------------------------
// Is implemented by all specialized signallers/subjects
// TODO: public Templates::IsSubscribedMethod
// --------------------------------------------------

class DLL_STATE SubjectBase
        : public Templates::BlockMethod
        , public Templates::UnblockMethod
        , public Templates::IsBlockedMethod
        , public Templates::IsEmptyMethod
        , public Templates::IsActiveMethod
        , public Templates::SizeMethod<int>
        , public Templates::SubscribeMethod1<Subscription::Ptr, std::shared_ptr<ObserverAction>>
        , public Templates::UnsubscribeMethod1<bool, std::shared_ptr<ObserverAction>>
        , public Templates::PolicyMethod<SubjectPolicy>
        , public Templates::HistoryMethod<Events>
        , public Templates::IdMethod<SubjectId>
        , public Templates::StatusConstMethod<Status::SubjectStatus>
{
public:
    virtual ~SubjectBase() {}

    CLASS_TRAITS(SubjectBase)

    virtual bool Filter(Event::Ptr event) = 0;
    virtual bool AddFilter(FilterCriterion::Ptr filter) = 0;

    virtual bool Submit(Event::Ptr event) = 0;
    virtual bool Submit(Events events) = 0;

    virtual bool UnsubscribeAll() = 0;
};

// --------------------------------------------------
// Is implemented by all specialized signallers/subjects
// --------------------------------------------------

class DLL_STATE Subject
        : public SubjectBase
        , public Templates::ReactorMethods<Event::Ptr, Event::Ptr, BaseLib::Exception>
        , public BaseLib::Subject::FilterOutSubject1<Event::Ptr>
{
public:
    virtual ~Subject() {}

    CLASS_TRAITS(Subject)

    friend std::ostream &operator<<(std::ostream &ostr, const Subject &subject)
    {
        ostr << "[Subject: "
             << subject.Id().Id().type().name()
             << ", "
             << subject.StatusConst()
             << "]";

        return ostr;
    }

};

}}
