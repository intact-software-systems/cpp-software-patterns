#ifndef BaseLib_Templates_FunctionHolder_h_IsIncluded
#define BaseLib_Templates_FunctionHolder_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Debug.h"
#include "BaseLib/Templates/AnyType.h"
#include "BaseLib/Templates/BaseTypes.h"
#include "BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

// ----------------------------------------------------
// Base class for Functions
// ----------------------------------------------------

class Function
{
public:
    template <typename FunctionPointer>
    Function(FunctionPointer id)
        : any_(id)
    {}
    Function(Templates::Any any)
        : any_(any)
    {}
    virtual ~Function()
    {}

    CLASS_TRAITS(Function)

    Templates::Any Id() const
    {
        return any_;
    }

    template <typename FunctionPointer>
    FunctionPointer* Func() const
    {
        return Templates::AnyCast<FunctionPointer>(&any_);
    }

    bool operator==(const Function &rvalue) const
    {
        return this->any_ == rvalue.any_;
    }

    bool operator!=(const Function &rvalue) const
    {
        return this->any_ != rvalue.any_;
    }

private:
    Templates::Any any_;
};

// ----------------------------------------------------
// Function0
// ----------------------------------------------------

template <typename Class, typename Return>
class Function0
        : public Function
        , public OperatorMethod0<Return>
{
    typedef Return (Class::*Func)();

public:
    Function0(std::shared_ptr<Class> delegate, Func func)
        : Function(func)
        , delegate_(delegate)
        , func_(func)
        , rawdelegate_(delegate.get())
    { }

    virtual ~Function0() { }

    CLASS_TRAITS(Function0)

    virtual Return operator()()
    {
        if(delegate_)
        {
            return (rawdelegate_->*func_)();
        }
        ICRITICAL() << "Delegate is NULL";
        return Return();
    }

    std::shared_ptr<Class>  Delegate() const { return delegate_; }
    Func                    Funct() const { return func_; }

private:
    std::shared_ptr<Class>  delegate_;
    Func                    func_;
    Class                   *rawdelegate_;
};

// ----------------------------------------------------
// Function1
// ----------------------------------------------------
// class _Rp, class ..._ArgTypes
template <typename Class, typename Return, typename Arg1>
class Function1
        : public Function
        , public OperatorMethod1<Return, Arg1>
{
    typedef Return (Class::*Func)(Arg1);

public:
    Function1(std::shared_ptr<Class> delegate, Func func)
        : Function(func)
        , delegate_(delegate)
        , func_(func)
        , rawdelegate_(delegate.get())
    { }

    virtual ~Function1() { }

    CLASS_TRAITS(Function1)

    virtual Return operator()(Arg1 arg1)
    {
        if(delegate_)
        {
            return (rawdelegate_->*func_)(arg1);
        }

        ICRITICAL() << "Delegate is NULL";
        return Return();
    }

    std::shared_ptr<Class>  Delegate() const { return delegate_; }
    Func                    Funct() const { return func_; }

private:
    std::shared_ptr<Class>  delegate_;
    Func                    func_;
    Class                   *rawdelegate_;
};

// ----------------------------------------------------
// Function2
// ----------------------------------------------------

template <typename Class, typename Return, typename Arg1, typename Arg2>
class Function2
        : public Function
        , public OperatorMethod2<Return, Arg1, Arg2>
{
    typedef Return (Class::*Func)(Arg1, Arg2);

public:
    Function2(std::shared_ptr<Class> delegate, Func func)
        : Function(func)
        , delegate_(delegate)
        , func_(func)
        , rawdelegate_(delegate.get())
    { }

    virtual ~Function2() { }

    CLASS_TRAITS(Function2)

    virtual Return operator()(Arg1 arg1, Arg2 arg2)
    {
        if(delegate_)
        {
            return (rawdelegate_->*func_)(arg1, arg2);
        }

        ICRITICAL() << "Delegate is NULL";
        return Return();
    }

    std::shared_ptr<Class>  Delegate() const { return delegate_; }
    Func                    Funct() const { return func_; }

private:
    std::shared_ptr<Class>  delegate_;
    Func                    func_;
    Class                   *rawdelegate_;
};

// ----------------------------------------------------
// Function3
// ----------------------------------------------------

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3>
class Function3
        : public Function
        , public OperatorMethod3<Return, Arg1, Arg2, Arg3>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3);

public:
    Function3(std::shared_ptr<Class> delegate, Func func)
        : Function(func)
        , delegate_(delegate)
        , func_(func)
    { }

    virtual ~Function3() { }

    CLASS_TRAITS(Function3)

    virtual Return operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        if(delegate_)
        {
            return (delegate_.get()->*func_)(arg1, arg2, arg3);
        }
        ICRITICAL() << "Delegate is NULL";
        return Return();
    }

    std::shared_ptr<Class>  Delegate() const { return delegate_; }
    Func                    Funct() const { return func_; }

private:
    std::shared_ptr<Class>  delegate_;
    Func                    func_;
};

// ----------------------------------------------------
// Function4
// ----------------------------------------------------

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Function4
        : public Function
        , public OperatorMethod4<Return, Arg1, Arg2, Arg3, Arg4>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3, Arg4);

public:
    Function4(std::shared_ptr<Class> delegate, Func func)
        : Function(func)
        , delegate_(delegate)
        , func_(func)
    { }

    virtual ~Function4() { }

    CLASS_TRAITS(Function4)

    virtual Return operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        if(delegate_)
        {
            return (delegate_.get()->*func_)(arg1, arg2, arg3, arg4);
        }
        ICRITICAL() << "Delegate is NULL";
        return Return();
    }

    std::shared_ptr<Class>  Delegate() const { return delegate_; }
    Func                    Funct() const { return func_; }

private:
    std::shared_ptr<Class>  delegate_;
    Func                    func_;
};

// ----------------------------------------------------
// Function5
// ----------------------------------------------------

template <typename Class, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class Function5
        : public Function
        , public OperatorMethod5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
{
    typedef Return (Class::*Func)(Arg1, Arg2, Arg3, Arg4, Arg5);

public:
    Function5(std::shared_ptr<Class> delegate, Func func)
        : Function(func)
        , delegate_(delegate)
        , func_(func)
    { }

    virtual ~Function5() { }

    CLASS_TRAITS(Function5)

    virtual Return operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        if(delegate_)
        {
            return (delegate_.get()->*func_)(arg1, arg2, arg3, arg4, arg5);
        }

        ICRITICAL() << "Delegate is NULL";
        return Return();
    }

    std::shared_ptr<Class>  Delegate() const { return delegate_; }
    Func                    Funct() const { return func_; }

private:
    std::shared_ptr<Class>  delegate_;
    Func                    func_;
};

}}

#endif
