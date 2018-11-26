#pragma once

#include"RxConcurrent/FutureFactory.h"
#include"RxConcurrent/ScheduledFutureTask.h"
#include"RxConcurrent/ThreadPool/ThreadPoolFactory.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE ComposerBase
{
public:
    virtual ~ComposerBase();
};

template <typename Return, typename Arg1>
class Composer1
    : public ComposerBase
      , public std::enable_shared_from_this<Composer1<Return, Arg1>>
{
public:
    Composer1(std::function<Return(Arg1)> function)
        : function_(std::make_shared<Templates::OperatorFunction1<Return, Arg1>>(function))
    { }

    Composer1(std::shared_ptr<Templates::OperatorFunction1<Return, Arg1>> function)
        : function_(function)
    { }

    virtual ~Composer1()
    { }

    template <typename Return1>
    std::shared_ptr<Composer1<Return1, Arg1>> Then(std::function<Return1(Return)> after)
    {
        auto newComposer = std::make_shared<Composer1<Return1, Arg1>>(
            function_->After(after)
        );

        composers_.push_back(newComposer);
        return newComposer;
    }

    template <typename Arg2>
    std::shared_ptr<Composer1<Arg1, Arg2>> Before(std::function<Arg1(Arg2)> before)
    {
        auto newComposer = std::make_shared<Composer1<Arg1, Arg2>>(
            function_->Before(before)
        );

        composers_.push_back(newComposer);
        return newComposer;
    }

    ScheduledFutureTask<Return> Execute(Arg1 arg1, TaskPolicy policy = TaskPolicy::RunOnceImmediately())
    {
        return ThreadPoolFactory::Instance().GetDefault()->Schedule<Return>(
            FutureFactory::Create<Return, Arg1>(function_, arg1),
            policy
        );
    }

private:
    std::list<std::shared_ptr<ComposerBase>>                    composers_;
    std::shared_ptr<Templates::OperatorFunction1<Return, Arg1>> function_;
};

}}
