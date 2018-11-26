#ifndef BaseLib_Templates_ActionHolder_h_IsIncluded
#define BaseLib_Templates_ActionHolder_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

// ------------------------------------------------
// An action context
// ------------------------------------------------

class DLL_STATE ActionContext
        : public Templates::EntryMethod
        , public Templates::ExitMethod
{
public:
    virtual ~ActionContext() {}

    virtual void Entry() { }
    virtual void Exit() { }

    static std::shared_ptr<ActionContext> Empty()
    {
        static auto instance = std::make_shared<ActionContext>();
        return instance;
    }
};

typedef std::shared_ptr<ActionContext> ActionContextPtr;

// ------------------------------------------------
// Action base class
// ------------------------------------------------

/**
 * TODO: Include IsDone? Do I need a policy to check if it is done?
 */
class DLL_STATE Action
        : public Templates::IsSuccessMethod
        , public Templates::IsDoneMethod
        , public Templates::CancelMethod
        , public Templates::EntryMethod
        , public Templates::ExitMethod
{
public:
    virtual ~Action() {}

    CLASS_TRAITS(Action)

    virtual void Entry() { }
    virtual void Exit() { }
};

// ------------------------------------------------
// Action base classes to be implemented
// ------------------------------------------------

template <typename Return>
class Action0
        : public Action
        , public Templates::InvokeMethod0<Return>
{
public:
    virtual ~Action0() {}

    CLASS_TRAITS(Action0)
};

template <typename Return, typename Arg1>
class Action1
        : public Action
        , public Templates::InvokeMethod1<Return, Arg1>
{
public:
    virtual ~Action1() {}

    CLASS_TRAITS(Action1)
};

template <typename Return, typename Arg1, typename Arg2>
class Action2
        : public Action
        , public Templates::InvokeMethod2<Return, Arg1, Arg2>
{
public:
    virtual ~Action2() {}

    CLASS_TRAITS(Action2)
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class Action3
        : public Action
        , public Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3>
{
public:
    virtual ~Action3() {}

    CLASS_TRAITS(Action3)
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Action4
        : public Action
        , public Templates::InvokeMethod4<Return, Arg1, Arg2, Arg3, Arg4>
{
public:
    virtual ~Action4() {}

    CLASS_TRAITS(Action4)
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class Action5
        : public Action
        , public Templates::InvokeMethod5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
{
public:
    virtual ~Action5() {}

    CLASS_TRAITS(Action5)
};

// ------------------------------------------------
// Action invokers/decorators to hide parameterized Actions
// ------------------------------------------------

template <typename Return>
class Action0Invoker : public Action0<Return>
{
public:
    Action0Invoker(const typename Action0<Return>::Ptr &action)
        : action_(action)
    {}
    virtual ~Action0Invoker()
    {}

    CLASS_TRAITS(Action0Invoker)

    virtual Return Invoke()
    {
        return action_->Invoke();
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

private:
    typename Action0<Return>::Ptr action_;
};

template <typename Return, typename Arg1>
class Action1Invoker : public Action0<Return>
{
public:
    Action1Invoker(
            const typename Action1<Return, Arg1>::Ptr &action,
            Arg1 arg1)
        : action_(action)
        , arg1_(arg1)
    {}
    virtual ~Action1Invoker()
    {}

    CLASS_TRAITS(Action1Invoker)

    virtual Return Invoke()
    {
        return action_->Invoke(arg1_);
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

private:
    typename Action1<Return, Arg1>::Ptr action_;
    Arg1 arg1_;
};

template <typename Return, typename Arg1, typename Arg2>
class Action2Invoker : public Action0<Return>
{
public:
    Action2Invoker(
            const typename Action2<Return, Arg1, Arg2>::Ptr &action,
            Arg1 arg1,
            Arg2 arg2)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
    {}
    virtual ~Action2Invoker()
    {}

    CLASS_TRAITS(Action2Invoker)

    virtual Return Invoke()
    {
        return action_-> template Invoke(arg1_, arg2_);
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

private:
    typename Action2<Return, Arg1, Arg2>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class Action3Invoker : public Action0<Return>
{
public:
    Action3Invoker(
            const typename Action3<Return, Arg1, Arg2, Arg3>::Ptr &action,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    {}
    virtual ~Action3Invoker()
    {}

    CLASS_TRAITS(Action3Invoker)

    virtual Return Invoke()
    {
        return action_-> template Invoke(arg1_, arg2_, arg3_);
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

private:
    typename Action3<Return, Arg1, Arg2, Arg3>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Action4Invoker : public Action0<Return>
{
public:
    Action4Invoker(
            const typename Action4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr &action,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3,
            Arg4 arg4)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    {}
    virtual ~Action4Invoker()
    {}

    CLASS_TRAITS(Action4Invoker)

    virtual Return Invoke()
    {
        return action_-> template Invoke(arg1_, arg2_, arg3_, arg4_);
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

private:
    typename Action4<Return, Arg1, Arg2, Arg3, Arg4>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class Action5Invoker : public Action0<Return>
{
public:
    Action5Invoker(
            const typename Action5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr &action,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3,
            Arg4 arg4,
            Arg5 arg5)
        : action_(action)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
        , arg5_(arg5)
    {}
    virtual ~Action5Invoker()
    {}

    CLASS_TRAITS(Action5Invoker)

    virtual Return Invoke()
    {
        return action_-> template Invoke(arg1_, arg2_, arg3_, arg4_, arg5_);
    }

    virtual void Exit()
    {
        action_->Exit();
    }

    virtual void Entry()
    {
        action_->Entry();
    }

    virtual bool Cancel()
    {
        return action_->Cancel();
    }

    virtual bool IsSuccess() const
    {
        return action_->IsSuccess();
    }

    virtual bool IsDone() const
    {
        return action_->IsDone();
    }

private:
    typename Action5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
    Arg4 arg4_;
    Arg5 arg5_;
};

}}

#endif
