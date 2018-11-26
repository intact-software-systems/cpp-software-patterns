#pragma once

#include"RxConcurrent/CommonDefines.h"

namespace BaseLib { namespace Concurrent
{

template <typename V>
class FlowController
        : public Templates::Action0<bool>
        , public Templates::ResumeMethod
        , public Templates::IsResumedMethod
        , public Templates::SuspendMethod
        , public Templates::TriggerMethod
        , public Templates::IsSuspendedMethod
        , public Templates::IsEmptyMethod
        , public Templates::InitializeMethods
        , public Templates::ReactorMethods<bool, V, BaseLib::Exception>
        , public ENABLE_SHARED_FROM_THIS_T1(FlowController, V)
{
public:
    virtual ~FlowController() {}

    CLASS_TRAITS(FlowController)

    typename FlowController<V>::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual std::list<V> Pull() = 0;
    virtual std::list<V> History() const = 0;

    virtual bool Set(Templates::Action0<bool>::Ptr action) = 0;

    virtual const FlowControllerStatus& Status() const = 0;

    virtual Duration NextSchedule(const Status::ExecutionStatus &status, const TaskPolicy &policy) = 0;
};

}}
