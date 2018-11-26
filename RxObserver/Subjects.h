#pragma once

#include"RxObserver/IncludeExtLibs.h"

#include"RxObserver/SubjectDescription.h"
#include"RxObserver/Subject0.h"
#include"RxObserver/Subject1.h"
#include"RxObserver/Subject2.h"

#include"RxObserver/SubjectId.h"
#include"RxObserver/Subscriptions.h"

#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// SubjectsConfig
// ----------------------------------------------------

class DLL_STATE SubjectsConfig
{
public:
    SubjectsConfig(SubjectPolicy policy = SubjectPolicy::Default())
        : defaultSubjectPolicy_(policy)
    {}
    virtual ~SubjectsConfig()
    {}

    SubjectPolicy DefaultSubjectPolicy() const { return defaultSubjectPolicy_; }

private:
    SubjectPolicy defaultSubjectPolicy_;
};

// ----------------------------------------------------
// SubjectsState
// ----------------------------------------------------

class DLL_STATE SubjectsState
{
public:
    typedef Collection::IList<Subject::Ptr> SubjectList;

public:
    SubjectsState(SubjectDescription description = SubjectDescription::Default());
    virtual ~SubjectsState();

    const SubjectList& subjects() const;
    SubjectList& subjects();

    const SubjectDescription& Id() const;

private:
    SubjectList subjects_;
    SubjectDescription description_;
};

// ----------------------------------------------------
// Subjects
// ----------------------------------------------------

class DLL_STATE Subjects
        : public Templates::ContextObject<SubjectsConfig, SubjectsState>
        , public Templates::LockableType<Subjects, Mutex>
        , public ENABLE_SHARED_FROM_THIS(Subjects)
{
public:
    Subjects(SubjectDescription description = SubjectDescription::Default(), SubjectPolicy policy = SubjectPolicy::Default());
    virtual ~Subjects();

    CLASS_TRAITS(Subjects)

    Subjects::Ptr GetPtr();

    SubjectsState::SubjectList GetSubjects() const;

//    void Add(Subject::Ptr observer);
//    bool Remove(Subject::Ptr observer);

    bool Block();
    bool Unblock();

    bool IsBlocked() const;
    //bool IsActive() const;

    bool Unsubscribe();

    bool IsEmpty() const;
    int Size() const;

    SubjectDescription Description() const;

    SubjectPolicy Policy() const;

    // ---------------------------------------------------------
    // Support pull model interaction
    // ---------------------------------------------------------

    Events History() const;

    Subject::Ptr Find(SubjectId subjectId) const;
};

// ----------------------------------------------------
// class SubjectsType
// TODO: Use template type from c++ 11 that is a "Function signature"
// TODO: Ordering, FIFO/Random/etc. If FIFO then share ringbuffer from subjects-group to all specific subject instances.
// To ensure FIFO ordering then Subjects need a FlowController. Or?
// TOOD: To ensure ordering, add FlowController to this, communicate with ObserversType (contains all Observers).
// ----------------------------------------------------

template <typename EventType>
class SubjectsType : public Subjects
{
private:
    typedef std::shared_ptr<EventType>  EventTypePtr;

public:
    SubjectsType(SubjectDescription description = SubjectDescription::Default(), SubjectPolicy policy = SubjectPolicy::Default())
        : Subjects(description, policy)
    {}
    virtual ~SubjectsType()
    {}

    CLASS_TRAITS(SubjectsType)

    // -----------------------------------------
    // Create subjects
    // -----------------------------------------

    template <typename Return>
    typename Subject0<Return>::Ptr Create(Return (EventType::*member)())
    {
        Locker locker(this);
        return this->create<Subject0<Return>>(SubjectId(member), this->config().DefaultSubjectPolicy());
    }

    template <typename Return, typename Arg1>
    typename Subject1<Return, Arg1>::Ptr Create(Return (EventType::*member)(Arg1))
    {
        Locker locker(this);
        return this->create<Subject1<Return, Arg1>>(SubjectId(member), this->config().DefaultSubjectPolicy());
    }

    template <typename Return, typename Arg1, typename Arg2>
    typename Subject2<Return, Arg1, Arg2>::Ptr Create(Return (EventType::*member)(Arg1, Arg2))
    {
        Locker locker(this);
        return this->create<Subject2<Return, Arg1, Arg2>>(SubjectId(member), this->config().DefaultSubjectPolicy());
    }

    // -----------------------------------------
    // Find subjects
    // -----------------------------------------

    template <typename Return>
    typename Subject0<Return>::Ptr Find(SubjectId subjectId)
    {
        Locker locker(this);
        return this->find<Subject0<Return>>(subjectId);
    }

    template <typename Return, typename Arg1>
    typename Subject1<Return, Arg1>::Ptr Find(SubjectId subjectId)
    {
        Locker locker(this);
        return this->find<Subject1<Return, Arg1>>(subjectId);
    }

    template <typename Return, typename Arg1, typename Arg2>
    typename Subject2<Return, Arg1, Arg2>::Ptr Find(SubjectId subjectId)
    {
        Locker locker(this);
        return this->find<Subject2<Return, Arg1, Arg2>>(subjectId);
    }

    // -----------------------------------------
    // Subscribe functions - Find Subject based on function signature and subscribe
    // -----------------------------------------

    template <typename Return>
    typename Subscription::Ptr Subscribe(EventTypePtr observer, Return (EventType::*member)(), ObserverPolicy policy)
    {
        ASSERT(observer);

        //typedef std::function<Return ()>      FunctionType;

        typedef Templates::Function0<EventType, Return>     FunctionType;
        typedef details::Observer0<FunctionType, Return>    ObserverType;
        typedef Subject0<Return>                            SubjectType;

        typename SubjectType::Ptr subject = Find<Return>(SubjectId(member));

        if(!subject)
        {
            IWARNING() << "Could not find subject!";
            return Subscription::Ptr();
        }

        //FunctionType func = std::bind(member, observer);
        FunctionType func(observer, member);

        typename ObserverType::Ptr observerType(new ObserverType(observer, func, policy));

        return subject->Subscribe(observerType->GetPtr());
    }

    template <typename Return, typename Arg1>
    typename Subscription::Ptr Subscribe(EventTypePtr observer, Return (EventType::*member)(Arg1), ObserverPolicy policy)
    {
        ASSERT(observer);

        //typedef std::function<Return (Arg1)>      FunctionType;

        typedef Templates::Function1<EventType, Return, Arg1>   FunctionType;
        typedef details::Observer1<FunctionType, Return, Arg1>  ObserverType;
        typedef Subject1<Return, Arg1>                          SubjectType;

        typename SubjectType::Ptr subject = Find<Return, Arg1>(SubjectId(member));

        if(!subject)
        {
            IWARNING() << "Could not find subject!";
            return Subscription::Ptr();
        }

        //FunctionType func = std::bind(member, observer, std::placeholders::_1);
        FunctionType func(observer, member);

        typename ObserverType::Ptr observerType(new ObserverType(observer, func, policy));

        return subject->Subscribe(observerType->GetPtr());
    }


    template <typename Return, typename Arg1, typename Arg2>
    typename Subscription::Ptr Subscribe(EventTypePtr observer, Return (EventType::*member)(Arg1, Arg2), ObserverPolicy policy)
    {
        ASSERT(observer);

        //typedef std::function<Return (Arg1, Arg2)>        FunctionType;
        typedef Templates::Function2<EventType, Return, Arg1, Arg2>     FunctionType;
        typedef details::Observer2<FunctionType, Return, Arg1, Arg2>    ObserverType;
        typedef Subject2<Return, Arg1, Arg2>                            SubjectType;

        typename SubjectType::Ptr subject = Find<Return, Arg1, Arg2>(SubjectId(member));

        if(!subject)
        {
            IWARNING() << "Could not find subject!";
            return Subscription::Ptr();
        }

        //FunctionType func = std::bind(member, observer, std::placeholders::_1, std::placeholders::_2);
        FunctionType func(observer, member);

        typename ObserverType::Ptr observerType(new ObserverType(observer, func, policy));

        return subject->Subscribe(observerType->GetPtr());
    }

    // -----------------------------------------
    // Unsubscribe function
    // -----------------------------------------

    bool Unsubscribe(EventTypePtr observer)
    {
        Locker locker(this);
        bool isUnsubscribed = false;

        for(Subject::Ptr subject : this->state().subjects())
        {
            typename details::SubjectBase::Ptr subjectBase = std::dynamic_pointer_cast<details::SubjectBase>(subject);
            ASSERT(subjectBase);

            isUnsubscribed = subjectBase->UnsubscribeType<EventTypePtr>(observer);
        }

        return isUnsubscribed;
    }

    bool IsSubscribedType(EventTypePtr observer) const
    {
        // TODO: How to check is subscribed for all attached subjects?
        bool isSubscribed = false;

        Locker locker(this);
        for(Subject::Ptr subject : this->state().subjects())
        {
            details::SubjectBase::Ptr subjectBase = std::dynamic_pointer_cast<details::SubjectBase>(subject);
            ASSERT(subjectBase);

            isSubscribed = subjectBase->IsSubscribedType<EventTypePtr>(observer);
            if(!isSubscribed) return false;
        }

        return isSubscribed;
    }

    // -----------------------------------------
    // Signal to subjects
    // -----------------------------------------

    template <typename Source, typename Return>
    Event::Ptr Next(Return (EventType::*member)(), Source *source)
    {
        typedef Subject0<Return> SubjectType;

        typename SubjectType::Ptr subject;

        {
            Locker lock(this);
            subject = find<SubjectType>(SubjectId(member));
        }

        if(!subject)
        {
            IWARNING() << "Could not find subject!";
            return Event::Ptr();
        }

        return subject->template Next<Source>(source);
    }

    template <typename Source, typename Return, typename Arg1>
    Event::Ptr Next(Arg1 arg1, Return (EventType::*member)(Arg1), Source *source)
    {
        typedef Subject1<Return, Arg1> SubjectType;

        typename SubjectType::Ptr subject;

        {
            Locker lock(this);
            subject = find<SubjectType>(SubjectId(member));
        }

        if(!subject)
        {
            IWARNING() << "Could not find subject!";
            return Event::Ptr();
        }

        return subject->template Next<Source>(arg1, source);
    }

    template <typename Source, typename Return, typename Arg1, typename Arg2>
    Event::Ptr Next(Arg1 arg1, Arg2 arg2, Return (EventType::*member)(Arg1, Arg2), Source *source)
    {
        typedef Subject2<Return, Arg1, Arg2> SubjectType;

        typename SubjectType::Ptr subject;

        {
            Locker lock(this);
            subject = find<SubjectType>(SubjectId(member));
        }

        if(!subject)
        {
            IWARNING() << "Could not find subject!";
            return Event::Ptr();
        }

        return subject->template Next<Source>(arg1, arg2, source);
    }

    // -----------------------------------------
    // Connect and disconnect as observer to subject events
    // -----------------------------------------

    template <typename EventObserver>
    void ConnectObserver(EventObserver *obs)
    {
        for(Subject::Ptr subject : this->state().subjects())
        {
            subject->Connect(obs);
        }
    }

    template <typename EventObserver>
    void DisconnectObserver(EventObserver *obs)
    {
        for(Subject::Ptr subject : this->state().subjects())
        {
            subject->Disconnect(obs);
        }
    }

private:

    // -----------------------------------------
    // private functions
    // -----------------------------------------

    template <typename SubjectType>
    typename SubjectType::Ptr create(SubjectId subjectId, SubjectPolicy policy)
    {
        typename SubjectType::Ptr subject = this->find<SubjectType>(subjectId);
        if(subject != nullptr)
            return subject;

        typename SubjectType::Ptr subjectType(new SubjectType(subjectId, policy));
        this->state().subjects().push_back(subjectType->GetPtr());

        return subjectType;
    }

    template <typename SubjectType>
    typename SubjectType::Ptr find(SubjectId subjectId)
    {
        for(Subject::Ptr subject : this->state().subjects())
        {
            if(subject->Id() == subjectId) {
                return std::dynamic_pointer_cast<SubjectType>(subject);
            }
        }

        return typename SubjectType::Ptr();
    }
};

}}

