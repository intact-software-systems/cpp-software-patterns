#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/Export.h"

namespace BaseLib {
namespace Concurrent {

class DLL_STATE ActionFactory
{
public:
    virtual ~ActionFactory() {}

    template <typename Return>
    static typename Templates::Action0<Return>::Ptr Create(const std::function<Return()>& func)
    {
        return std::make_shared<Templates::OperatorContextAction0<Return>>(
            Templates::ActionContext::Empty(),
            std::make_shared<Templates::OperatorFunction0<Return>>(func)
        );
    }

    template <typename Return, typename Arg1>
    static typename Templates::Action0<Return>::Ptr Create(const std::function<Return(Arg1)>& func, Arg1 arg1)
    {
        return std::make_shared<Templates::OperatorContextAction1<Return, Arg1>>(
            Templates::ActionContext::Empty(),
            std::make_shared<Templates::OperatorFunction1<Return, Arg1>>(func),
            arg1
        );
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename Templates::Action0<Return>::Ptr Create(const std::function<Return(Arg1, Arg2)>& func, Arg1 arg1, Arg2 arg2)
    {
        return std::make_shared<Templates::OperatorContextAction2<Return, Arg1, Arg2>>(
            Templates::ActionContext::Empty(),
            std::make_shared<Templates::OperatorFunction2<Return, Arg1, Arg2>>(func),
            arg1,
            arg2
        );
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename Templates::Action0<Return>::Ptr Create(const std::function<Return(Arg1, Arg2, Arg3)>& func, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        return std::make_shared<Templates::OperatorContextAction3<Return, Arg1, Arg2, Arg3>>(
            Templates::ActionContext::Empty(),
            std::make_shared<Templates::OperatorFunction3<Return, Arg1, Arg2, Arg3>>(func),
            arg1,
            arg2,
            arg3
        );
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename Templates::Action0<Return>::Ptr Create(const std::function<Return(Arg1, Arg2, Arg3, Arg4)>& func, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        return std::make_shared<Templates::OperatorContextAction4<Return, Arg1, Arg2, Arg3, Arg4>>(
            Templates::ActionContext::Empty(),
            std::make_shared<Templates::OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>>(func),
            arg1,
            arg2,
            arg3,
            arg4
        );
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename Templates::Action0<Return>::Ptr Create(const std::function<Return(Arg1, Arg2, Arg3, Arg4, Arg5)>& func, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        return std::make_shared<Templates::OperatorContextAction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>>(
            Templates::ActionContext::Empty(),
            std::make_shared<Templates::OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>>(func),
            arg1,
            arg2,
            arg3,
            arg4,
            arg5
        );
    }



    template <typename Return>
    static typename Templates::Action0<Return>::Ptr Create(std::shared_ptr<Templates::ActionContext> context, const std::function<Return()>& func)
    {
        return std::make_shared<Templates::OperatorContextAction0<Return>>(
            context,
            std::make_shared<Templates::OperatorFunction0<Return>>(func)
        );
    }

    template <typename Return, typename Arg1>
    static typename Templates::Action0<Return>::Ptr Create(std::shared_ptr<Templates::ActionContext> context, const std::function<Return(Arg1)>& func, Arg1 arg1)
    {
        return std::make_shared<Templates::OperatorContextAction1<Return, Arg1>>(
            context,
            std::make_shared<Templates::OperatorFunction1<Return, Arg1>>(func),
            arg1
        );
    }

    template <typename Return, typename Arg1, typename Arg2>
    static typename Templates::Action0<Return>::Ptr Create(std::shared_ptr<Templates::ActionContext> context, const std::function<Return(Arg1, Arg2)>& func, Arg1 arg1, Arg2 arg2)
    {
        return std::make_shared<Templates::OperatorContextAction2<Return, Arg1, Arg2>>(
            context,
            std::make_shared<Templates::OperatorFunction2<Return, Arg1, Arg2>>(func),
            arg1,
            arg2
        );
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3>
    static typename Templates::Action0<Return>::Ptr Create(std::shared_ptr<Templates::ActionContext> context, const std::function<Return(Arg1, Arg2, Arg3)>& func, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        return std::make_shared<Templates::OperatorContextAction3<Return, Arg1, Arg2, Arg3>>(
            context,
            std::make_shared<Templates::OperatorFunction3<Return, Arg1, Arg2, Arg3>>(func),
            arg1,
            arg2,
            arg3
        );
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static typename Templates::Action0<Return>::Ptr Create(std::shared_ptr<Templates::ActionContext> context, const std::function<Return(Arg1, Arg2, Arg3, Arg4)>& func, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        return std::make_shared<Templates::OperatorContextAction4<Return, Arg1, Arg2, Arg3, Arg4>>(
            context,
            std::make_shared<Templates::OperatorFunction4<Return, Arg1, Arg2, Arg3, Arg4>>(func),
            arg1,
            arg2,
            arg3,
            arg4
        );
    }

    template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static typename Templates::Action0<Return>::Ptr Create(std::shared_ptr<Templates::ActionContext> context, const std::function<Return(Arg1, Arg2, Arg3, Arg4, Arg5)>& func, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        return std::make_shared<Templates::OperatorContextAction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>>(
            context,
            std::make_shared<Templates::OperatorFunction5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>>(func),
            arg1,
            arg2,
            arg3,
            arg4,
            arg5
        );
    }

};

}
}
