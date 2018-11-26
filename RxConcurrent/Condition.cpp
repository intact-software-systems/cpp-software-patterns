/*
 * Condition.cpp
 *
 *  Created on: Apr 12, 2012
 *      Author: knuthelv
 */
#include "RxConcurrent/Condition.h"
#include "RxConcurrent/WaitSet.h"

namespace BaseLib { namespace Concurrent
{

// -------------------------------------------------------
// Base class for Condition implementations
// -------------------------------------------------------
/**
 * @brief Condition::~Condition: Since WaitSet uses shared_pointers it is up to the
 * user to detach from waitSet and then condition is automatically deleted.
 */
AbstractCondition::~AbstractCondition()
{ }

/**
 * @brief Condition::GetPtr
 * @return
 */
AbstractCondition::Ptr AbstractCondition::GetPtr()
{
    return shared_from_this();
}

// -------------------------------------------------------
//	Thread safe implementation
// -------------------------------------------------------
void AbstractCondition::SignalAll()
{
    WaitSetSet copyOfSet;
    {
        MutexLocker lock(&mutex_);
        copyOfSet = waitSet_;
    }

    // NB! WaitSet calls mutexed functions in Condition objects,
    // therefore, this should be a non-mutexed function
    AbstractCondition::signalAll(copyOfSet, this->GetPtr());
}

// -------------------------------------------------------
//	Thread safe implementation
// -------------------------------------------------------
bool AbstractCondition::Attach(WaitSet *waitSet)
{
    MutexLocker lock(&mutex_);
    return attach(waitSet);
}

// -------------------------------------------------------
//	Thread safe implementation
// -------------------------------------------------------
bool AbstractCondition::Detach(WaitSet *waitSet)
{
    MutexLocker lock(&mutex_);
    return detach(waitSet);
}

// -------------------------------------------------------
// Usage: When a Condition's trigger value is altered then
// 		call SignalAll to wakeup WaitSet
// -------------------------------------------------------
void AbstractCondition::signalAll(const WaitSetSet &waitSet, std::shared_ptr<AbstractCondition> self)
{
    // NB! WaitSet calls mutexed functions in Condition objects,
    // therefore, this should be a non-mutexed function
    for(WaitSetSet::const_iterator it = waitSet.begin(), it_end = waitSet.end(); it != it_end; ++it)
    {
        WaitSet *waitSet = *it;

        ASSERT(waitSet != NULL);

        waitSet->Signal(self);
    }
}

// -------------------------------------------------------
// Usage: When a Condition is attached to a WaitSet
// 		the WaitSet calls attachWaitSet(this)
// -------------------------------------------------------
bool AbstractCondition::attach(WaitSet *waitSet)
{
    std::pair<WaitSetSet::iterator, bool> pairIterator = waitSet_.insert(waitSet);

    if(pairIterator.second == false)
    {
        IWARNING() << "WaitSet already present!";
    }

    // -- debug element is inserted --
    ASSERT(pairIterator.second == true);
    // -- debug --

    return pairIterator.second;
}

// -------------------------------------------------------
// Usage: When a Condition is detached from a WaitSet
// 		the WaitSet calls detachWaitSet(this)
// -------------------------------------------------------
bool AbstractCondition::detach(WaitSet *waitSet)
{
    size_t erased = waitSet_.erase(waitSet);

    if(erased == 0)
    {
        IWARNING() << "WaitSet already deleted!";
    }

    // -- debug element is erased --
    ASSERT(erased == 1);
    // -- debug --

    return (erased == 1);
}

}}
