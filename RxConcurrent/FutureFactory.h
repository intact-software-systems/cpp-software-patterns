#pragma once

#include"RxConcurrent/CommonDefines.h"

#include"RxConcurrent/FutureTask.h"
#include"RxConcurrent/FutureBase.h"

#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

/**
 *- Separate execution of task and holder.
 *  Make uniform way of executing
 *  - Runnable
 *  - Callable
 *  - Strategies
 *  - Function pointers
 * through decorators.
 *
 * Make function pointer decorator. Ex: Action.run() { func_(); }
 * Make strategy decorator Ex: Action run() { s.perform(); }
 * FutureTask extend Task?
 *
 *
 * - FutureTask(Callable<V> callable)
 *      Creates a FutureTask that will, upon running, execute the given Callable.
 * - FutureTask(Runnable runnable, V result)
 *      Creates a FutureTask that will, upon running, execute the given Runnable,
 *      and arrange that get will return the given result on successful completion.
 *
 * TODO:
 *      - FutureTask should be able to process multiple future tasks
 *        Add that returns InstanceHandle, Get(InstanceHandle) that returns ResultHolder
 *      - Handle exceptions and set some flag to indicate failure
 *      - Perhaps resultHolder should be pair<Status, Return> where enum Status { OK, NotOk};
 *      - Add possibility of signal-slot listeners upon completion?
 *      - NB! Make thread-safe!!!
 *      - To support FutureTask.Cancel() I need to add a handle to the current TaskExecutor
 *
 * TODO: Support default future task policy, i.e., run once max attempt once.
 */
class DLL_STATE FutureFactory
        : public Templates::NoCopy<FutureFactory>
        , public StaticSingleton<FutureFactory>
{
public:
    // ----------------------------------------------------
    // Simple futures
    // ----------------------------------------------------

    template <typename Return>
    static typename BaseLib::Concurrent::Future<Return> Create(typename BaseLib::Concurrent::FutureTask<Return>::Ptr futureTask)
    {
        return Future<Return>(futureTask);
    }

    // ----------------------------------------------------
    // Runnable futures
    // ----------------------------------------------------

    static FutureTask<bool>::Ptr Create(Runnable::Ptr runnable)
    {
        auto runnableInvoker = std::make_shared<RunnableCaller<Runnable::Ptr>>(runnable);
        return std::make_shared<FutureTask<bool>>(runnableInvoker);
    }

    static FutureTask<bool>::Ptr Create(Runnable *runnable)
    {
        auto runnableInvoker = std::make_shared<RunnableCaller<Runnable*>>(runnable);
        return std::make_shared<FutureTask<bool>>(runnableInvoker);
    }

    // ----------------------------------------------------
    // Function pointer futures
    // ----------------------------------------------------

    template <typename Return, typename Class>
    static typename FutureTask<Return>::Ptr Create(Class* action, Return (Class::*member)())
    {
        typedef Templates::FunctionPointerInvoker0<Class, Return> StrategyInvoker;

        auto strategyInvoker = StrategyInvoker(action, member);
        auto runnableInvoker = std::make_shared<RunnableInvoker<Return, StrategyInvoker>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Class, typename Arg1>
    static typename FutureTask<Return>::Ptr Create(Class* action, Return (Class::*member)(Arg1), const Arg1 &arg1)
    {
        typedef typename Templates::FunctionPointerInvoker1<Class, Return, Arg1> StrategyInvoker;

        auto strategyInvoker = StrategyInvoker(action, member, arg1);
        auto runnableInvoker = std::make_shared<RunnableInvoker<Return, StrategyInvoker>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Class, typename Arg1, typename Arg2>
    static typename FutureTask<Return>::Ptr Create(Class* action, Return (Class::*member)(Arg1, Arg2), const Arg1 &arg1, const Arg2 &arg2)
    {
        typedef Templates::FunctionPointerInvoker2<Class, Return, Arg1, Arg2> StrategyInvoker;

        auto strategyInvoker = StrategyInvoker(action, member, arg1, arg2);
        auto runnableInvoker = std::make_shared<RunnableInvoker<Return, StrategyInvoker>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Class, typename Arg1, typename Arg2, typename Arg3>
    static typename FutureTask<Return>::Ptr Create(Class* action, Return (Class::*member)(Arg1, Arg2, Arg3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
    {
        typedef Templates::FunctionPointerInvoker3<Class, Return, Arg1, Arg2, Arg3> StrategyInvoker;

        auto strategyInvoker = StrategyInvoker(action, member, arg1, arg2, arg3);
        auto runnableInvoker = std::make_shared<RunnableInvoker<Return, StrategyInvoker>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Class, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename FutureTask<Return>::Ptr Create(Class* action, Return (Class::*member)(Arg1, Arg2, Arg3, Arg4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
    {
        typedef Templates::FunctionPointerInvoker4<Class, Return, Arg1, Arg2, Arg3, Arg4> StrategyInvoker;

        auto strategyInvoker = StrategyInvoker(action, member, arg1, arg2, arg3, arg4);
        auto runnableInvoker = std::make_shared<RunnableInvoker<Return, StrategyInvoker>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    // ----------------------------------------------------
    // Action futures
    // ----------------------------------------------------

    template <typename Return>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Action0<Return>::Ptr action)
    {
        typedef Templates::Action0Invoker<Return> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action);
        auto runnableInvoker = std::make_shared<RunnableActionInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }


    template <typename Return, typename Arg1>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Action1<Return, Arg1>::Ptr action, Arg1 arg1)
    {
        typedef Templates::Action1Invoker<Return, Arg1> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1);
        auto runnableInvoker = std::make_shared<RunnableActionInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Action2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        typedef Templates::Action2Invoker<Return, Arg1, Arg2> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2);
        auto runnableInvoker = std::make_shared<RunnableActionInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Action3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typedef Templates::Action3Invoker<Return, Arg1, Arg2, Arg3> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3);
        auto runnableInvoker = std::make_shared<RunnableActionInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Action4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        typedef Templates::Action4Invoker<Return, Arg1, Arg2, Arg3, Arg4> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3, arg4);
        auto runnableInvoker = std::make_shared<RunnableActionInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Action5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        typedef Templates::Action5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3, arg4, arg5);
        auto runnableInvoker = std::make_shared<RunnableActionInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    // ----------------------------------------------------
    // Strategy futures
    // ----------------------------------------------------

    template <typename Return>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy0<Return>::Ptr action)
    {
        typedef Templates::Strategy0Invoker<Return> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action);
        auto runnableInvoker = std::make_shared<RunnableStrategyInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy1<Return, Arg1>::Ptr action, Arg1 arg1)
    {
        typedef Templates::Strategy1Invoker<Return, Arg1> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1);
        auto runnableInvoker = std::make_shared<RunnableStrategyInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        typedef Templates::Strategy2Invoker<Return, Arg1, Arg2> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2);
        auto runnableInvoker = std::make_shared<RunnableStrategyInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typedef Templates::Strategy3Invoker<Return, Arg1, Arg2, Arg3> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3);
        auto runnableInvoker = std::make_shared<RunnableStrategyInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        typedef Templates::Strategy4Invoker<Return, Arg1, Arg2, Arg3, Arg4> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3, arg4);
        auto runnableInvoker = std::make_shared<RunnableStrategyInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        typedef Templates::Strategy5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3, arg4, arg5);
        auto runnableInvoker = std::make_shared<RunnableStrategyInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    // ----------------------------------------------------
    // Strategy futures
    // ----------------------------------------------------

    template <typename Return>
    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst0<Return>::Ptr action)
    {
        typedef Templates::StrategyConst0Invoker<Return> StrategyInvoker;
        auto strategyInvoker = std::make_shared<StrategyInvoker>(action);
        auto runnableInvoker = std::make_shared<RunnableStrategyConstInvoker<Return,StrategyInvoker>>(strategyInvoker);
        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1>
    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst1<Return, Arg1>::Ptr action, Arg1 arg1)
    {
        typedef Templates::StrategyConst1Invoker<Return, Arg1> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1);
        auto runnableInvoker = std::make_shared<RunnableStrategyConstInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        typedef Templates::StrategyConst2Invoker<Return, Arg1, Arg2> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2);
        auto runnableInvoker = std::make_shared<RunnableStrategyConstInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typedef Templates::StrategyConst3Invoker<Return, Arg1, Arg2, Arg3> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3);
        auto runnableInvoker = std::make_shared<RunnableStrategyConstInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        typedef Templates::StrategyConst4Invoker<Return, Arg1, Arg2, Arg3, Arg4> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3, arg4);
        auto runnableInvoker = std::make_shared<RunnableStrategyConstInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        typedef Templates::StrategyConst5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5> StrategyInvoker;

        auto strategyInvoker = std::make_shared<StrategyInvoker>(action, arg1, arg2, arg3, arg4, arg5);
        auto runnableInvoker = std::make_shared<RunnableStrategyConstInvoker<Return, std::shared_ptr<StrategyInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    // ----------------------------------------------------
    // Strategy futures
    // ----------------------------------------------------

    template <typename Return>
    static typename FutureTask<Return>::Ptr Create(std::function<Return ()> action)
    {
        typedef Templates::OperatorFunction0<Return> StrategyInvoker;
        typedef Templates::Operator0Invoker<Return> OperatorInvoker;

        auto operatorFunction = std::make_shared<StrategyInvoker>(action);
        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1>
    static typename FutureTask<Return>::Ptr Create(std::function<Return (Arg1)> action, Arg1 arg1)
    {
        typedef Templates::OperatorFunction1<Return, Arg1> StrategyInvoker;
        typedef Templates::Operator1Invoker<Return, Arg1> OperatorInvoker;

        auto operatorFunction = std::make_shared<StrategyInvoker>(action);
        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename FutureTask<Return>::Ptr Create(std::function<Return (Arg1, Arg2)> action, Arg1 arg1, Arg2 arg2)
    {
        typedef Templates::OperatorFunction2<Return, Arg1, Arg2> StrategyInvoker;
        typedef Templates::Operator2Invoker<Return, Arg1, Arg2> OperatorInvoker;

        auto operatorFunction = std::make_shared<StrategyInvoker>(action);
        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename FutureTask<Return>::Ptr Create(std::function<Return (Arg1, Arg2, Arg3)> action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typedef Templates::OperatorFunction3<Return, Arg1, Arg2, Arg3> StrategyInvoker;
        typedef Templates::Operator3Invoker<Return, Arg1, Arg2, Arg3> OperatorInvoker;

        auto operatorFunction = std::make_shared<StrategyInvoker>(action);
        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2, arg3);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename FutureTask<Return>::Ptr Create(std::function<Return (Arg1, Arg2, Arg3, Arg4)> action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        typedef Templates::OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4> StrategyInvoker;
        typedef Templates::Operator4Invoker<Return, Arg1, Arg2, Arg3, Arg4> OperatorInvoker;

        auto operatorFunction = std::make_shared<StrategyInvoker>(action);
        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2, arg3, arg4);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename FutureTask<Return>::Ptr Create(std::function<Return (Arg1, Arg2, Arg3, Arg4, Arg5)> action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        typedef Templates::OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5> StrategyInvoker;
        typedef Templates::Operator5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5> OperatorInvoker;

        auto operatorFunction = std::make_shared<StrategyInvoker>(action);
        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2, arg3, arg4, arg5);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    // ----------------------------------------------------
    // Strategy futures
    // ----------------------------------------------------

    template <typename Return>
    static typename FutureTask<Return>::Ptr Create(typename Templates::OperatorFunction0<Return>::Ptr operatorFunction)
    {
        typedef Templates::Operator0Invoker<Return> OperatorInvoker;

        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1>
    static typename FutureTask<Return>::Ptr Create(typename Templates::OperatorFunction1<Return, Arg1>::Ptr operatorFunction, Arg1 arg1)
    {
        typedef Templates::Operator1Invoker<Return, Arg1> OperatorInvoker;

        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename FutureTask<Return>::Ptr Create(typename Templates::OperatorFunction2<Return, Arg1, Arg2>::Ptr operatorFunction, Arg1 arg1, Arg2 arg2)
    {
        typedef Templates::Operator2Invoker<Return, Arg1, Arg2> OperatorInvoker;

        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename FutureTask<Return>::Ptr Create(typename Templates::OperatorFunction3<Return, Arg1, Arg2, Arg3>::Ptr operatorFunction, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typedef Templates::Operator3Invoker<Return, Arg1, Arg2, Arg3> OperatorInvoker;

        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2, arg3);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename FutureTask<Return>::Ptr Create(typename Templates::OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr operatorFunction, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        typedef Templates::Operator4Invoker<Return, Arg1, Arg2, Arg3, Arg4> OperatorInvoker;

        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2, arg3, arg4);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename FutureTask<Return>::Ptr Create(typename Templates::OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr operatorFunction, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        typedef Templates::Operator5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5> OperatorInvoker;

        auto strategyInvoker = std::make_shared<OperatorInvoker>(operatorFunction, arg1, arg2, arg3, arg4, arg5);
        auto runnableInvoker = std::make_shared<RunnableFunctionInvoker<Return, std::shared_ptr<OperatorInvoker>>>(strategyInvoker);

        return std::make_shared<FutureTask<Return>>(runnableInvoker);
    }
};

}}

