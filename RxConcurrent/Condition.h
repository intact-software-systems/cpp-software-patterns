/*
 * Condition.h
 *
 *  Created on: Apr 12, 2012
 *      Author: knuthelv
 */
#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

/*----------------------------------------------------
    typedefs and pre-declarations
    TODO: Move to CommonDefines.h
----------------------------------------------------*/
class WaitSet;

typedef std::set<WaitSet*> WaitSetSet;

/*----------------------------------------------------
 Base class for Condition implementations

 TODO:
 virtual void Trigger() = 0;
 virtual void Untrigger() = 0;
 virtual bool IsTriggered() const = 0;

 Is it possible to have other trigger values than boolean?
----------------------------------------------------*/
class DLL_STATE AbstractCondition : public Templates::NoCopy<AbstractCondition>, public std::enable_shared_from_this<AbstractCondition>
{
public:
    virtual ~AbstractCondition();

    CLASS_TRAITS(AbstractCondition)

    virtual bool GetTriggerValue() const = 0;

    virtual void SetTriggerValue(bool val) = 0;

    virtual void Dispatch() = 0;

public:
    // -------------------------------------------------------
    // Usage: Creates a shared pointer from this
    // -------------------------------------------------------
    AbstractCondition::Ptr GetPtr();

    // -------------------------------------------------------
    // Usage: When a Condition's trigger value is altered then
    // 		call SignalAll to wakeup WaitSet
    // -------------------------------------------------------
    void SignalAll();

    // -------------------------------------------------------
    // Usage: When a Condition is attached to a WaitSet
    // 		the WaitSet calls attach(this)
    // -------------------------------------------------------
    bool Attach(WaitSet *waitSet);

    // -------------------------------------------------------
    // Usage: When a Condition is detached from a WaitSet
    // 		the WaitSet calls detach(this)
    // -------------------------------------------------------
    bool Detach(WaitSet *waitSet);

protected:
    // -------------------------------------------------------
    // thread-safe implementations
    // -------------------------------------------------------
    static void 	signalAll(const WaitSetSet &waitSet, std::shared_ptr<AbstractCondition> self);

protected:
    // -------------------------------------------------------
    // non-thread-safe implementations
    // -------------------------------------------------------
    bool 	attach(WaitSet *waitSet);
    bool 	detach(WaitSet *waitSet);

protected:
    mutable BaseLib::Mutex	mutex_;

private:
    WaitSetSet		waitSet_;
};

/*----------------------------------------------------
 Various: typedefs
----------------------------------------------------*/
typedef std::set<std::shared_ptr<AbstractCondition>>	ConditionSet;

}}
