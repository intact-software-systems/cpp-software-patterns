#include "RxObserver/SubjectFactory.h"

namespace BaseLib { namespace Concurrent
{

SubjectFactory::SubjectFactory()
{ }

SubjectFactory::~SubjectFactory()
{ }

void SubjectFactory::Delete(Subjects::Ptr subject)
{
    Locker lock(this);

    subjects_.remove(subject->Description());
}

void SubjectFactory::Delete(SubjectDescription description)
{
    Locker lock(this);

    subjects_.remove(description);
}

void SubjectFactory::ClearAll()
{
//    for(Subjects::Ptr subject : subjects_)
//    {
//    }
}

void SubjectFactory::DeleteAll()
{

}


}}
