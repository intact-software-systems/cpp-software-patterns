#pragma once

#include"RxConcurrent/CommonDefines.h"

namespace BaseLib { namespace Concurrent {

template <typename Return>
class FutureTask
    : public Templates::FutureResult<Return>
      , public std::enable_shared_from_this<FutureTask<Return>>
{
public:
    FutureTask(typename Templates::FutureResult<Return>::Ptr action)
        : action_(action)
    { }

    virtual ~FutureTask()
    { }

    CLASS_TRAITS(FutureTask)

    typename FutureTask<Return>::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual void run()
    {
        action_->run();
    }

    virtual std::string GetName() const override
    {
        return action_->GetName();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsCancelled() const
    {
        return action_->IsCancelled();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

    virtual bool Interrupt()
    {
        return action_->Interrupt();
    }

    virtual bool IsInterrupted() const
    {
        return action_->IsInterrupted();
    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        return action_->WaitFor(msecs);
    }

    virtual typename Templates::FutureResult<Return>::ResultHolder Result(int64 msecs = LONG_MAX) const
    {
        return action_->Result(msecs);
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual const Status::ExecutionStatus& StatusConst() const
    {
        return action_->StatusConst();
    }

    virtual Status::ExecutionStatus& Status()
    {
        return action_->Status();
    }

    virtual void OnErrorDo(std::function<void(GeneralException)> errorFunc)
    {
        action_->OnErrorDo(errorFunc);
    }

    virtual void OnNextDo(std::function<void(Return ret)> nextFunc)
    {
        action_->OnNextDo(nextFunc);
    }

    virtual void OnCompleteDo(std::function<void()> completeFunc)
    {
        action_->OnCompleteDo(completeFunc);
    }

    virtual void FinallyDo(std::function<void()> finallyFunc)
    {
        action_->FinallyDo(finallyFunc);
    }

    bool operator==(const FutureTask<Return>& rvalue) const
    {
        return this->action_.get() == rvalue.action_.get();
    }

private:
    typename Templates::FutureResult<Return>::Ptr action_;
};

}}
