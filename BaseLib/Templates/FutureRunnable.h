#pragma once

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Runnable.h"

#include "BaseLib/Status/ExecutionStatus.h"
#include "BaseLib/Status/FutureStatus.h"

#include "BaseLib/Templates/ActionHolder.h"
#include "BaseLib/Export.h"

namespace BaseLib { namespace Templates {

class FutureCallbacks
{
public:
    virtual ~FutureCallbacks() {}

    virtual void OnErrorDo(std::function<void (GeneralException)> errorFunc) = 0;
    virtual void OnCompleteDo(std::function<void ()> completeFunc) = 0;
    virtual void FinallyDo(std::function<void ()> finallyFunc) = 0;
};

template <typename Return>
class FutureCallbackResult
    : public FutureCallbacks
{
public:
    virtual ~FutureCallbackResult() {}

    virtual void OnNextDo(std::function<void (Return ret)> nextFunc) = 0;
};

class FutureBase
        : public Action
        , public IsCancelledMethod
        , public WaitForMethod1<bool, int64>
{
public:
    virtual ~FutureBase() {}

    CLASS_TRAITS(FutureBase)
};

class RunnableFuture
        : public Runnable
        , public FutureBase
        , public InterruptMethod
        , public IsInterruptedMethod
        , public StatusMethod<Status::ExecutionStatus>
        , public StatusConstMethod<Status::ExecutionStatus>
{
public:
    virtual ~RunnableFuture() {}

    CLASS_TRAITS(RunnableFuture)
};

template <typename Return>
class FutureResult
        : public RunnableFuture
        , public ResultMethod1<std::pair<Status::FutureStatus, Return>, int64>
        , public FutureCallbackResult<Return>
{
public:
    virtual ~FutureResult() {}

    CLASS_TRAITS(FutureResult)

    typedef std::pair<Status::FutureStatus, Return> ResultHolder;
};

}}
