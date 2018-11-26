#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/FutureTask.h"
#include"RxConcurrent/FutureFactory.h"

namespace BaseLib { namespace Concurrent
{

// ---------------------------------------------------------
// Strategy base for creating futures
// ---------------------------------------------------------

template <typename Return>
class StaticStrategyFutureFactory0
{
public:
    virtual ~StaticStrategyFutureFactory0() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy0<Return>::Ptr action)
    {
        return FutureFactory::Create<Return>(action);
    }
};

template <typename Return, typename Arg1>
class StaticStrategyFutureFactory1
{
public:
    virtual ~StaticStrategyFutureFactory1() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy1<Return, Arg1>::Ptr action, Arg1 arg1)
    {
        return FutureFactory::Create<Return, Arg1>(action, arg1);
    }
};

template <typename Return, typename Arg1, typename Arg2>
class StaticStrategyFutureFactory2
{
public:
    virtual ~StaticStrategyFutureFactory2() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        return FutureFactory::Create<Return, Arg1, Arg2>(action, arg1, arg2);
    }
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class StaticStrategyFutureFactory3
{
public:
    virtual ~StaticStrategyFutureFactory3() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        return FutureFactory::Create<Return, Arg1, Arg2, Arg3>(action, arg1, arg2, arg3);
    }
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StaticStrategyFutureFactory4
{
public:
    virtual ~StaticStrategyFutureFactory4() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        return FutureFactory::Create<Return, Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4);
    }
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StaticStrategyFutureFactory5
{
public:
    virtual ~StaticStrategyFutureFactory5() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        return FutureFactory::Create<Return, Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5);
    }
};

// ---------------------------------------------------------
// Strategy base for creating futures
// ---------------------------------------------------------

template <typename Return>
class StaticStrategyConstFutureFactory0
{
public:
    virtual ~StaticStrategyConstFutureFactory0() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst0<Return>::Ptr action)
    {
        return FutureFactory::Create<Return>(action);
    }
};

template <typename Return, typename Arg1>
class StaticStrategyConstFutureFactory1
{
public:
    virtual ~StaticStrategyConstFutureFactory1() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst1<Return, Arg1>::Ptr action, Arg1 arg1)
    {
        return FutureFactory::Create<Return, Arg1>(action, arg1);
    }
};

template <typename Return, typename Arg1, typename Arg2>
class StaticStrategyConstFutureFactory2
{
public:
    virtual ~StaticStrategyConstFutureFactory2() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        return FutureFactory::Create<Return, Arg1, Arg2>(action, arg1, arg2);
    }
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class StaticStrategyConstFutureFactory3
{
public:
    virtual ~StaticStrategyConstFutureFactory3() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        return FutureFactory::Create<Return, Arg1, Arg2, Arg3>(action, arg1, arg2, arg3);
    }
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StaticStrategyConstFutureFactory4
{
public:
    virtual ~StaticStrategyConstFutureFactory4() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        return FutureFactory::Create<Return, Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4);
    }
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StaticStrategyConstFutureFactory5
{
public:
    virtual ~StaticStrategyConstFutureFactory5() {}

    static typename FutureTask<Return>::Ptr Create(typename Templates::StrategyConst5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        return FutureFactory::Create<Return, Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5);
    }
};

}}
