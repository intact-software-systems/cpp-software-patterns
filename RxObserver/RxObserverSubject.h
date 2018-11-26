#pragma once

#include"RxObserver/RxObserver.h"

namespace BaseLib {

/**
 * DONE: Input qos policies to define behavior of the signallers.
 * DONE: Subscribe to OnFilteredOut function, can be reimplemented
 * DONE: Create SubjectsProxy
 * DONE: Add filters to individual events
 * DONE: Make history for individual events available through subject
 * DONE: Subject should return Event::Ptr for each event function, this will however mean that subject cannot implement RxEvents<T>
 *
 * TODO: return values for functions
 * TODO: Define SubjectDescription("RxEvents") as pure virtual getter in SubjectsProxy?
 * TODO: When subject is activated or deactivated, notify the observers.
 *
 * Dynamically Initialize SubjectsProxy with a list of SubjectId/Functions/EventTypes based on function signatures, for example, RxEvents<T>::OnNext, etc.
 * Use this list to identify which which event to collect History from, etc.
 */
template <typename T>
class RxObserverSubjectNew
    : public Concurrent::SubjectEventsType<RxEventsReturn<T>, RxEventsSource<T>>
{
public:
    RxObserverSubjectNew(
        Concurrent::SubjectDescription description = RxEventsSource<T>::DefaultDescription(),
        Concurrent::SubjectPolicy policy = Concurrent::SubjectPolicy::Default())
        : Concurrent::SubjectEventsType<RxEventsReturn<T>, RxEventsSource<T>>(RxEventsSource<T>::CreatePtr(), description, policy)
    { }

    virtual ~RxObserverSubjectNew()
    { }

//    Events OnNextHistory() const
//    {
//        typename Subject1<void, T>::Ptr subjectNext = this->event().subject()->template Create<bool, T>(&RxEventsReturn<T>::OnNext);
//        return subjectNext->History();
//    }
};

}
