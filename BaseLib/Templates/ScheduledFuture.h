#pragma once

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Templates/FutureRunnable.h"

#include "BaseLib/Templates/MethodInterfaces.h"
#include "BaseLib/Templates/Lifecycle.h"

namespace BaseLib { namespace Templates
{

class ScheduledFutureCallbacks
    : public FutureCallbacks
{
public:
    virtual ~ScheduledFutureCallbacks() {}

    virtual void OnTimeoutDo(std::function<void ()>) = 0;
};

class ScheduledRunnableFuture
        : public RunnableFuture
        , public IsReadyMethod
        , public IsExecutingMethod
        , public IsTimeoutMethod
        , public TriggerMethod
        , public SuspendMethod
        , public ResumeMethod
        , public IsSuspendedMethod
        , public ReadyInMethod<Duration>
        , public TimeoutInMethod<Duration>
        , public DetachMethod<bool>
        , public IsAttachedMethod
{
public:
    virtual ~ScheduledRunnableFuture() {}

    CLASS_TRAITS(ScheduledRunnableFuture)
};

template <typename Policy>
class ScheduledRunnablePolicyFuture
    : public ScheduledRunnableFuture
    , public PolicyMethod<Policy>
{
public:
    virtual ~ScheduledRunnablePolicyFuture() {}

    CLASS_TRAITS(ScheduledRunnablePolicyFuture)
};


template <typename Return, typename Policy>
class ScheduledFutureResult
        : public ScheduledRunnablePolicyFuture<Policy>
        , public ResultMethod1<std::pair<Status::FutureStatus, Return>, int64>
{
public:
    virtual ~ScheduledFutureResult() {}

    CLASS_TRAITS(ScheduledFutureResult)
};

}}
