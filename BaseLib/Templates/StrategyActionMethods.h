#ifndef BaseLib_Templates_StrategyActionMethods_h_Included
#define BaseLib_Templates_StrategyActionMethods_h_Included

#include"BaseLib/CommonDefines.h"

#include"BaseLib/Templates/ActionHolder.h"
#include"BaseLib/Templates/StrategyMethods.h"

namespace BaseLib { namespace Templates
{

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return>
class StrategyAction0Invoker : public Action0<Return>
{
public:
    StrategyAction0Invoker(typename Strategy0<Return>::Ptr action)
        : action_(action)
    {}
    virtual ~StrategyAction0Invoker()
    {}

    CLASS_TRAITS(StrategyAction0Invoker)

    virtual Return Invoke()
    {
        return action_->Perform();
    }

    virtual void Exit()
    { }

    virtual void Entry()
    {  }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

private:
    typename Strategy0<Return>::Ptr action_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1>
class StrategyAction1Invoker : public Action0<Return>
{
public:
    StrategyAction1Invoker(typename Strategy1<Return, Arg1>::Ptr action, Arg1 arg1)
        : action_(action)
        , arg1_(arg1)
    {}
    virtual ~StrategyAction1Invoker()
    {}

    CLASS_TRAITS(StrategyAction1Invoker)

    virtual Return Invoke()
    {
        return action_->Perform(arg1_);
    }

    virtual void Exit()
    { }

    virtual void Entry()
    {  }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

private:
    typename Strategy1<Return, Arg1>::Ptr action_;
    Arg1 arg1_;
};


// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1, typename Arg2>
class StrategyAction2Invoker : public Action0<Return>
{
public:
    StrategyAction2Invoker(typename Strategy2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
    {}
    virtual ~StrategyAction2Invoker()
    {}

    CLASS_TRAITS(StrategyAction2Invoker)

    virtual Return Invoke()
    {
        return action_->Perform(arg1_, arg2_);
    }

    virtual void Exit()
    { }

    virtual void Entry()
    {  }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

private:
    typename Strategy2<Return, Arg1, Arg2>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class StrategyAction3Invoker : public Action0<Return>
{
public:
    StrategyAction3Invoker(typename Strategy3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    {}
    virtual ~StrategyAction3Invoker()
    {}

    CLASS_TRAITS(StrategyAction3Invoker)

    virtual Return Invoke()
    {
        return action_->Perform(arg1_, arg2_, arg3_);
    }

    virtual void Exit()
    { }

    virtual void Entry()
    {  }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

private:
    typename Strategy3<Return, Arg1, Arg2, Arg3>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};


template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StrategyAction4Invoker : public Action0<Return>
{
public:
    StrategyAction4Invoker(typename Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    {}
    virtual ~StrategyAction4Invoker()
    {}

    CLASS_TRAITS(StrategyAction4Invoker)

    virtual Return Invoke()
    {
        return action_->Perform(arg1_, arg2_, arg3_, arg4_);
    }

    virtual void Exit()
    { }

    virtual void Entry()
    {  }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

private:
    typename Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
};


template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StrategyAction5Invoker : public Action0<Return>
{
public:
    StrategyAction5Invoker(typename Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
        , arg5_(arg5)
    {}
    virtual ~StrategyAction5Invoker()
    {}

    CLASS_TRAITS(StrategyAction5Invoker)

    virtual Return Invoke()
    {
        return action_->Perform(arg1_, arg2_, arg3_, arg4_, arg5_);
    }

    virtual void Exit()
    { }

    virtual void Entry()
    {  }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

private:
    typename Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
    Arg5 arg5_;
};

}}

#endif
