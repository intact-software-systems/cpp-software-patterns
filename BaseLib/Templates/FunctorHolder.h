/*
 * FunctorHolder.h
 *
 *  Created on: May 1, 2012
 *      Author: knuthelv
 */
#ifndef BaseLib_Templates_FunctorHolder_h_IsIncluded
#define BaseLib_Templates_FunctorHolder_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

/*----------------------------------------------------
 Base class for FunctorHolder
----------------------------------------------------*/
class Functor0 : public Templates::InvokeMethod0<void>
{
public:
    virtual ~Functor0() {}

    CLASS_TRAITS(Functor0)
};

/*----------------------------------------------------
 Null object for functor holder
----------------------------------------------------*/
class FunctorNullObject : public Functor0
{
public:
    virtual ~FunctorNullObject() { }

    virtual void Invoke() {}
};

/*----------------------------------------------------
 class FunctorInvoker0
----------------------------------------------------*/
template <typename FUN>
class FunctorInvoker0 : public Functor0
{
public:
    FunctorInvoker0(const FUN& func)
        : func_(func)
    { }

    virtual ~FunctorInvoker0()
    { }

    virtual void Invoke()
    {
        this->func_.operator()();
    }

private:
    FUN func_;
};

template <typename FUN>
class FunctorPtrInvoker0 : public Functor0
{
public:
    FunctorPtrInvoker0(FUN *func)
        : func_(func)
    { }

    virtual ~FunctorPtrInvoker0()
    { }

    virtual void Invoke()
    {
        func_->operator()();
    }

private:
    FUN *func_;
};

/*----------------------------------------------------
class FunctorInvoker1
----------------------------------------------------*/
template <typename FUN, typename ARG1>
class FunctorInvoker1 : public Functor0
{
public:
    FunctorInvoker1(const FUN& func, const ARG1& arg1)
        : func_(func)
        , arg1_(arg1)
    { }

    virtual ~FunctorInvoker1()
    { }

    virtual void Invoke()
    {
        func_(arg1_);
    }

private:
    FUN func_;
    ARG1 arg1_;
};

template <typename FUN, typename ARG1>
class FunctorPtrInvoker1 : public Functor0
{
public:
    FunctorPtrInvoker1(FUN *func, const ARG1 &arg1)
        : func_(func)
        , arg1_(arg1)
    { }

    virtual ~FunctorPtrInvoker1()
    { }

    virtual void Invoke()
    {
        func_->operator()(arg1_);
    }

private:
    FUN *func_;
    ARG1 arg1_;
};

/*----------------------------------------------------
class FunctorInvoker2
----------------------------------------------------*/
template <typename FUN, typename ARG1, typename ARG2>
class FunctorInvoker2 : public Functor0
{
public:
    FunctorInvoker2(const FUN& func, const ARG1& arg1, const ARG2& arg2)
        : func_(func)
        , arg1_(arg1)
        , arg2_(arg2)
    { }

    virtual ~FunctorInvoker2()
    { }

    virtual void Invoke()
    {
        func_(arg1_, arg2_);
    }

private:
    FUN func_;
    ARG1 arg1_;
    ARG2 arg2_;
};

template <typename FUN, typename ARG1, typename ARG2>
class FunctorPtrInvoker2 : public Functor0
{
public:
    FunctorPtrInvoker2(FUN* func, const ARG1 &arg1, const ARG2 &arg2)
        : func_(func)
        , arg1_(arg1)
        , arg2_(arg2)
    { }

    virtual ~FunctorPtrInvoker2()
    { }

    virtual void Invoke()
    {
        func_->operator()(arg1_, arg2_);
    }

private:
    FUN *func_;
    ARG1 arg1_;
    ARG2 arg2_;
};

/*----------------------------------------------------
class FunctorInvoker3
----------------------------------------------------*/
template <typename FUN, typename ARG1, typename ARG2, typename ARG3>
class FunctorInvoker3 : public Functor0
{
public:
    FunctorInvoker3(const FUN& func, const ARG1& arg1, const ARG2& arg2, const ARG3 &arg3)
        : func_(func)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    { }

    virtual ~FunctorInvoker3()
    { }

    virtual void Invoke()
    {
        func_(arg1_, arg2_, arg3_);
    }

private:
    FUN func_;
    ARG1 arg1_;
    ARG2 arg2_;
    ARG3 arg3_;
};

template <typename FUN, typename ARG1, typename ARG2, typename ARG3>
class FunctorPtrInvoker3 : public Functor0
{
public:
    FunctorPtrInvoker3(FUN* func, const ARG1 &arg1, const ARG2 &arg2, const ARG3 &arg3)
        : func_(func)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    { }

    virtual ~FunctorPtrInvoker3()
    { }

    virtual void Invoke()
    {
        func_->operator()(arg1_, arg2_, arg3_);
    }

private:
    FUN *func_;
    ARG1 arg1_;
    ARG2 arg2_;
    ARG3 arg3_;
};

}}

#endif
