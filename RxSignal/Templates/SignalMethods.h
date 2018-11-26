#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Export.h"

namespace BaseLib { namespace Templates {

// -----------------------------------------------------
// Signalling classes
// -----------------------------------------------------

class DLL_STATE Signal : public Runnable
{
public:
    virtual ~Signal()
    { }

    CLASS_TRAITS(Signal)
};


template <typename SIGNALLER>
class Signal0 : public Signal
{
public:
    Signal0(SIGNALLER signaller)
        : signaller_(signaller)
    { }

    virtual ~Signal0()
    {
        IDEBUG() << "Destructed signaller";
    }

    CLASS_TRAITS(Signal0)

    virtual void run()
    {
        signaller_->Signal();
    }

private:
    SIGNALLER signaller_;
};

template <typename SIGNALLER, typename Arg1>
class Signal1 : public Signal
{
public:
    Signal1(SIGNALLER signaller, Arg1 arg1)
        : signaller_(signaller)
        , arg1_(arg1)
    { }

    virtual ~Signal1()
    {
        IDEBUG() << "Destructed signaller";
    }

    CLASS_TRAITS(Signal1)

    virtual void run()
    {
        signaller_->Signal(arg1_);
    }

private:
    SIGNALLER signaller_;
    Arg1      arg1_;
};

template <typename SIGNALLER, typename Arg1, typename Arg2>
class Signal2 : public Signal
{
public:
    Signal2(SIGNALLER signaller, Arg1 arg1, Arg2 arg2)
        : signaller_(signaller)
        , arg1_(arg1)
        , arg2_(arg2)
    { }

    virtual ~Signal2()
    {
        IDEBUG() << "Destructed signaller";
    }

    CLASS_TRAITS(Signal2)

    virtual void run()
    {
        signaller_->Signal(arg1_, arg2_);
    }

private:
    SIGNALLER signaller_;
    Arg1      arg1_;
    Arg2      arg2_;
};


template <typename SIGNALLER, typename Arg1, typename Arg2, typename Arg3>
class Signal3 : public Signal
{
public:
    Signal3(SIGNALLER signaller, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : signaller_(signaller)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    { }

    virtual ~Signal3()
    {
        IDEBUG() << "Destructed signaller";
    }

    CLASS_TRAITS(Signal3)

    virtual void run()
    {
        signaller_->Signal(arg1_, arg2_, arg3_);
    }

private:
    SIGNALLER signaller_;
    Arg1      arg1_;
    Arg2      arg2_;
    Arg3      arg3_;
};

// -----------------------------------------------------
// Signalling classes
// -----------------------------------------------------

template <typename Return, typename Class, typename SIGNALLER>
class FunctionSignal0 : public Signal
{
    typedef Return (Class::*Func)();

public:
    FunctionSignal0(SIGNALLER signaller, Return (Class::*member)())
        : signaller_(signaller)
        , func_(member)
    { }

    virtual ~FunctionSignal0()
    { }

    CLASS_TRAITS(FunctionSignal0)

    virtual void run()
    {
        signaller_->template Signal<Class>(func_);
    }

private:
    SIGNALLER signaller_;
    Func      func_;
};

template <typename Return, typename Class, typename SIGNALLER, typename Arg1>
class FunctionSignal1 : public Signal
{
    typedef Return (Class::*Func)(Arg1);

public:
    FunctionSignal1(SIGNALLER signaller, Return (Class::*member)(Arg1), Arg1 arg1)
        : signaller_(signaller)
        , func_(member)
        , arg1_(arg1)
    { }

    virtual ~FunctionSignal1()
    { }

    CLASS_TRAITS(FunctionSignal1)

    virtual void run()
    {
        signaller_->template Signal<Class>(arg1_, func_);
    }

private:
    SIGNALLER signaller_;
    Func      func_;
    Arg1      arg1_;
};

template <typename Return, typename Class, typename SIGNALLER, typename Arg1, typename Arg2>
class FunctionSignal2 : public Signal
{
    typedef Return (Class::*Func)(Arg1, Arg2);

public:
    FunctionSignal2(SIGNALLER signaller, Return (Class::*member)(Arg1, Arg2), Arg1 arg1, Arg2 arg2)
        : signaller_(signaller)
        , func_(member)
        , arg1_(arg1)
        , arg2_(arg2)
    { }

    virtual ~FunctionSignal2()
    { }

    CLASS_TRAITS(FunctionSignal2)

    virtual void run()
    {
        signaller_->template Signal<Class>(arg1_, arg2_, func_);
    }

private:
    SIGNALLER signaller_;
    Func      func_;
    Arg1      arg1_;
    Arg2      arg2_;
};

template <typename Return, typename Class, typename SIGNALLER, typename Arg1, typename Arg2, typename Arg3>
class FunctionSignal3 : public Signal
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3);

public:
    FunctionSignal3(SIGNALLER signaller, Return (Class::*member)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : signaller_(signaller)
        , func_(member)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    { }

    virtual ~FunctionSignal3()
    { }

    CLASS_TRAITS(FunctionSignal3)

    virtual void run()
    {
        signaller_->template Signal<Class>(arg1_, arg2_, arg3_, func_);
    }

private:
    SIGNALLER signaller_;
    Func      func_;
    Arg1      arg1_;
    Arg2      arg2_;
    Arg3      arg3_;
};

}}

