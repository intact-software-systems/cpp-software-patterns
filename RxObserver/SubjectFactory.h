#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/SubscriptionBase.h"
#include"RxObserver/ObserverBase.h"
#include"RxObserver/SubjectDescription.h"
#include"RxObserver/Subjects.h"
#include"RxObserver/SubjectGroup.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// -------------------------------------------------
// Static access to enable "subscription" from observers that do not have access to the subject objcet instances.
// Subscription can enable "many to one" "one to many" and "many to many".
// Multicast: How to enable object being both "subject and observer".
// -------------------------------------------------

/**
 * TODO: When a subject is done, call Complete. How to detect done when multiple subjects?
 *      - I need to keep track of active subjects.
 *      - SubjectManager/Monitor/Supervisor that observers activate/deactivate and removes when done.
 *
 * TODO: When a subject has an error, call Error. Monitor this also.
 *
 */
class DLL_STATE SubjectFactory
        : public Templates::FactorySingleton<SubjectFactory, Mutex>
{
public:
    typedef Collection::IMap<SubjectDescription, Subjects::Ptr> SubjectsMap;

public:
    SubjectFactory();
    virtual ~SubjectFactory();

    // -----------------------------------------
    // Create subjects
    // -----------------------------------------

    template <typename Return, typename EventType>
    typename Subject0<Return>::Ptr Create(SubjectDescription description,
                                          SubjectId subjectId,
                                          SubjectPolicy policy)
    {
        typename SubjectGroup<EventType>::Ptr subjects = Create<EventType>(description);
        return subjects->template Create<Return>(subjectId, policy);
    }


    template <typename Return, typename Arg1, typename EventType>
    typename Subject1<Return, Arg1>::Ptr Create(SubjectDescription description,
                                                SubjectId subjectId,
                                                SubjectPolicy policy)
    {
        typename SubjectGroup<EventType>::Ptr subjects = Create<EventType>(description);
        return subjects->template Create<Return, Arg1>(subjectId, policy);
    }

    // -----------------------------------------
    // Find subjects
    // -----------------------------------------

    template <typename Return, typename EventType>
    typename Subject0<Return>::Ptr Find(SubjectDescription description,
                                        SubjectId subjectId)
    {
        typename SubjectGroup<EventType>::Ptr subjects = Find<EventType>(description);
        if(!subjects) return typename Subject0<Return>::Ptr();

        return subjects->template Find<Return>(subjectId);
    }


    template <typename Return, typename Arg1, typename EventType>
    typename Subject1<Return, Arg1>::Ptr Find(SubjectDescription description,
                                              SubjectId subjectId)
    {
        typename SubjectGroup<EventType>::Ptr subjects = Find<EventType>(description);
        if(!subjects) return typename Subject1<Return, Arg1>::Ptr();

        return subjects->template Find<Return, Arg1>(subjectId);
    }

    // -----------------------------------------
    // Create subjects
    // -----------------------------------------

    /**
     * Creates a new Subject object and returns it. The Subject object is saved in the
     * factory and can be accessed by its name later.
     */
    template <typename EventType>
    typename SubjectGroup<EventType>::Ptr Create(SubjectDescription description, SubjectPolicy policy = SubjectPolicy::Default())
    {
        Locker lock(this);

        typename SubjectGroup<EventType>::Ptr subjectsType = this->find<EventType>(description);

        if(!subjectsType)
        {
            subjectsType = typename SubjectGroup<EventType>::Ptr( new SubjectGroup<EventType>(description, policy));
            subjects_.put(description, subjectsType->GetPtr());
        }

        return subjectsType;
    }

    // -----------------------------------------
    // Find subjects
    // -----------------------------------------

    /**
     * To retrieve a Subject based on the SubjectDescription. If the specified description
     * does not identify an existing Subject, a NULL is returned.
     */
    template <typename EventType>
    typename SubjectGroup<EventType>::Ptr Find(SubjectDescription description)
    {
        Locker lock(this);

        return this->find<EventType>(description);
    }

    /**
     * To delete a Subject. This operation releases all the resources allocated to the Subject.
     */
    void Delete(Subjects::Ptr subject);

    /**
     * To delete a Subject. This operation releases all the resources allocated to the Subject.
     */
    void Delete(SubjectDescription description);

    /**
     * Clears (not removes) all Subject objects created by this Factory.
     */
    void ClearAll();

    /**
     * Clears and removes all Subject objects created by this Factory.
     */
    void DeleteAll();


private:

    template <typename EventType>
    typename SubjectGroup<EventType>::Ptr find(SubjectDescription description)
    {
        Subjects::Ptr subjects = subjects_.get(description);

        return std::dynamic_pointer_cast<SubjectGroup<EventType>>(subjects);
    }

private:
    SubjectsMap subjects_;
};

}}

