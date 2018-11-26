#include "RxObserver/Subjects.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// SubjectsState
// ----------------------------------------------------

SubjectsState::SubjectsState(SubjectDescription description)
    : description_(description)
{ }

SubjectsState::~SubjectsState()
{ }

const SubjectsState::SubjectList &SubjectsState::subjects() const
{
    return subjects_;
}

SubjectsState::SubjectList &SubjectsState::subjects()
{
    return subjects_;
}

const SubjectDescription &SubjectsState::Id() const
{
    return description_;
}

// ----------------------------------------------------
// Subjects
// ----------------------------------------------------

Subjects::Subjects(SubjectDescription description, SubjectPolicy policy)
    : Templates::ContextObject<SubjectsConfig, SubjectsState>(SubjectsConfig(policy), SubjectsState(description))
{ }

Subjects::~Subjects()
{ }

Subjects::Ptr Subjects::GetPtr()
{
    return shared_from_this();
}

SubjectsState::SubjectList Subjects::GetSubjects() const
{
    Locker locker(this);
    return this->state().subjects();
}

//void Subjects::Add(Subject::Ptr subject)
//{
//    Locker locker(this);
//    this->state().subjects().put(subject->Id(), subject);
//}

//bool Subjects::Remove(Subject::Ptr subject)
//{
//    Locker locker(this);
//    return this->state().subjects().remove(subject->Id()) != nullptr;
//}

bool Subjects::Block()
{
    Locker locker(this);
    for(Subject::Ptr subject : this->state().subjects())
    {
        subject->Block();
    }
    return true;
}

bool Subjects::Unblock()
{
    Locker locker(this);
    for(Subject::Ptr subject : this->state().subjects())
    {
        subject->Unblock();
    }
    return true;
}

bool Subjects::IsBlocked() const
{
    Locker locker(this);
    for(Subject::Ptr subject : this->state().subjects())
    {
        if(!subject->IsBlocked()) return false;
    }
    return true;
}

bool Subjects::Unsubscribe()
{
    Locker locker(this);
    for(Subject::Ptr subject : this->state().subjects())
    {
        subject->UnsubscribeAll();
    }
    return true;
}

bool Subjects::IsEmpty() const
{
    Locker locker(this);
    return this->state().subjects().empty();
}

int Subjects::Size() const
{
    Locker locker(this);
    return this->state().subjects().size();
}

SubjectDescription Subjects::Description() const
{
    Locker locker(this);
    return this->state().Id();
}

SubjectPolicy Subjects::Policy() const
{
    return this->config().DefaultSubjectPolicy();
}

Events Subjects::History() const
{
    Locker locker(this);

    Events history;

    for(Subject::Ptr subject : this->state().subjects())
    {
        history.merge(subject->History());
    }

    return history;
}

Subject::Ptr Subjects::Find(SubjectId subjectId) const
{
    for(Subject::Ptr subject : this->state().subjects())
    {
        if(subject->Id().Id() == subjectId.Id()) {
            return subject;
        }
    }

    return Subject::Ptr();
}

}}
