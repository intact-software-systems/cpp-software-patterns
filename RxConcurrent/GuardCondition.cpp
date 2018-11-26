/*
 * GuardCondition.cpp
 *
 *  Created on: Apr 12, 2012
 *      Author: knuthelv
 */
#include "RxConcurrent/GuardCondition.h"

namespace BaseLib { namespace Concurrent
{
/*----------------------------------------------------
 GuardCondition implementations
----------------------------------------------------*/
GuardCondition::~GuardCondition()
{
    if(functorHolder_)
    {
        delete functorHolder_;
        functorHolder_ = NULL;
    }
}

/*----------------------------------------------------
    virtual public functions

NB! To avoid infinite loops of triggers reset
    trigger to false
----------------------------------------------------*/
void GuardCondition::Dispatch()
{
    MutexLocker lock(&mutex_);

    if(triggerValue_ && functorHolder_)
    {
        functorHolder_->Invoke();
    }
    // -- debug --
    else if(!triggerValue_)
    {
        IWARNING() << "Trigger value == false";
    }
    // -- debug --

    // NB! To avoid infinite loops of triggers!
    triggerValue_ = false;
}

/*----------------------------------------------------
    public functions: set/reset functor handler
----------------------------------------------------*/
void GuardCondition::ResetHandler()
{
    MutexLocker lock(&mutex_);

    Templates::Functor0* tmp = functorHolder_;
    functorHolder_ = NULL;
    delete tmp;
}

bool GuardCondition::HasHandler() const
{
    MutexLocker lock(&mutex_);

    return (functorHolder_ != NULL);
}

/*----------------------------------------------------
    public functions: set/get trigger value
----------------------------------------------------*/
bool GuardCondition::GetTriggerValue() const
{
    MutexLocker lock(&mutex_);

    return triggerValue_;
}

void GuardCondition::SetTriggerValue(bool triggerValue)
{
    {
        MutexLocker lock(&mutex_);
        triggerValue_ = triggerValue;
    }

    // when to call Signal? true and false?
    if(triggerValue)
    {
        SignalAll();
    }
}

}} // namespace BaseLib::Concurrent

