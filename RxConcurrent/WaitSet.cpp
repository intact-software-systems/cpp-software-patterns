/*
 * WaitSet.cpp
 *
 *  Created on: Apr 12, 2012
 *      Author: knuthelv
 */
#include "RxConcurrent/WaitSet.h"

namespace BaseLib { namespace Concurrent {
/*----------------------------------------------------
 Base class for Condition implementations
----------------------------------------------------*/
WaitSet::WaitSet()
    : valid_(true)
{ }

/**
 * TODO: Potential deadlock in Stop()?
 */
WaitSet::~WaitSet()
{
    Stop();
}

WaitSet::Ptr WaitSet::GetPtr()
{
    return shared_from_this();
}

/*----------------------------------------------------
  avoid threads waiting forever
----------------------------------------------------*/
void WaitSet::Stop()
{
    MutexLocker lock(&mutex_);

    valid_ = false;

    waitCondition_.wakeAll();
}

void WaitSet::Start()
{
    MutexLocker lock(&mutex_);

    valid_ = true;

    waitCondition_.wakeAll();
}

void WaitSet::WakeAll() const
{
    MutexLocker lock(&mutex_);
    waitCondition_.wakeAll();
}

bool WaitSet::IsValid() const
{
    MutexLocker lock(&mutex_);
    return isValid();
}

bool WaitSet::isValid() const
{
    return valid_;
}

/*----------------------------------------------------
    Wait for triggered conditions

    - NB! Must be used with GetTriggeredConditions()
      to retrieve the triggered conditions.
----------------------------------------------------*/
bool WaitSet::Wait(int64 milliseconds)
{
    MutexLocker lock(&mutex_);

    if(!isValid()) return false;

    return waitForTriggeredConditions(milliseconds);
}

/*----------------------------------------------------
    Wait for triggered conditions and Dispatch when woken up.
    The dispatch uses template typename Functor (function pointer/holder)
    and calls operator()() in class.
----------------------------------------------------*/
bool WaitSet::WaitDispatch(int64 milliseconds)
{
    {
        MutexLocker lock(&mutex_);

        if(!isValid()) return false;

        waitForTriggeredConditions(milliseconds);
    }

    return DispatchTriggeredConditions();
}

bool WaitSet::WaitDispatchNext(int64 milliseconds)
{
    AbstractCondition::Ptr cond;

    {
        MutexLocker lock(&mutex_);

        if(!isValid()) return false;

        waitForTriggeredConditions(milliseconds);

        cond = dequeueTriggered();
    }

    return WaitSet::DispatchTriggeredCondition(cond);
}

AbstractCondition::Ptr WaitSet::WaitGetNext(int64 milliseconds)
{
    ElapsedTimer timer(milliseconds);

    ASSERT(!timer.HasExpired());

    while(true)
    {
        MutexLocker lock(&mutex_);

        waitForTriggeredConditions(timer.Remaining().InMillis());

        AbstractCondition::Ptr cond = dequeueTriggered();
        if(cond != nullptr)
        {
            return cond->GetPtr();
        }

        if(timer.HasExpired()) break;
        if(!isValid()) break;
    }

    return AbstractCondition::Ptr();
}

bool WaitSet::WaitDispatch(AbstractCondition::Ptr condition, int64 milliseconds)
{
    // Is valid?
    {
        MutexLocker lock(&mutex_);
        if(!isValid()) return false;
    }

    // Wait for triggered condition
    ElapsedTimer timer(milliseconds);

    while(!condition->GetTriggerValue() && !timer.HasExpired())
    {
        MutexLocker lock(&mutex_);

        bool isWoken = waitCondition_.wait(&mutex_, timer.Remaining().InMillis());
        if(isWoken)
        {
            break;
        }
        else if(!isValid())
        {
            break;
        }
    }

    return WaitSet::DispatchTriggeredCondition(condition);
}

/*----------------------------------------------------
    Signal thread waiting in Wait or WaitDispatch

    Example usage: GuardCondition->SetTriggerValue()
        calls WaitSet::Signal if it is attached
----------------------------------------------------*/
void WaitSet::Signal(AbstractCondition::Ptr condition)
{
    MutexLocker lock(&mutex_);

    if(!isValid()) return;

    ConditionSet::iterator it = conditions_.find(condition);

    // -- insert --
    if(it != conditions_.end())
    {
        enqueueTriggered(condition);

        waitCondition_.wakeOne();

        // -- debug --
        // It is possible that a condition is triggered twice before WaitSet thread gets to run!
        // ASSERT(inserted.second == true);
        // -- debug --
    }
    else // condition not attached!
    {
        // -- debug --
        ASSERT(it != conditions_.end());
        // -- debug --
    }
}

/*----------------------------------------------------
- Attach Condition to this WaitSet.
- Behind the scenes attach this Waitset to the Condition to
    allow it to wakeup this WaitSet upon trigger.
----------------------------------------------------*/
bool WaitSet::Attach(AbstractCondition::Ptr condition)
{
    bool isAttached = true;

    // -----------------------------------
    // attach wait-set to condition
    // -----------------------------------
    {
        isAttached = condition->Attach(this);
        if(!isAttached)
        {
            // -- debug --
            IWARNING() << "Already attached";
            // -- debug --

            return false;
        }
    }

    bool isTriggeredCondition = condition->GetTriggerValue();

    // -----------------------------------
    // insert condition to set
    // -----------------------------------
    {
        MutexLocker lock(&mutex_);

        std::pair<ConditionSet::iterator, bool> inserted = conditions_.insert(condition->GetPtr());

        // -- debug --
        ASSERT(inserted.second);
        // -- debug --

        if(isTriggeredCondition)
        {
            enqueueTriggered(condition);

            waitCondition_.wakeOne();
        }
    }

    // -- debug --
    ASSERT(isAttached);
    // -- debug --

    return isAttached;
}

/*----------------------------------------------------
- Detach the Condition from the WaitSet
----------------------------------------------------*/
bool WaitSet::Detach(AbstractCondition::Ptr condition)
{
    // ---------------------------
    // Erase condition from set
    // ---------------------------
    {
        MutexLocker lock(&mutex_);

        conditions_.erase(condition);

        triggeredConditions_.erase(condition);
        triggeredQueue_.remove(condition);
    }

    condition->SetTriggerValue(false);

    bool isDetached = condition->Detach(this);
    if(!isDetached)
    {
        // -- debug --
        IWARNING() << "Already detached";
        // -- debug --

        return false;
    }

    return isDetached;
}

/*----------------------------------------------------
 Return a copy of attached conditions
----------------------------------------------------*/
ConditionSet WaitSet::GetConditions() const
{
    MutexLocker lock(&mutex_);

    return conditions_;
}

/*----------------------------------------------------
 Return all triggered conditions
----------------------------------------------------*/
ConditionSet WaitSet::GetTriggeredConditions() const
{
    ConditionSet conditions;
    {
        MutexLocker lock(&mutex_);
        conditions = conditions_;
    }

    return WaitSet::getTriggeredConditions(conditions);
}

/*----------------------------------------------------
 Dispatch all triggered conditions
----------------------------------------------------*/
bool WaitSet::DispatchTriggeredConditions()
{
    bool dispatchedCondition = false;

    while(true)
    {
        AbstractCondition::Ptr condition;

        {
            MutexLocker lock(&mutex_);
            condition = dequeueTriggered();
        }

        if(!condition) break;

        dispatchedCondition = WaitSet::DispatchTriggeredCondition(condition);
    }

    return dispatchedCondition;
}

/*----------------------------------------------------
 Clears the queue of triggered conditions
----------------------------------------------------*/
void WaitSet::ClearTriggeredQueue()
{
    MutexLocker lock(&mutex_);
    clearTriggeredQueue();
}

/*----------------------------------------------------
private functions that are not thread-safe
----------------------------------------------------*/
bool WaitSet::enqueueTriggered(AbstractCondition::Ptr cond)
{
    bool isEnqueued = false;

    // ----------------------------------------
    // If already triggered then return false
    // ------------------------------------------
    if(triggeredConditions_.find(cond) != triggeredConditions_.end())
    {
        isEnqueued = false;
    }
    else
    {
        triggeredQueue_.push_back(cond->GetPtr());
        triggeredConditions_.insert(cond->GetPtr());

        isEnqueued = true;
    }

    // -- debug --
    ASSERT(triggeredQueue_.size() <= conditions_.size());
    ASSERT(triggeredQueue_.size() == triggeredConditions_.size());
    // -- debug --

    return isEnqueued;
}

/**
 * @brief WaitSet::dequeueTriggered returns a triggered condition.
 *
 * Note! It is allowed that a trigger value may change, but no need checking.
 *
 * NB! It is possible that the condition was triggered and then un-triggered,
 * therefore, the implementation should handle these cases.
 *
 * NB! Calling cond->GetTriggerValue() may cause a deadlock between condition
 * and WaitSet so avoid calling any mutexed function on condition inside mutexed function in WaitSet!
 *
 * @return
 */
AbstractCondition::Ptr WaitSet::dequeueTriggered()
{
    AbstractCondition::Ptr cond;

    while(!triggeredQueue_.empty())
    {
        cond = triggeredQueue_.front();

        triggeredQueue_.pop_front();
        triggeredConditions_.erase(cond);

        if(!cond)
        {
            // -- debug --
            ICRITICAL() << "Condition was NULL!";
            ASSERT(cond);
            // -- debug --

            continue;
        }

        break;
    }

    // -- debug --
    ASSERT(triggeredQueue_.size() == triggeredConditions_.size());
    // -- debug --

    return cond;
}

void WaitSet::clearTriggeredQueue()
{
    triggeredQueue_.clear();
    triggeredConditions_.clear();
}

bool WaitSet::waitForTriggeredConditions(int64 milliseconds)
{
    ElapsedTimer timer(milliseconds);

    // ---------------------------------------------
    // 	wait if no triggered conditions
    // 	wait if no conditions at all
    // ---------------------------------------------
    while(triggeredQueue_.empty() && !timer.HasExpired())
    {
        bool isWoken = waitCondition_.wait(&mutex_, timer.Remaining().InMillis());
        if(isWoken)
        {
            break;
        }
        else if(!isValid())
        {
            break;
        }
    }

    return !triggeredQueue_.empty();
}

bool WaitSet::DispatchTriggeredCondition(AbstractCondition::Ptr condition)
{
    if(!condition) return false;

    if(condition->GetTriggerValue())
    {
        condition->Dispatch();  // resets trigger to false!
        return true;
    }

    return false;
}

ConditionSet WaitSet::getTriggeredConditions(const ConditionSet& conditions)
{
    ConditionSet triggeredConditions;

    std::pair<ConditionSet::iterator, bool> inserted;

    for(ConditionSet::const_iterator it = conditions.begin(), it_end = conditions.end(); it != it_end; ++it)
    {
        AbstractCondition::Ptr condition = *it;

        // -- debug --
        ASSERT(condition);
        // -- debug --

        if(condition->GetTriggerValue())
        {
            inserted = triggeredConditions.insert(condition->GetPtr());

            // -- debug --
            ASSERT(inserted.second);
            // -- debug --
        }
    }

    return triggeredConditions;
}

}}

