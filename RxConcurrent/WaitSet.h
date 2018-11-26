/*
 * WaitSet.h
 *
 *  Created on: Apr 12, 2012
 *      Author: knuthelv
 */
#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/Condition.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

/*----------------------------------------------------
 Base class for Condition implementations

    - Uses signal strategy -> Conditions use WaitSet->signal to wakeup any waiting threads
    - Uses function pointers
    - What context should Conditions hold?
    - How to know what a Condition means and to which object it relates to?
        -> attach an object in a condition as a template
        -> reimplement Dispatch, which will use this object to perform some action!
        -> This object is a function pointer, FunctorHolder.

Current issue:
    - No event ordering.
    - Possible starvation if I implement a DispatchOne() function!
    - Use a FIFO queue? WFQ? RWFQ?
    - Does a SignalNext() make sense?
----------------------------------------------------*/

class DLL_STATE WaitSet : public ENABLE_SHARED_FROM_THIS(WaitSet)
{
public:
    WaitSet();
    virtual ~WaitSet();

    CLASS_TRAITS(WaitSet)

    /**
     * @brief GetPtr Creates a shared pointer from this
     * @return
     */
    WaitSet::Ptr GetPtr();

    /**
     * Wakes waiting threads and returns.
     *
     * Typically used when destruction of object.
     */
    void Stop();

    /**
     * Sets state to valid.
     *
     * Typically used upon reinitialization of object.
     */
    void Start();

    /**
     * @brief WakeAll wakes-up all waiting thread.
     */
    void WakeAll() const;

    /**
     * @brief IsValid checks validity of WaitSet (after calling Stop WaitSet is invalid)
     *
     * @return true if valid
     */
    bool IsValid() const;

    /**
     * Waits for one of the attached condition to trigger or
     * for a timeout to expire. The attached conditions that
     * have been notified before the expiration of the timeout
     * are returned by the call.
     *
     * @param 	milliseconds the maximum amount of time for which the wait
     * should block while waiting for a condition to be triggered.
     *
     * @return	true if events to dispatch, false if no events to dispatch.
     */
    bool Wait(int64 milliseconds = LONG_MAX);

    /**
     * Waits for at least one of the attached conditions to  trigger and then
     * dispatches the events, or, times out and unblocks.
     *
     * @return 	true if dispatched events, false if no events dispatched.
     */
    bool WaitDispatch(int64 milliseconds = LONG_MAX);

    /**
     * Waits for at least one of the attached conditions to  trigger and then
     * dispatches the next queued event, or, times out and unblocks.
     *
     * @return 	true if dispatched events, false if no events dispatched.
     */
    bool WaitDispatchNext(int64 milliseconds = LONG_MAX);

    /**
     * Waits for at least one of the attached conditions to  trigger and then
     * returns the condition, or, times out and unblocks.
     *
     * @return condition if triggered, otherwise null pointer
     */
    AbstractCondition::Ptr WaitGetNext(int64 milliseconds = LONG_MAX);

    /**
     * Waits for the conditions to trigger and then
     * dispatches the events, or, times out and unblocks.
     *
     * @return true if dispatched events, false if no events dispatched.
     */
    bool WaitDispatch(AbstractCondition::Ptr condition, int64 milliseconds = LONG_MAX);

    /**
     *  Adds Condition to triggeredConditions_ and wakes up all threads
     *  waiting in Wait or WaitDispatch.
     */
    void Signal(AbstractCondition::Ptr condition);

public:
    void ClearTriggeredQueue();

    bool Attach(AbstractCondition::Ptr cond);
    bool Detach(AbstractCondition::Ptr cond);

    ConditionSet GetConditions() const;
    ConditionSet GetTriggeredConditions() const;

    bool DispatchTriggeredConditions();

public:
    static bool DispatchTriggeredCondition(AbstractCondition::Ptr condition);

private:
    bool                   enqueueTriggered(AbstractCondition::Ptr cond);
    AbstractCondition::Ptr dequeueTriggered();
    void                   clearTriggeredQueue();

private:
    bool isValid() const;
    bool waitForTriggeredConditions(int64 milliseconds);

private:
    static ConditionSet getTriggeredConditions(const ConditionSet& conditions);

private:
    ConditionSet conditions_;
    bool         valid_;

private:
    typedef std::list<AbstractCondition::Ptr> ConditionQueue;

    ConditionQueue triggeredQueue_;
    ConditionSet   triggeredConditions_;

private:
    mutable BaseLib::Mutex         mutex_;
    mutable BaseLib::WaitCondition waitCondition_;
};

}}
