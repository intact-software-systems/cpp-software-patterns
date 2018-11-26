#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessGroup.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE RxDataAccessGroup
{
public:
    RxDataAccessGroup(Templates::ActionContextPtr actionContext, DataAccessGroup group);
    virtual ~RxDataAccessGroup();

    // --------------------------------------------------
    // Factory functions
    // --------------------------------------------------

    static RxDataAccessGroup Sequential(Templates::ActionContextPtr actionContext);
    static RxDataAccessGroup Parallel(Templates::ActionContextPtr actionContext);

    // --------------------------------------------------
    // Access to group
    // --------------------------------------------------

    DataAccessGroup Group();

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template<typename K, typename V>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            typename Templates::Strategy0<IMap<K, V>>::Ptr strategy)
    {
        typename Templates::StrategyContextAction0<IMap<K, V>>::Ptr contextStrategy(
                new Templates::StrategyContextAction0<IMap<K, V>>(
                        actionContext_,
                        strategy
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            typename Templates::Strategy1<IMap<K, V>, Arg1>::Ptr strategy,
            Arg1 arg1)
    {
        typename Templates::StrategyContextAction1<IMap<K, V>, Arg1>::Ptr contextStrategy(
                new Templates::StrategyContextAction1<IMap<K, V>, Arg1>(
                        actionContext_,
                        strategy,
                        arg1
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            typename Templates::Strategy2<IMap<K, V>, Arg1, Arg2>::Ptr strategy,
            Arg1 arg1,
            Arg2 arg2)
    {
        typename Templates::StrategyContextAction2<IMap<K, V>, Arg1, Arg2>::Ptr contextStrategy(
                new Templates::StrategyContextAction2<IMap<K, V>, Arg1, Arg2>(
                        actionContext_,
                        strategy,
                        arg1,
                        arg2
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            typename Templates::Strategy3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr strategy,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3)
    {
        typename Templates::StrategyContextAction3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr contextStrategy(
                new Templates::StrategyContextAction3<IMap<K, V>, Arg1, Arg2, Arg3>(
                        actionContext_,
                        strategy,
                        arg1,
                        arg2,
                        arg3
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            typename Templates::Strategy4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr strategy,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3,
            Arg4 arg4)
    {
        typename Templates::StrategyContextAction4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr contextStrategy(
                new Templates::StrategyContextAction4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>(
                        actionContext_,
                        strategy,
                        arg1,
                        arg2,
                        arg3,
                        arg4
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            typename Templates::Strategy5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr strategy,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3,
            Arg4 arg4,
            Arg5 arg5)
    {
        typename Templates::StrategyContextAction5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr contextStrategy(
                new Templates::StrategyContextAction5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>(
                        actionContext_,
                        strategy,
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                        arg5
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template<typename K, typename V>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            const std::function<IMap<K,V> ()> &func)
    {
        auto op = std::make_shared<Templates::OperatorFunction0<IMap<K,V>>>(func);

        typename Templates::OperatorContextAction0<IMap<K, V>>::Ptr contextStrategy(
                new Templates::OperatorContextAction0<IMap<K, V>>(
                        actionContext_,
                        op
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            const std::function<IMap<K,V> (Arg1)> &func,
            Arg1 arg1)
    {
        auto op = std::make_shared<Templates::OperatorFunction1<IMap<K,V>, Arg1>>(func);

        typename Templates::OperatorContextAction1<IMap<K, V>, Arg1>::Ptr contextStrategy(
                new Templates::OperatorContextAction1<IMap<K, V>, Arg1>(
                        actionContext_,
                        op,
                        arg1
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            const std::function<IMap<K,V> (Arg1, Arg2)> &strategy,
            Arg1 arg1,
            Arg2 arg2)
    {
        auto op = std::make_shared<Templates::OperatorFunction2<IMap<K,V>, Arg1, Arg2>>(strategy);

        typename Templates::OperatorContextAction2<IMap<K, V>, Arg1, Arg2>::Ptr contextStrategy(
                new Templates::OperatorContextAction2<IMap<K, V>, Arg1, Arg2>(
                        actionContext_,
                        op,
                        arg1,
                        arg2
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            const std::function<IMap<K, V> (Arg1, Arg2, Arg3)> &strategy,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3)
    {
        auto op = std::make_shared<Templates::OperatorFunction3<IMap<K,V>, Arg1, Arg2, Arg3>>(strategy);

        typename Templates::OperatorContextAction3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr contextStrategy(
                new Templates::OperatorContextAction3<IMap<K, V>, Arg1, Arg2, Arg3>(
                        actionContext_,
                        op,
                        arg1,
                        arg2,
                        arg3
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            const std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4)> &strategy,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3,
            Arg4 arg4)
    {
        auto op = std::make_shared<Templates::OperatorFunction4<IMap<K,V>, Arg1, Arg2, Arg3, Arg4>>(strategy);

        typename Templates::OperatorContextAction4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr contextStrategy(
                new Templates::OperatorContextAction4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>(
                        actionContext_,
                        op,
                        arg1,
                        arg2,
                        arg3,
                        arg4
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

    template<typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    typename DataAccess<K, V>::Ptr Chain(
            RxData::CacheDescription description,
            DataAccessPolicy policy,
            const std::function<IMap<K,V> (Arg1, Arg2, Arg3, Arg4, Arg5)> &strategy,
            Arg1 arg1,
            Arg2 arg2,
            Arg3 arg3,
            Arg4 arg4,
            Arg5 arg5)
    {
        auto op = std::make_shared<Templates::OperatorFunction5<IMap<K,V>, Arg1, Arg2, Arg3, Arg4, Arg5>>(strategy);

        typename Templates::OperatorContextAction5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr contextStrategy(
                new Templates::OperatorContextAction5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>(
                        actionContext_,
                        op,
                        arg1,
                        arg2,
                        arg3,
                        arg4,
                        arg5
                )
        );

        return group_.Chain<K, V>(description, policy, contextStrategy);
    }

private:
    const Templates::ActionContextPtr actionContext_;
    DataAccessGroup group_;
};

}
