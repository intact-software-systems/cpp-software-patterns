#ifndef BaseLib_Templates_StrategyMethods_h_Included
#define BaseLib_Templates_StrategyMethods_h_Included

#include"BaseLib/CommonDefines.h"

namespace BaseLib { namespace Templates
{

// --------------------------------------------
// Convenience methods
// --------------------------------------------

class IsStrategyMethod
{
public:
    virtual ~IsStrategyMethod() {}

    virtual bool IsStrategy() const = 0;
};

// --------------------------------------------
// Strategy methods
// --------------------------------------------

template <typename Return>
class Strategy0
{
public:
    virtual ~Strategy0() {}

    CLASS_TRAITS(Strategy0)

    virtual Return Perform() = 0;
};

template <typename Return, typename Arg1>
class Strategy1
{
public:
    virtual ~Strategy1() {}

    CLASS_TRAITS(Strategy1)

    virtual Return Perform(Arg1 t) = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class Strategy2
{
public:
    virtual ~Strategy2() {}

    CLASS_TRAITS(Strategy2)

    virtual Return Perform(Arg1 t, Arg2 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class Strategy3
{
public:
    virtual ~Strategy3() {}

    CLASS_TRAITS(Strategy3)

    virtual Return Perform(Arg1 t, Arg2 u, Arg3 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Strategy4
{
public:
    virtual ~Strategy4() {}

    CLASS_TRAITS(Strategy4)

    virtual Return Perform(Arg1 t, Arg2 u, Arg3 v, Arg4 w) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class Strategy5
{
public:
    virtual ~Strategy5() {}

    CLASS_TRAITS(Strategy5)

    virtual Return Perform(Arg1 t, Arg2 u, Arg3 v, Arg4 w, Arg5 x) = 0;
};

// --------------------------------------------
// Const Strategy methods
// --------------------------------------------

template <typename Return>
class StrategyConst0
{
public:
    virtual ~StrategyConst0() {}

    CLASS_TRAITS(StrategyConst0)

    virtual Return Perform() const = 0;
};

template <typename Return, typename Arg1>
class StrategyConst1
{
public:
    virtual ~StrategyConst1() {}

    CLASS_TRAITS(StrategyConst1)

    virtual Return Perform(const Arg1 &t) const = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class StrategyConst2
{
public:
    virtual ~StrategyConst2() {}

    CLASS_TRAITS(StrategyConst2)

    virtual Return Perform(const Arg1 &t, const Arg2 &v) const = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class StrategyConst3
{
public:
    virtual ~StrategyConst3() {}

    CLASS_TRAITS(StrategyConst3)

    virtual Return Perform(const Arg1 &t, const Arg2 &u, const Arg3 &v) const = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StrategyConst4
{
public:
    virtual ~StrategyConst4() {}

    CLASS_TRAITS(StrategyConst4)

    virtual Return Perform(const Arg1 &t, const Arg2 &u, const Arg3 &v, const Arg4 &w) const = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StrategyConst5
{
public:
    virtual ~StrategyConst5() {}

    CLASS_TRAITS(StrategyConst5)

    virtual Return Perform(const Arg1 &t, const Arg2 &u, const Arg3 &v, const Arg4 &w, const Arg5 &x) const = 0;
};

// --------------------------------------------
// Strategy invokers
// --------------------------------------------

template <typename Return>
class Strategy0Invoker : public Strategy0<Return>
{
public:
    Strategy0Invoker(const typename Strategy0<Return>::Ptr &strategy)
        : strategy_(strategy)
    {}
    virtual ~Strategy0Invoker()
    {}

    CLASS_TRAITS(Strategy0Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform();
    }

private:
    typename Strategy0<Return>::Ptr strategy_;
};

template <typename Return, typename Arg1>
class Strategy1Invoker : public Strategy0<Return>
{
public:
    Strategy1Invoker(const typename Strategy1<Return, Arg1>::Ptr &strategy, const Arg1 &arg1)
        : strategy_(strategy)
        , arg1_(arg1)
    {}
    virtual ~Strategy1Invoker()
    {}

    CLASS_TRAITS(Strategy1Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform(arg1_);
    }

private:
    typename Strategy1<Return, Arg1>::Ptr strategy_;

    Arg1 arg1_;
};

template <typename Return, typename Arg1, typename Arg2>
class Strategy2Invoker : public Strategy0<Return>
{
public:
    Strategy2Invoker(const typename Strategy2<Return, Arg1, Arg2>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
    {}
    virtual ~Strategy2Invoker()
    {}

    CLASS_TRAITS(Strategy2Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform(arg1_, arg2_);
    }

private:
    typename Strategy2<Return, Arg1, Arg2>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class Strategy3Invoker : public Strategy0<Return>
{
public:
    Strategy3Invoker(const typename Strategy3<Return, Arg1, Arg2, Arg3>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    {}
    virtual ~Strategy3Invoker()
    {}

    CLASS_TRAITS(Strategy3Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform(arg1_, arg2_, arg3_);
    }

private:
    typename Strategy3<Return, Arg1, Arg2, Arg3>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Strategy4Invoker : public Strategy0<Return>
{
public:
    Strategy4Invoker(const typename Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    {}
    virtual ~Strategy4Invoker()
    {}

    CLASS_TRAITS(Strategy4Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform(arg1_, arg2_, arg3_, arg4_);
    }

private:
    typename Strategy4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class Strategy5Invoker : public Strategy0<Return>
{
public:
    Strategy5Invoker(const typename Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
        , arg5_(arg5)
    {}
    virtual ~Strategy5Invoker()
    {}

    CLASS_TRAITS(Strategy5Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform(arg1_, arg2_, arg3_, arg4_, arg5_);
    }

private:
    typename Strategy5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
    Arg5 arg5_;
};

// --------------------------------------------
// Const strategy invokers
// --------------------------------------------

template <typename Return>
class StrategyConst0Invoker : public StrategyConst0<Return>
{
public:
    StrategyConst0Invoker(const typename StrategyConst0<Return>::Ptr &strategy)
        : strategy_(strategy)
    {}
    virtual ~StrategyConst0Invoker()
    {}

    CLASS_TRAITS(StrategyConst0Invoker)

    virtual Return Perform() const
    {
        return strategy_->Perform();
    }

private:
    typename StrategyConst0<Return>::Ptr strategy_;
};

template <typename Return, typename Arg1>
class StrategyConst1Invoker : public StrategyConst0<Return>
{
public:
    StrategyConst1Invoker(const typename StrategyConst1<Return, Arg1>::Ptr &strategy, const Arg1 &arg1)
        : strategy_(strategy)
        , arg1_(arg1)
    {}
    virtual ~StrategyConst1Invoker()
    {}

    CLASS_TRAITS(StrategyConst1Invoker)

    virtual Return Perform() const
    {
        return strategy_->Perform(arg1_);
    }

private:
    typename StrategyConst1<Return, Arg1>::Ptr strategy_;

    Arg1 arg1_;
};

template <typename Return, typename Arg1, typename Arg2>
class StrategyConst2Invoker : public StrategyConst0<Return>
{
public:
    StrategyConst2Invoker(const typename StrategyConst2<Return, Arg1, Arg2>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
    {}
    virtual ~StrategyConst2Invoker()
    {}

    CLASS_TRAITS(StrategyConst2Invoker)

    virtual Return Perform() const
    {
        return strategy_->Perform(arg1_, arg2_);
    }

private:
    typename StrategyConst2<Return, Arg1, Arg2>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class StrategyConst3Invoker : public StrategyConst0<Return>
{
public:
    StrategyConst3Invoker(const typename StrategyConst3<Return, Arg1, Arg2, Arg3>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    {}
    virtual ~StrategyConst3Invoker()
    {}

    CLASS_TRAITS(StrategyConst3Invoker)

    virtual Return Perform()
    {
        return strategy_->Perform(arg1_, arg2_, arg3_);
    }

private:
    typename StrategyConst3<Return, Arg1, Arg2, Arg3>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class StrategyConst4Invoker : public StrategyConst0<Return>
{
public:
    StrategyConst4Invoker(const typename StrategyConst4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    {}
    virtual ~StrategyConst4Invoker()
    {}

    CLASS_TRAITS(StrategyConst4Invoker)

    virtual Return Perform() const
    {
        return strategy_->Perform(arg1_, arg2_, arg3_, arg4_);
    }

private:
    typename StrategyConst4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class StrategyConst5Invoker : public StrategyConst0<Return>
{
public:
    StrategyConst5Invoker(const typename StrategyConst5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr &strategy, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
        : strategy_(strategy)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
        , arg5_(arg5)
    {}
    virtual ~StrategyConst5Invoker()
    {}

    CLASS_TRAITS(StrategyConst5Invoker)

    virtual Return Perform() const
    {
        return strategy_->Perform(arg1_, arg2_, arg3_, arg4_, arg5_);
    }

private:
    typename StrategyConst5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr strategy_;

    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
    Arg5 arg5_;
};

}}

#endif
