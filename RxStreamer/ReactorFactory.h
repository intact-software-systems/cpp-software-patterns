#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessPolicy.h"
#include"RxStreamer/DataAccessControllerPolicy.h"
#include"RxStreamer/DataAccess.h"
#include"RxStreamer/DataAccessGroupChain.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DataAccessController;

// ---------------------------------------------------
// FactoryConfig
// ---------------------------------------------------

class DLL_STATE FactoryConfig
{};

// ---------------------------------------------------
// FactoryState
// ---------------------------------------------------

class DLL_STATE FactoryState
{
public:
    FactoryState() {}
    virtual ~FactoryState() {}

    //ReactorConfig::CommandControllerAccess executors() const;
};

// ---------------------------------------------------
// ReactorFactory
// ---------------------------------------------------

class DLL_STATE ReactorFactory
        : public Templates::FactorySingleton<ReactorFactory>
        , public Templates::ContextObject<FactoryConfig, FactoryState>
{
public:
    ReactorFactory() {}
    virtual ~ReactorFactory() {}

    template <typename K, typename StateType>
    typename RxScheduler<K, StateType>::Ptr CreateRxScheduler()
    {
        return RxScheduler<K, StateType>::Ptr( new RxScheduler<K, StateType>());
    }

    std::shared_ptr<DataAccessController> Create(DataAccessControllerPolicy policy, DataAccessGroupChain chain, RxData::CacheDescription cacheDescription);

    template <typename K, typename V>
    typename DataAccess<K,V>::Ptr Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action0<IMap<K, V>>::Ptr action)
    {
        typename CommandActions<IMap<K,V>>::Ptr command = CommandFactory::CreateCommand<IMap<K,V>>(CommandPolicy::Default(), action);
        command->Add(action);

        CommandsGroup group;
        group.Add(command);

        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(group);
        return access->GetPtr();
    }

    template <typename K, typename V, typename Arg1>
    typename DataAccess<K,V>::Ptr Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action1<IMap<K, V>, Arg1>::Ptr action, Arg1 arg1)
    {
        typename CommandActions<IMap<K,V>>::Ptr command = CommandFactory::CreateCommand<IMap<K,V>>(CommandPolicy::Default(), action);
        command->template Add<Arg1>(action, arg1);

        CommandsGroup group;
        group.Add(command);

        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(group);
        return access->GetPtr();
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    typename DataAccess<K,V>::Ptr Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action2<IMap<K, V>, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        typename CommandActions<IMap<K,V>>::Ptr command = CommandFactory::CreateCommand<IMap<K,V>>(CommandPolicy::Default(), action);
        command->template Add<Arg1, Arg2>(action, arg1, arg2);

        CommandsGroup group;
        group.Add(command);

        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(group);
        return access->GetPtr();
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    typename DataAccess<K,V>::Ptr Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typename CommandActions<IMap<K,V>>::Ptr command = CommandFactory::CreateCommand<IMap<K,V>>(CommandPolicy::Default(), action);
        command->template Add<Arg1, Arg2, Arg3>(action, arg1, arg2, arg3);

        CommandsGroup group;
        group.Add(command);

        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(group);
        return access->GetPtr();
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    typename DataAccess<K,V>::Ptr Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        typename CommandActions<IMap<K,V>>::Ptr command = CommandFactory::CreateCommand<IMap<K,V>>(CommandPolicy::Default(), action);
        command->template Add<Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4);

        CommandsGroup group;
        group.Add(command);

        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(group);

        return access->GetPtr();
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    typename DataAccess<K,V>::Ptr Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        typename CommandActions<IMap<K,V>>::Ptr command = CommandFactory::CreateCommand<IMap<K,V>>(CommandPolicy::Default(), action);
        command->template Add<Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5);

        CommandsGroup group;
        group.Add(command);

        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(group);
        return access->GetPtr();
    }

    template <typename K, typename V>
    typename DataAccess<K,V>::Ptr Create(
            RxData::CacheDescription masterCacheId,
            DataAccessPolicy rxPolicy,
            CommandsGroup commands)
    {
        typename DataAccess<K,V>::Ptr access(new DataAccess<K,V>(masterCacheId, rxPolicy, RxThreadPoolFactory::ControllerPool()));
        access->SetCommands(commands);
        return access->GetPtr();
    }
};

}
