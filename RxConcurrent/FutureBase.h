#pragma once

#include"RxConcurrent/CommonDefines.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class Future
    : public Templates::FutureResult<T>
{
public:
    Future(typename Templates::FutureResult<T>::Ptr future)
        : future_(future)
    { }

    virtual ~Future()
    { }

    CLASS_TRAITS(Future)

    virtual bool IsSuccess() const
    {
        return future_->IsSuccess();
    }

    virtual bool Cancel()
    {
        return future_->Cancel();
    }

    virtual bool Interrupt()
    {
        return future_->Interrupt();
    }

    virtual bool IsInterrupted() const
    {
        return future_->IsInterrupted();
    }

    virtual bool IsCancelled() const
    {
        return future_->IsCancelled();
    }

    virtual bool IsDone() const
    {
        return future_->IsDone();
    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        return future_->WaitFor(msecs);
    }

    virtual typename Templates::FutureResult<T>::ResultHolder Result(int64 msecs = LONG_MAX) const
    {
        return future_->Result(msecs);
    }

    virtual const Status::ExecutionStatus& StatusConst() const
    {
        return future_->StatusConst();
    }

    virtual Status::ExecutionStatus& Status()
    {
        return future_->Status();
    }

    virtual void OnErrorDo(std::function<void(GeneralException)> errorFunc)
    {
        future_->OnErrorDo(errorFunc);
    }

    virtual void OnNextDo(std::function<void(T ret)> nextFunc)
    {
        future_->OnNextDo(nextFunc);
    }

    virtual void OnCompleteDo(std::function<void()> completeFunc)
    {
        future_->OnCompleteDo(completeFunc);
    }

    virtual void FinallyDo(std::function<void()> finallyFunc)
    {
        future_->FinallyDo(finallyFunc);
    }

    virtual std::string GetName() const override
    {
        return future_->GetName();
    }

private:
    virtual void run()
    {
        future_->run();
    }

private:
    typename Templates::FutureResult<T>::Ptr future_;
};

// --------------------------------------------------------
// Support callbacks to AsyncComplectionController from finished futures
// - Use RxObserverNew<Return> (to support callbacks from async RxTransport, to future to commands)
// Add support for callbacks in futures upon completion
// - class ObservableFuture with support for adding std::function
// Refactor Signaller implementations to accept std::function
// --------------------------------------------------------

template <typename Return>
class FutureInvokerBase
    : public Templates::Notifyable<FutureInvokerBase<Return>, WaitCondition, Mutex>
      , public Templates::FutureResult<Return>
      , public Templates::ReactorMethods<bool, Return, BaseLib::GeneralException>
{
protected:
    typedef MutexTypeLocker<FutureInvokerBase<Return> > Locker;

public:
    FutureInvokerBase()
        : exception_("")
        , result_()
        , status_()
        , currentThread_(nullptr)
    { }

    virtual ~FutureInvokerBase() throw()
    { }

    CLASS_TRAITS(FutureInvokerBase)

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        Locker lock(this);
        return this->waitForSuccess(this, msecs);
    }

    virtual typename Templates::FutureResult<Return>::ResultHolder Result(int64 msecs = LONG_MAX) const
    {
        Locker lock(this);

        bool isWoken = this->waitForSuccess(this, msecs);

        if(status_.IsSuccess())
        {
            return typename Templates::FutureResult<Return>::ResultHolder(Status::FutureStatus::Ready(), result_);
        }
        else if(!status_.IsEverStarted())
        {
            return typename Templates::FutureResult<Return>::ResultHolder(Status::FutureStatus::Deferred(), Return());
        }
        else if(isWoken && !status_.IsSuccess())
        {
            return typename Templates::FutureResult<Return>::ResultHolder(Status::FutureStatus::Failed(exception_), Return());
        }

        return typename Templates::FutureResult<Return>::ResultHolder(Status::FutureStatus::Timeout(), Return());
    }

    virtual bool IsCancelled() const
    {
        return status_.IsCancelled();
    }

    virtual bool IsDone() const
    {
        return !status_.IsExecuting() && (status_.IsLastExecutionSuccess() || status_.IsLastExecutionFailure());
    }

    virtual bool IsSuccess() const
    {
        return status_.IsLastExecutionSuccess();
    }

    virtual bool Interrupt()
    {
        Locker lock(this);
        if(currentThread_ == nullptr)
        {
            return false;
        }

        currentThread_->interrupt();
        return status_.Interrupt();
    }

    virtual bool IsInterrupted() const
    {
        return status_.IsInterrupted();
    }

    virtual bool Cancel()
    {
        Locker lock(this);
        if(currentThread_ != nullptr)
        {
            currentThread_->interrupt();
        }

        return status_.IsCancelled()
               ? true
               : status_.Cancel();
    }

    virtual void Entry()
    {
        Locker lock(this);
        currentThread_ = Thread::currentThread();
        status_.Start();
    }

    virtual void Exit()
    {
        Locker lock(this);
        currentThread_ = nullptr;
    }

    virtual const Status::ExecutionStatus& StatusConst() const
    {
        return status_;
    }

    virtual Status::ExecutionStatus& Status()
    {
        return status_;
    }

//    virtual bool Apply(Return val)
//    {
//        return false;
//    }

//    template <typename Return1>
//    void Compose(std::function<Return1 (FutureInvokerBase*)> func)
//    {
//        //return std::make_shared<OperatorFunction1<Return1, Arg1>>(composer);
//    }

    // --------------------------------------------------------
    // Set callbacks when completed
    // --------------------------------------------------------

    virtual void OnErrorDo(std::function<void(GeneralException)> errorFunc)
    {
        MutexLocker lock(&callbackMutex_);
        errorFunc_ = errorFunc;
    }

    virtual void OnNextDo(std::function<void(Return ret)> nextFunc)
    {
        MutexLocker lock(&callbackMutex_);
        nextFunc_ = nextFunc;
    }

    virtual void OnCompleteDo(std::function<void()> completeFunc)
    {
        MutexLocker lock(&callbackMutex_);
        completeFunc_ = completeFunc;
    }

    virtual void FinallyDo(std::function<void()> finallyFunc)
    {
        MutexLocker lock(&callbackMutex_);
        finallyFunc_ = finallyFunc;
    }

    // --------------------------------------------------------
    // Interface ReactiveMethods
    // --------------------------------------------------------

    virtual bool Error(GeneralException t)
    {
        {
            Locker lock(this);
            status_.Failure();
            exception_ = t;
        }

        MutexLocker lock(&callbackMutex_);
        if(errorFunc_)
        {
            errorFunc_(t);
        }

        if(finallyFunc_)
        {
            finallyFunc_();
        }

        return true;
    }

    virtual bool Complete()
    {
        {
            Locker lock(this);
            status_.Success();
        }

        MutexLocker lock(&callbackMutex_);
        if(completeFunc_)
        {
            completeFunc_();
        }

        if(finallyFunc_)
        {
            finallyFunc_();
        }

        return true;
    }

    virtual bool Next(Return t)
    {
        {
            Locker lock(this);
            result_ = t;
        }

        MutexLocker lock(&callbackMutex_);
        if(nextFunc_)
        {
            nextFunc_(t);
        }

        return true;
    }

protected:
    GeneralException        exception_;
    Return                  result_;
    Status::ExecutionStatus status_;
    Thread                                * currentThread_;

    std::function<void(GeneralException)> errorFunc_    = [](GeneralException) { };
    std::function<void(Return ret)>       nextFunc_     = [](Return) { };
    std::function<void()>                 completeFunc_ = [] { };
    std::function<void()>                 finallyFunc_  = [] { };

    Mutex callbackMutex_;
};

template <typename Return>
class FutureInvoker
    : public FutureInvokerBase<Return>
      , public Templates::InvokeMethod0<Return>
{
public:
    virtual ~FutureInvoker()
    { }

    virtual void run()
    {
        this->result_ = Strategy::RunnableFutureExecution<Return, FutureInvoker>::Perform(*this);
    }
};

template <typename Return>
class FutureConstInvoker
    : public FutureInvokerBase<Return>
      , public Templates::InvokeConstMethod0<Return>
{
public:
    virtual ~FutureConstInvoker()
    { }

    virtual void run()
    {
        this->result_ = Strategy::RunnableFutureExecution<Return, FutureConstInvoker>::Perform(*this);
    }
};

template <typename Return, typename Invoker>
class RunnableInvoker
    : public FutureInvoker<Return>
{
public:
    RunnableInvoker(Invoker invoker)
        : invoker_(invoker)
    { }

    virtual ~RunnableInvoker()
    { }

    CLASS_TRAITS(RunnableInvoker)

    virtual Return Invoke()
    {
        return invoker_.Invoke();
    }

    virtual std::string GetName() const override
    {
        return Utility::GetTypeName<Invoker>();
    }

public:
    Invoker invoker_;
};

template <typename T>
class RunnableCaller : public FutureInvoker<bool>
{
public:
    RunnableCaller(T runnable)
        : runnable_(runnable)
    { }

    virtual ~RunnableCaller() throw()
    { }

    CLASS_TRAITS(RunnableCaller)

    virtual bool Invoke()
    {
        runnable_->run();
        return true;
    }

private:
    T runnable_;
};

template <typename Return, typename Strategy>
class RunnableStrategyInvoker
    : public FutureInvoker<Return>
{
public:
    RunnableStrategyInvoker(Strategy strategy)
        : strategy_(strategy)
    { }

    virtual ~RunnableStrategyInvoker()
    { }

    CLASS_TRAITS(RunnableStrategyInvoker)

    virtual Return Invoke()
    {
        return strategy_->Perform();
    }

    virtual std::string GetName() const override
    {
        return Utility::GetTypeName<Strategy>();
    }

private:
    Strategy strategy_;
};

template <typename Return, typename Strategy>
class RunnableStrategyConstInvoker
    : public FutureConstInvoker<Return>
{
public:
    RunnableStrategyConstInvoker(Strategy strategy)
        : strategy_(strategy)
    { }

    virtual ~RunnableStrategyConstInvoker()
    { }

    CLASS_TRAITS(RunnableStrategyConstInvoker)

    virtual Return Invoke() const
    {
        return strategy_->Perform();
    }

    virtual std::string GetName() const override
    {
        return Utility::GetTypeName<Strategy>();
    }

private:
    const Strategy strategy_;
};

template <typename Return, typename Function>
class RunnableFunctionInvoker
    : public FutureInvoker<Return>
{
public:
    RunnableFunctionInvoker(Function strategy)
        : strategy_(strategy)
    { }

    virtual ~RunnableFunctionInvoker()
    { }

    CLASS_TRAITS(RunnableFunctionInvoker)

    virtual Return Invoke()
    {
        return strategy_->operator()();
    }

    virtual std::string GetName() const override
    {
        return Utility::GetTypeName<Function>();
    }

private:
    Function strategy_;
};

template <typename Return, typename Function>
class RunnableFunctionConstInvoker
    : public FutureInvoker<Return>
{
public:
    RunnableFunctionConstInvoker(Function strategy)
        : strategy_(strategy)
    { }

    virtual ~RunnableFunctionConstInvoker()
    { }

    CLASS_TRAITS(RunnableFunctionConstInvoker)

    virtual Return Invoke() const
    {
        return strategy_->operator()();
    }

    virtual std::string GetName() const override
    {
        return Utility::GetTypeName<Function>();
    }

private:
    const Function strategy_;
};

template <typename Return, typename Invoker>
class RunnableActionInvoker
    : public FutureInvoker<Return>
{
public:
    RunnableActionInvoker(Invoker invoker)
        : invoker_(invoker)
    { }

    virtual ~RunnableActionInvoker()
    { }

    CLASS_TRAITS(RunnableActionInvoker)

    virtual Return Invoke()
    {
        return invoker_->Invoke();
    }

    virtual bool Cancel()
    {
        return invoker_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return invoker_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return invoker_->IsDone();
    }

    virtual void Entry()
    {
        FutureInvoker<Return>::Entry();
        invoker_->Entry();
    }

    virtual void Exit()
    {
        invoker_->Exit();
        FutureInvoker<Return>::Exit();
    }

    virtual std::string GetName() const override
    {
        return Utility::GetTypeName<Invoker>();
    }

private:
    Invoker invoker_;
};

}}

