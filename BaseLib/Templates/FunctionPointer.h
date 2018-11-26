#ifndef BaseLib_Templates_FunctionPointer_h_IsIncluded
#define BaseLib_Templates_FunctionPointer_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{
/*----------------------------------------------------
 Forward declarations
----------------------------------------------------*/

template <typename Class, typename Return>
class FunctionPointer0;

template <typename Class, typename Return, typename Arg1>
class FunctionPointer1;

template <typename Class, typename Return, typename Arg1, typename Arg2>
class FunctionPointer2;

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3>
class FunctionPointer3;

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FunctionPointer4;


template <typename Class, typename Return>
class FunctionPointerInvoker0;

template <typename Class, typename Return, typename Arg1>
class FunctionPointerInvoker1;

template <typename Class, typename Return, typename Arg1, typename Arg2>
class FunctionPointerInvoker2;

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3>
class FunctionPointerInvoker3;

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FunctionPointerInvoker4;


/*----------------------------------------------------
 Base class for FunctionPointer
----------------------------------------------------*/
template <typename Class, typename Return>
class FunctionPointerBase
{
public:
    virtual ~FunctionPointerBase() {}

    CLASS_TRAITS(FunctionPointerBase)

public:
    typedef Return (Class::*Func)();
};

/*----------------------------------------------------
 Base class for FunctionPointer
----------------------------------------------------*/
template <typename Class, typename Return>
class FunctionPointer
        : public FunctionPointerBase<Class, Return>
        , public DelegateMethod<Class*>
//        , public FunctionMethod< typename FunctionPointerBase<Class, Return>::Func >
{
public:
    virtual ~FunctionPointer() { }

    CLASS_TRAITS(FunctionPointer)

public:
    bool IsFunctionPointer0() const
    {
        return dynamic_cast<Templates::FunctionPointer0<Class, Return> *> (this) != NULL;
    }

    template <typename Arg1>
    bool IsFunctionPointer1() const
    {
        return dynamic_cast<Templates::FunctionPointer1<Class, Return, Arg1> *> (this) != NULL;
    }

    template <typename Arg1, typename Arg2>
    bool IsFunctionPointer2() const
    {
        return dynamic_cast<Templates::FunctionPointer2<Class, Return, Arg1, Arg2> *> (this) != NULL;
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    bool IsFunctionPointer3() const
    {
        return dynamic_cast<Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3> *> (this) != NULL;
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    bool IsFunctionPointer4() const
    {
        return dynamic_cast<Templates::FunctionPointer4<Class, Return, Arg1, Arg2, Arg3, Arg4> *> (this) != NULL;
    }

public:
    bool IsFunctionPointerInvoker0() const
    {
        return dynamic_cast<Templates::FunctionPointerInvoker0<Class, Return> *> (this) != NULL;
    }

    template <typename Arg1>
    bool IsFunctionPointerInvoker1() const
    {
        return dynamic_cast<Templates::FunctionPointerInvoker1<Class, Return, Arg1> *> (this) != NULL;
    }

    template <typename Arg1, typename Arg2>
    bool IsFunctionPointerInvoker2() const
    {
        return dynamic_cast<Templates::FunctionPointerInvoker2<Class, Return, Arg1, Arg2> *> (this) != NULL;
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    bool IsFunctionPointerInvoker3() const
    {
        return dynamic_cast<Templates::FunctionPointerInvoker3<Class, Return, Arg1, Arg2, Arg3> *> (this) != NULL;
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    bool IsFunctionPointerInvoker4() const
    {
        return dynamic_cast<Templates::FunctionPointerInvoker4<Class, Return, Arg1, Arg2, Arg3, Arg4> *> (this) != NULL;
    }

//public:
//    bool IsOfTypeInstance(Class* action) const
//    {
//        return DelegateMethod<Class*>::Delegate() == action;
//    }

//    bool IsOfTypeInstance(Class* action, Return (Class::*member)()) const
//    {
//        return DelegateMethod<Class*>::Delegate() == action && FunctionMethod<typename FunctionPointerBase<Class, Return>::Func>::Function() == member;
//    }

//    bool IsOfTypeFunction(Return (Class::*member)()) const
//    {
//        return FunctionMethod<typename FunctionPointerBase<Class, Return>::Func>::Function() == member;
//    }
};

// --------------------------------------------------------------------
// Derived class - parametrise the implementation so we can specify the
// target class in client code.
// --------------------------------------------------------------------
template <typename Class, typename Return>
class FunctionPointer0
        : public InvokeMethod0<Return>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)();

public:
    FunctionPointer0(Class* aThis, Func aFunc)
        : delegate_(aThis)
        , function_(aFunc)
    { }

    virtual ~FunctionPointer0() { }

    CLASS_TRAITS(FunctionPointer0)

    virtual Return Invoke()
    {
        return (delegate_->*function_)();
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    Func    Function() const { return function_; }

private:
    Class* delegate_;
    Func   function_;
};

template <typename Class, typename Return, typename Arg1>
class FunctionPointer1
        : public InvokeMethod1<Return, Arg1>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1);

public:
    FunctionPointer1(Class* aThis, Func aFunc)
        : delegate_(aThis)
        , function_(aFunc)
    { }

    virtual ~FunctionPointer1() { }

    CLASS_TRAITS(FunctionPointer1)

    virtual Return Invoke(Arg1 arg1)
    {
        return (delegate_->*function_)(arg1);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class* delegate_;
    Func   function_;
};

template <typename Class, typename Return, typename Arg1, typename Arg2>
class FunctionPointer2
        : public InvokeMethod2<Return, Arg1, Arg2>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1, Arg2);

public:
    FunctionPointer2(Class* aThis, Func aFunc)
        : delegate_(aThis)
        , function_(aFunc)
    { }

    virtual ~FunctionPointer2() { }

    CLASS_TRAITS(FunctionPointer2)

    virtual Return Invoke(Arg1 arg1, Arg2 arg2)
    {
        return (delegate_->*function_)(arg1, arg2);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class* delegate_;
    Func   function_;
};

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3>
class FunctionPointer3
        : public InvokeMethod3<Return, Arg1, Arg2, Arg3>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3);

public:
    FunctionPointer3(Class* aThis, Func aFunc)
        : delegate_(aThis)
        , function_(aFunc)
    { }

    virtual ~FunctionPointer3() { }

    CLASS_TRAITS(FunctionPointer3)

    virtual Return Invoke(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        return (delegate_->*function_)(arg1, arg2, arg3);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class* delegate_;
    Func   function_;
};

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FunctionPointer4
        : public InvokeMethod4<Return, Arg1, Arg2, Arg3, Arg4>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3, Arg4);

public:
    FunctionPointer4(Class* aThis, Func aFunc)
        : delegate_(aThis)
        , function_(aFunc)
    { }

    virtual ~FunctionPointer4() { }

    CLASS_TRAITS(FunctionPointer4)

    virtual Return Invoke(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        return (delegate_->*function_)(arg1, arg2, arg3, arg4);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class* delegate_;
    Func   function_;
};

// --------------------------------------------------------------------
// Derived class - parametrise the implementation so we can specify the
// target class in client code.
// --------------------------------------------------------------------
template <typename Class, typename Return>
class FunctionPointerInvoker0
        : public InvokeMethod0<Return>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)();

public:
    FunctionPointerInvoker0(Class* aThis, Func aFunc)
        : delegate_(aThis)
        , function_(aFunc)
    { }

    virtual ~FunctionPointerInvoker0() { }

    CLASS_TRAITS(FunctionPointerInvoker0)

    virtual Return Invoke()
    {
        return (delegate_->*function_)();
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class*  delegate_;
    Func    function_;
};

template <typename Class, typename Return, typename Arg1>
class FunctionPointerInvoker1
        : public InvokeMethod0<Return>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1);

public:
    FunctionPointerInvoker1(Class* aThis, Func aFunc, Arg1 arg1)
        : delegate_(aThis)
        , function_(aFunc)
        , arg1_(arg1)
    { }

    virtual ~FunctionPointerInvoker1() { }

    CLASS_TRAITS(FunctionPointerInvoker1)

    virtual Return Invoke()
    {
        return (delegate_->*function_)(arg1_);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class*  delegate_;
    Func    function_;
    Arg1    arg1_;
};

template <typename Class, typename Return, typename Arg1, typename Arg2>
class FunctionPointerInvoker2
        : public InvokeMethod0<Return>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1, Arg2);

public:
    FunctionPointerInvoker2(Class* aThis, Func aFunc, Arg1 arg1, Arg2 arg2)
        : delegate_(aThis)
        , function_(aFunc)
        , arg1_(arg1)
        , arg2_(arg2)
    { }

    virtual ~FunctionPointerInvoker2() { }

    CLASS_TRAITS(FunctionPointerInvoker2)

    virtual Return Invoke()
    {
        return (delegate_->*function_)(arg1_, arg2_);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class*  delegate_;
    Func    function_;
    Arg1    arg1_;
    Arg2    arg2_;
};

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3>
class FunctionPointerInvoker3
        : public InvokeMethod0<Return>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3);

public:
    FunctionPointerInvoker3(Class* aThis, Func aFunc, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : delegate_(aThis)
        , function_(aFunc)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    { }

    virtual ~FunctionPointerInvoker3() { }

    CLASS_TRAITS(FunctionPointerInvoker3)

    virtual Return Invoke()
    {
        return (delegate_->*function_)(arg1_, arg2_, arg3_);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class*  delegate_;
    Func    function_;
    Arg1    arg1_;
    Arg2    arg2_;
    Arg3    arg3_;
};

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FunctionPointerInvoker4
        : public InvokeMethod0<Return>
        , public FunctionPointer<Class, Return>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3, Arg4);

public:
    FunctionPointerInvoker4(Class* aThis, Func aFunc, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : delegate_(aThis)
        , function_(aFunc)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    { }

    virtual ~FunctionPointerInvoker4() { }

    CLASS_TRAITS(FunctionPointerInvoker4)

    virtual Return Invoke()
    {
        return (delegate_->*function_)(arg1_, arg2_, arg3_, arg4_);
    }

public:
    virtual Class*  Delegate() const { return delegate_; }
    virtual Func    Function() const { return function_; }

private:
    Class*  delegate_;
    Func    function_;
    Arg1    arg1_;
    Arg2    arg2_;
    Arg3    arg3_;
    Arg4    arg4_;
};

}}

#endif
