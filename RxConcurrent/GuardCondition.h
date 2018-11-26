#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/Condition.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

/*----------------------------------------------------
 GuardCondition implementations - thread safe

 TODO: Ability to cancel/interrupt a callback to functor
    - Use FutureTask
----------------------------------------------------*/
class DLL_STATE GuardCondition : public AbstractCondition
{
public:
    // -------------------------------------------------------
    // Basic constructor with default triggerValue == false
    // -------------------------------------------------------
    GuardCondition()
        : AbstractCondition()
        , triggerValue_(false)
        , functorHolder_(NULL)
    { }

    // -------------------------------------------------------
    // Sets Functor and argument to be invoked (if any)
    // Default triggerValue == false
    // -------------------------------------------------------
    template <typename Functor>
    GuardCondition(const Functor& func);

    template <typename Functor>
    GuardCondition(Functor* func);

    template <typename Functor, typename ARG>
    GuardCondition(const Functor& func, const ARG& arg);

    template <typename Functor, typename ARG>
    GuardCondition(Functor* func, const ARG& arg);

    // -------------------------------------------------------
    // Destructs object, deleting attached functor (if any)
    // -------------------------------------------------------
    virtual ~GuardCondition();

    CLASS_TRAITS(GuardCondition)

    // -------------------------------------------------------
    // Dispatch (invoke) attached functor (if any)
    // -------------------------------------------------------
    virtual void Dispatch();

    // -------------------------------------------------------
    // Sets Functor and argument to be invoked (if any)
    // -------------------------------------------------------
    template <typename Functor>
    void SetHandler(const Functor& func);

    template <typename Functor>
    void SetHandler(Functor* func);

    template <typename Functor, typename ARG>
    void SetHandler(const Functor& func, const ARG& arg);

    template <typename Functor, typename ARG>
    void SetHandler(Functor* func, const ARG& arg);

    // -------------------------------------------------------
    // Resets and deletes existing attached functor
    // -------------------------------------------------------
    void ResetHandler();

    // -------------------------------------------------------
    // Checks if GuardCondition has a Functor attached
    // -------------------------------------------------------
    bool HasHandler() const;

    // -------------------------------------------------------
    // Usage: Get the triggerValue
    // -------------------------------------------------------
    virtual bool GetTriggerValue() const;

    // -------------------------------------------------------
    // Usage: Set the triggerValue and signals attached WaitSet
    //		(if any)
    // -------------------------------------------------------
    virtual void SetTriggerValue(bool triggerValue);

private:
    bool                        triggerValue_;

    // TODO: Replace with Future with cancel/interrupt support?
    Templates::Functor0*	functorHolder_;

private:
    GuardCondition(const GuardCondition &);
    GuardCondition& operator=(const GuardCondition&);
};


/*----------------------------------------------------
 GuardCondition template implementations Constructor
----------------------------------------------------*/
template <typename Functor>
GuardCondition::GuardCondition(const Functor& func)
    : AbstractCondition()
    , triggerValue_(false)
    , functorHolder_(new Templates::FunctorInvoker0<Functor>(func))
{ }

template <typename Functor>
GuardCondition::GuardCondition(Functor* func)
    : AbstractCondition()
    , triggerValue_(false)
    , functorHolder_(new Templates::FunctorPtrInvoker0<Functor>(func))
{ }

template <typename Functor, typename ARG>
GuardCondition::GuardCondition(const Functor& func, const ARG& arg)
    : AbstractCondition()
    , triggerValue_(false)
    , functorHolder_(new Templates::FunctorInvoker1<Functor, ARG>(func, arg))
{ }

template <typename Functor, typename ARG>
GuardCondition::GuardCondition(Functor* func, const ARG &arg)
    : AbstractCondition()
    , triggerValue_(false)
    , functorHolder_(new Templates::FunctorPtrInvoker1<Functor, ARG>(func, arg))
{ }

/*----------------------------------------------------
 GuardCondition template implementations SetHandler
----------------------------------------------------*/

template <typename Functor>
void GuardCondition::SetHandler(const Functor& func)
{
    MutexLocker lock(&mutex_);

    Templates::Functor0* tmp = functorHolder_;
    functorHolder_ = new Templates::FunctorInvoker0<Functor>(func);

    if(tmp != NULL)
        delete tmp;
}

template <typename Functor>
void GuardCondition::SetHandler(Functor* func)
{
    MutexLocker lock(&mutex_);

    Templates::Functor0* tmp = functorHolder_;
    functorHolder_ = new Templates::FunctorPtrInvoker0<Functor>(func);

    if(tmp != NULL)
        delete tmp;
}

template <typename Functor, typename ARG>
void GuardCondition::SetHandler(const Functor& func, const ARG &arg)
{
    MutexLocker lock(&mutex_);

    Templates::Functor0* tmp = functorHolder_;
    functorHolder_ = new Templates::FunctorInvoker1<Functor, ARG>(func, arg);

    if(tmp != NULL)
        delete tmp;
}


template <typename Functor, typename ARG>
void GuardCondition::SetHandler(Functor* func, const ARG &arg)
{
    MutexLocker lock(&mutex_);

    Templates::Functor0* tmp = functorHolder_;
    functorHolder_ = new Templates::FunctorPtrInvoker1<Functor, ARG>(func, arg);

    if(tmp != NULL)
        delete tmp;
}

}}

