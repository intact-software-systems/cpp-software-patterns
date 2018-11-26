#pragma once

#include"RxFSM/IncludeExtLibs.h"

namespace Reactor
{

/**
 * Do I need an interface for state machine contexts?
 */
template <typename Context>
class StateMachineContext
{
public:
    StateMachineContext(std::shared_ptr<Context> context)
        : context_(context)
    {}
    virtual ~StateMachineContext()
    {}

    const Context& context() const
    {
        return context_.context().operator *();
    }

    Context& context()
    {
        return context_.context().operator *();
    }

private:
    Templates::ContextDataShared<Context> context_;
};

//template<typename Return, typename T>
//class StateAction : public StrategyContextAction1<Return, T>
//{
//public:
//    StateAction(ActionContextPtr context, typename Strategy1<Return, T>::Ptr action)
//            : Templates::StrategyContextAction1<Return, T>(context, action, T())
//    { }
//};

/**
 * I want every action in a state machine to have access to a user defined context.
 */
template<typename Context, typename Return>
class StateMachineAction0
        : public Templates::Action0<Return>
        , public StateMachineContext<Context>
{
public:
    StateMachineAction0(std::shared_ptr<Context> context) : StateMachineContext<Context>(context)
    { }
    virtual ~StateMachineAction0()
    { }

    CLASS_TRAITS(StateMachineAction0)

    virtual void Entry()
    {
        this->context().Entry();
    }

    virtual void Exit()
    {
        this->context().Exit();
    }
};

template<typename Context, typename Return, typename Arg1>
class StateMachineAction1
        : public Templates::Action1<Return, Arg1>
        , public StateMachineContext<Context>
{
public:
    StateMachineAction1(std::shared_ptr<Context> context) : StateMachineContext<Context>(context)
    { }
    virtual ~StateMachineAction1()
    { }

    CLASS_TRAITS(StateMachineAction1)

    virtual void Entry()
    {
        this->context().Entry();
    }

    virtual void Exit()
    {
        this->context().Exit();
    }
};


template<typename Context, typename Return, typename Arg1, typename Arg2>
class StateMachineAction2
        : public Templates::Action2<Return, Arg1, Arg2>
        , public StateMachineContext<Context>
{
public:
    StateMachineAction2(std::shared_ptr<Context> context) : StateMachineContext<Context>(context)
    { }
    virtual ~StateMachineAction2()
    { }

    CLASS_TRAITS(StateMachineAction2)

    virtual void Entry()
    {
        this->context().Entry();
    }

    virtual void Exit()
    {
        this->context().Exit();
    }
};

template<typename Context, typename Return, typename Arg1, typename Arg2, typename Arg3>
class StateMachineAction3
        : public Templates::Action3<Return, Arg1, Arg2, Arg3>
        , public StateMachineContext<Context>
{
public:
    StateMachineAction3(std::shared_ptr<Context> context) : StateMachineContext<Context>(context)
    { }
    virtual ~StateMachineAction3()
    { }

    CLASS_TRAITS(StateMachineAction3)

    virtual void Entry()
    {
        this->context().Entry();
    }

    virtual void Exit()
    {
        this->context().Exit();
    }
};

template<typename Context, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StateMachineAction4
        : public Templates::Action4<Return, Arg1, Arg2, Arg3, Arg4>
        , public StateMachineContext<Context>
{
public:
    StateMachineAction4(std::shared_ptr<Context> context) : StateMachineContext<Context>(context)
    { }
    virtual ~StateMachineAction4()
    { }

    CLASS_TRAITS(StateMachineAction4)

    virtual void Entry()
    {
        this->context().Entry();
    }

    virtual void Exit()
    {
        this->context().Exit();
    }
};

template<typename Context, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StateMachineAction5
        : public Templates::Action5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
        , public StateMachineContext<Context>
{
public:
    StateMachineAction5(std::shared_ptr<Context> context) : StateMachineContext<Context>(context)
    { }
    virtual ~StateMachineAction5()
    { }

    CLASS_TRAITS(StateMachineAction5)

    virtual void Entry()
    {
        this->context().Entry();
    }

    virtual void Exit()
    {
        this->context().Exit();
    }
};

}
