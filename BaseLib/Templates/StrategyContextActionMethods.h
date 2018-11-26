#ifndef BaseLib_Templates_StrategyContextActionMethods_h_Included
#define BaseLib_Templates_StrategyContextActionMethods_h_Included

#include"BaseLib/CommonDefines.h"

#include"BaseLib/Templates/ActionHolder.h"
#include"BaseLib/Templates/StrategyActionMethods.h"

namespace BaseLib { namespace Templates
{

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return>
class StrategyContextAction0 : public StrategyAction0Invoker<Return>
{
public:
    StrategyContextAction0(ActionContextPtr context, typename Strategy0<Return>::Ptr action)
        : StrategyAction0Invoker<Return>(action)
        , context_(context)
    {}
    virtual ~StrategyContextAction0()
    {}

    CLASS_TRAITS(StrategyContextAction0)

    virtual void Exit()
    {
        context_->Exit();
    }

    virtual void Entry()
    {
        context_->Entry();
    }

private:
    ActionContextPtr context_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1>
class StrategyContextAction1 : public StrategyAction1Invoker<Return, Arg1>
{
public:
    StrategyContextAction1(ActionContextPtr context, typename Strategy1<Return, Arg1>::Ptr action, Arg1 arg1)
        : StrategyAction1Invoker<Return, Arg1>(action, arg1)
        , context_(context)
    {}
    virtual ~StrategyContextAction1()
    {}

    CLASS_TRAITS(StrategyContextAction1)

    virtual void Exit()
    {
        context_->Exit();
    }

    virtual void Entry()
    {
        context_->Entry();
    }

private:
    ActionContextPtr context_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1, typename Arg2>
class StrategyContextAction2 : public StrategyAction2Invoker<Return, Arg1, Arg2>
{
public:
    StrategyContextAction2(ActionContextPtr context, typename Strategy2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
        : StrategyAction2Invoker<Return, Arg1, Arg2>(action, arg1, arg2)
        , context_(context)
    {}
    virtual ~StrategyContextAction2()
    {}

    CLASS_TRAITS(StrategyContextAction2)

    virtual void Exit()
    {
        context_->Exit();
    }

    virtual void Entry()
    {
        context_->Entry();
    }

private:
    ActionContextPtr context_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class StrategyContextAction3 : public StrategyAction3Invoker<Return, Arg1, Arg2, Arg3>
{
public:
    StrategyContextAction3(ActionContextPtr context, typename Strategy3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : StrategyAction3Invoker<Return, Arg1, Arg2, Arg3>(action, arg1, arg2, arg3)
        , context_(context)
    {}
    virtual ~StrategyContextAction3()
    {}

    CLASS_TRAITS(StrategyContextAction3)

    virtual void Exit()
    {
        context_->Exit();
    }

    virtual void Entry()
    {
        context_->Entry();
    }

private:
    ActionContextPtr context_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StrategyContextAction4 : public StrategyAction4Invoker<Return, Arg1, Arg2, Arg3, Arg4>
{
public:
    StrategyContextAction4(ActionContextPtr context, typename Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : StrategyAction4Invoker<Return, Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4)
        , context_(context)
    {}
    virtual ~StrategyContextAction4()
    {}

    CLASS_TRAITS(StrategyContextAction4)

    virtual void Exit()
    {
        context_->Exit();
    }

    virtual void Entry()
    {
        context_->Entry();
    }

private:
    ActionContextPtr context_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StrategyContextAction5 : public StrategyAction5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
{
public:
    StrategyContextAction5(ActionContextPtr context, typename Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : StrategyAction5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5)
        , context_(context)
    {}
    virtual ~StrategyContextAction5()
    {}

    CLASS_TRAITS(StrategyContextAction5)

    virtual void Exit()
    {
        context_->Exit();
    }

    virtual void Entry()
    {
        context_->Entry();
    }

private:
    ActionContextPtr context_;
};

}}

#endif
