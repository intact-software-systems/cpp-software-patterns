#ifndef BaseLib_Templates_FunctionProxy_h_IsIncluded
#define BaseLib_Templates_FunctionProxy_h_IsIncluded

#include "BaseLib/CommonDefines.h"

#include "BaseLib/Templates/MethodInterfaces.h"
#include "BaseLib/Templates/ActionHolder.h"

namespace BaseLib { namespace Templates
{

// ----------------------------------------------------
// Function0
// ----------------------------------------------------

template <typename Return>
class OperatorFunction0 : public OperatorMethod0<Return>
{
public:
    OperatorFunction0(std::function<Return ()> function)
        : function_(function)
    {}
    virtual ~OperatorFunction0() { }

    CLASS_TRAITS(OperatorFunction0)

    virtual Return operator()()
    {
        return function_();
    }

private:
    std::function<Return ()> function_;
};

// ----------------------------------------------------
// Function1
// ----------------------------------------------------

template <typename Return, typename Arg1>
class OperatorFunction1
    : public OperatorMethod1<Return, Arg1>
    , public std::enable_shared_from_this<OperatorFunction1<Return, Arg1>>
{
public:
    OperatorFunction1(std::function<Return (Arg1)> function)
        : function_(function)
    {}
    virtual ~OperatorFunction1() { }

    CLASS_TRAITS(OperatorFunction1)

    virtual Return operator()(Arg1 arg1)
    {
        return function_(arg1);
    }

    template <typename Return1>
    std::shared_ptr<OperatorFunction1<Return1, Arg1>> After(std::function<Return1(Return)> after)
    {
        std::function<Return1 (Arg1)> composer = [=](Arg1 val) mutable -> Return1
                        {
                            return after(this->operator()(val));
                        };


        return std::make_shared<OperatorFunction1<Return1, Arg1>>(composer);
    }

    template <typename Arg2>
    std::shared_ptr<OperatorFunction1<Arg1, Arg2>> Before(std::function<Arg1 (Arg2)> before)
    {
        std::function<Arg1 (Arg2)> composer = [=](Arg2 val) mutable -> Arg1
        {
            return this->operator()(before(val));
        };

        return std::make_shared<OperatorFunction1<Arg1, Arg2>>(composer);
    }

private:
    std::function< Return (Arg1) > function_;
};

// ----------------------------------------------------
// Function2
// ----------------------------------------------------

template <typename Return, typename Arg1, typename Arg2>
class OperatorFunction2 : public OperatorMethod2<Return, Arg1, Arg2>
{
public:
    OperatorFunction2(std::function<Return (Arg1, Arg2)> function)
        : function_(function)
    {}

    virtual ~OperatorFunction2() { }

    CLASS_TRAITS(OperatorFunction2)

    virtual Return operator()(Arg1 arg1, Arg2 arg2)
    {
        return function_(arg1, arg2);
    }

    template <typename Return1>
    std::shared_ptr<OperatorFunction2<Return1, Arg1, Arg2>> After(std::function<Return1(Return)> after)
    {
        std::function<Return1 (Arg1, Arg2)> composer = [=](Arg1 arg1, Arg2 arg2) mutable -> Return1
        {
            return after(this->operator()(arg1, arg2));
        };

        return std::make_shared<OperatorFunction2<Return1, Arg1, Arg2>>(composer);
    }

    template <typename Arg3>
    std::shared_ptr<OperatorFunction2<Arg1, Arg2, Arg3>> Before(std::function<Arg1 (Arg2, Arg3)> before)
    {
        std::function<Arg1 (Arg2, Arg3)> composer = [=](Arg2 arg2, Arg3 arg3) mutable -> Arg1
        {
            return this->operator()(before(arg2, arg3), arg2);
        };

        return std::make_shared<OperatorFunction2<Arg1, Arg2, Arg3>>(composer);
    }


private:
    std::function< Return (Arg1, Arg2) > function_;
};

// ----------------------------------------------------
// Function3
// ----------------------------------------------------

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class OperatorFunction3 : public OperatorMethod3<Return, Arg1, Arg2, Arg3>
{
public:
    OperatorFunction3(std::function<Return (Arg1, Arg2, Arg3)> function)
        : function_(function)
    {}

    virtual ~OperatorFunction3() { }

    CLASS_TRAITS(OperatorFunction3)

    virtual Return operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        return function_(arg1, arg2, arg3);
    }

private:
    std::function< Return (Arg1, Arg2, Arg3) > function_;
};

// ----------------------------------------------------
// Function4
// ----------------------------------------------------

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class OperatorFunction4 : public OperatorMethod4<Return, Arg1, Arg2, Arg3, Arg4>
{
public:
    OperatorFunction4(std::function<Return (Arg1, Arg2, Arg3, Arg4)> function)
        : function_(function)
    {}
    virtual ~OperatorFunction4() { }

    CLASS_TRAITS(OperatorFunction4)

    virtual Return operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        return function_(arg1, arg2, arg3, arg4);
    }

private:
    std::function< Return (Arg1, Arg2, Arg3, Arg4) > function_;
};

// ----------------------------------------------------
// Function5
// ----------------------------------------------------

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class OperatorFunction5 : public OperatorMethod5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
{
public:
    OperatorFunction5(std::function<Return (Arg1, Arg2, Arg3, Arg4, Arg5)> function)
        : function_(function)
    {}
    virtual ~OperatorFunction5() { }

    CLASS_TRAITS(OperatorFunction5)

    virtual Return operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        return function_(arg1, arg2, arg3, arg4, arg5);
    }

private:
    std::function< Return (Arg1, Arg2, Arg3, Arg4, Arg5) > function_;
};

// --------------------------------------------
// Strategy invokers
// --------------------------------------------

template <typename Return>
class Operator0Invoker : public OperatorMethod0<Return>
{
public:
    Operator0Invoker(std::shared_ptr<OperatorFunction0<Return>> strategy)
        : strategy_(strategy)
    {}
    virtual ~Operator0Invoker()
    {}

    CLASS_TRAITS(Operator0Invoker)

    virtual Return operator()()
    {
        return strategy_->operator()();
    }

private:
    std::shared_ptr<OperatorFunction0<Return>> strategy_;
};

template <typename Return, typename Arg1>
class Operator1Invoker : public OperatorMethod0<Return>
{
public:
    Operator1Invoker(std::shared_ptr<OperatorFunction1<Return, Arg1>> &strategy, const Arg1 &arg1)
        : strategy_(strategy)
        , arg1_(arg1)
    {}
    virtual ~Operator1Invoker()
    {}

    CLASS_TRAITS(Operator1Invoker)

    virtual Return operator()()
    {
        return strategy_->operator()(arg1_);
    }

private:
    std::shared_ptr<OperatorFunction1<Return, Arg1>> strategy_;

    Arg1 arg1_;
};

template <typename Return, typename Arg1, typename Arg2>
class Operator2Invoker : public OperatorMethod0<Return>
{
public:
    Operator2Invoker(std::shared_ptr<OperatorFunction2<Return, Arg1, Arg2>> strategy, const Arg1 &arg1, const Arg2 &arg2)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
    {}
    virtual ~Operator2Invoker()
    {}

    CLASS_TRAITS(Operator2Invoker)

    virtual Return operator()()
    {
        return strategy_->operator()(arg1_, arg2_);
    }

private:
    std::shared_ptr<OperatorFunction2<Return, Arg1, Arg2>> strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class Operator3Invoker : public OperatorMethod0<Return>
{
public:
    Operator3Invoker(std::shared_ptr<OperatorFunction3<Return, Arg1, Arg2, Arg3>> strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    {}
    virtual ~Operator3Invoker()
    {}

    CLASS_TRAITS(Operator3Invoker)

    virtual Return operator()()
    {
        return strategy_->operator()(arg1_, arg2_, arg3_);
    }

private:
    std::shared_ptr<OperatorFunction3<Return, Arg1, Arg2, Arg3>> strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Operator4Invoker : public OperatorMethod0<Return>
{
public:
    Operator4Invoker(std::shared_ptr<OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>> strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    {}
    virtual ~Operator4Invoker()
    {}

    CLASS_TRAITS(Operator4Invoker)

    virtual Return operator()()
    {
        return strategy_->operator()(arg1_, arg2_, arg3_, arg4_);
    }

private:
    std::shared_ptr<OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>> strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class Operator5Invoker : public OperatorMethod0<Return>
{
public:
    Operator5Invoker(std::shared_ptr<OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>> strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
        , arg5_(arg5)
    {}
    virtual ~Operator5Invoker()
    {}

    CLASS_TRAITS(Operator5Invoker)

    virtual Return operator()()
    {
        return strategy_->operator()(arg1_, arg2_, arg3_, arg4_, arg5_);
    }

private:
    std::shared_ptr<OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>> strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
    Arg5 arg5_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return>
class OperatorAction0Invoker : public Action0<Return>
{
public:
    OperatorAction0Invoker(typename OperatorFunction0<Return>::Ptr action)
            : action_(action)
    {}
    virtual ~OperatorAction0Invoker()
    {}

    CLASS_TRAITS(OperatorAction0Invoker)

    virtual Return Invoke()
    {
        return action_->operator()();
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
    typename OperatorFunction0<Return>::Ptr action_;
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1>
class OperatorAction1Invoker : public Action0<Return>
{
public:
    OperatorAction1Invoker(typename OperatorFunction1<Return, Arg1>::Ptr action, Arg1 arg1)
            : action_(action)
            , arg1_(arg1)
    {}
    virtual ~OperatorAction1Invoker()
    {}

    CLASS_TRAITS(OperatorAction1Invoker)

    virtual Return Invoke()
    {
        return action_->operator()(arg1_);
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
    typename OperatorFunction1<Return, Arg1>::Ptr action_;
    Arg1 arg1_;
};


// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return, typename Arg1, typename Arg2>
class OperatorAction2Invoker : public Action0<Return>
{
public:
    OperatorAction2Invoker(typename OperatorFunction2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
            : action_(action)
            , arg1_(arg1)
            , arg2_(arg2)
    {}
    virtual ~OperatorAction2Invoker()
    {}

    CLASS_TRAITS(OperatorAction2Invoker)

    virtual Return Invoke()
    {
        return action_->operator()(arg1_, arg2_);
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
    typename OperatorFunction2<Return, Arg1, Arg2>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class OperatorAction3Invoker : public Action0<Return>
{
public:
    OperatorAction3Invoker(typename OperatorFunction3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
            : action_(action)
            , arg1_(arg1)
            , arg2_(arg2)
            , arg3_(arg3)
    {}
    virtual ~OperatorAction3Invoker()
    {}

    CLASS_TRAITS(OperatorAction3Invoker)

    virtual Return Invoke()
    {
        return action_->operator()(arg1_, arg2_, arg3_);
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
    typename OperatorFunction3<Return, Arg1, Arg2, Arg3>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};


template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class OperatorAction4Invoker : public Action0<Return>
{
public:
    OperatorAction4Invoker(typename OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
            : action_(action)
            , arg1_(arg1)
            , arg2_(arg2)
            , arg3_(arg3)
            , arg4_(arg4)
    {}
    virtual ~OperatorAction4Invoker()
    {}

    CLASS_TRAITS(OperatorAction4Invoker)

    virtual Return Invoke()
    {
        return action_->operator()(arg1_, arg2_, arg3_, arg4_);
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
    typename OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
};


template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class OperatorAction5Invoker : public Action0<Return>
{
public:
    OperatorAction5Invoker(typename OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
            : action_(action)
            , arg1_(arg1)
            , arg2_(arg2)
            , arg3_(arg3)
            , arg4_(arg4)
            , arg5_(arg5)
    {}
    virtual ~OperatorAction5Invoker()
    {}

    CLASS_TRAITS(OperatorAction5Invoker)

    virtual Return Invoke()
    {
        return action_->operator()(arg1_, arg2_, arg3_, arg4_, arg5_);
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
    typename OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
    Arg5 arg5_;
};


// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return>
class OperatorContextAction0 : public OperatorAction0Invoker<Return>
{
public:
    OperatorContextAction0(ActionContextPtr context, typename OperatorFunction0<Return>::Ptr action)
            : OperatorAction0Invoker<Return>(action)
            , context_(context)
    {}
    virtual ~OperatorContextAction0()
    {}

    CLASS_TRAITS(OperatorContextAction0)

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
class OperatorContextAction1 : public OperatorAction1Invoker<Return, Arg1>
{
public:
    OperatorContextAction1(ActionContextPtr context, typename OperatorFunction1<Return, Arg1>::Ptr action, Arg1 arg1)
            : OperatorAction1Invoker<Return, Arg1>(action, arg1)
            , context_(context)
    {}
    virtual ~OperatorContextAction1()
    {}

    CLASS_TRAITS(OperatorContextAction1)

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
class OperatorContextAction2 : public OperatorAction2Invoker<Return, Arg1, Arg2>
{
public:
    OperatorContextAction2(ActionContextPtr context, typename OperatorFunction2<Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
            : OperatorAction2Invoker<Return, Arg1, Arg2>(action, arg1, arg2)
            , context_(context)
    {}
    virtual ~OperatorContextAction2()
    {}

    CLASS_TRAITS(OperatorContextAction2)

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
class OperatorContextAction3 : public OperatorAction3Invoker<Return, Arg1, Arg2, Arg3>
{
public:
    OperatorContextAction3(ActionContextPtr context, typename OperatorFunction3<Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
            : OperatorAction3Invoker<Return, Arg1, Arg2, Arg3>(action, arg1, arg2, arg3)
            , context_(context)
    {}
    virtual ~OperatorContextAction3()
    {}

    CLASS_TRAITS(OperatorContextAction3)

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
class OperatorContextAction4 : public OperatorAction4Invoker<Return, Arg1, Arg2, Arg3, Arg4>
{
public:
    OperatorContextAction4(ActionContextPtr context, typename OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
            : OperatorAction4Invoker<Return, Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4)
            , context_(context)
    {}
    virtual ~OperatorContextAction4()
    {}

    CLASS_TRAITS(OperatorContextAction4)

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
class OperatorContextAction5 : public OperatorAction5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
{
public:
    OperatorContextAction5(ActionContextPtr context, typename OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
            : OperatorAction5Invoker<Return, Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5)
            , context_(context)
    {}
    virtual ~OperatorContextAction5()
    {}

    CLASS_TRAITS(OperatorContextAction5)

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
