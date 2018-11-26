#pragma once

#include<RxStreamer/DataAccessControllerPolicy.h>
#include"RxStreamer/RxDataAccessGroup.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE StreamerGroup
{
public:
    StreamerGroup(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, RxDataAccessGroup group);
    virtual ~StreamerGroup();

    // --------------------------------------
    // factory functions
    // --------------------------------------

    static StreamerGroup Create(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, RxDataAccessGroup group);

    // --------------------------------------
    // getters
    // --------------------------------------

    RxData::CacheDescription    CacheId() const;
    DataAccessPolicy            Policy() const;
    RxDataAccessGroup           Group() const;

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template <typename K, typename V>
    StreamerGroup& Chain(std::function<IMap<K, V> ()> strategy)
    {
        group_.Chain<K,V>(this->masterCacheId_, this->dataAccessPolicy_, strategy);
        return *this;
    }

    template <typename K, typename V, typename Arg1>
    StreamerGroup& Chain(Arg1 arg1, std::function<IMap<K, V> (Arg1)> strategy)
    {
        group_.Chain<K,V,Arg1>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, std::function<IMap<K, V> (Arg1, Arg2)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, Arg3 arg3, std::function<IMap<K, V> (Arg1, Arg2, Arg3)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2, arg3);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4, Arg5)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4, Arg5>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4, arg5);
        return *this;
    }

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template <typename K, typename V>
    StreamerGroup& Chain(typename Templates::Strategy0<IMap<K, V>>::Ptr strategy)
    {
        group_.Chain<K,V>(this->masterCacheId_, this->dataAccessPolicy_, strategy);
        return *this;
    }

    template <typename K, typename V, typename Arg1>
    StreamerGroup& Chain(Arg1 arg1, typename Templates::Strategy1<IMap<K, V>, Arg1>::Ptr strategy)
    {
        group_.Chain<K,V,Arg1>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, typename Templates::Strategy2<IMap<K, V>, Arg1, Arg2>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, Arg3 arg3, typename Templates::Strategy3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2, arg3);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, typename Templates::Strategy4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    StreamerGroup& Chain(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, typename Templates::Strategy5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4, Arg5>(this->masterCacheId_, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4, arg5);
        return *this;
    }

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template <typename K, typename V>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, std::function<IMap<K, V> ()> strategy)
    {
        group_.Chain<K,V>(masterCacheId, this->dataAccessPolicy_, strategy);
        return *this;
    }

    template <typename K, typename V, typename Arg1>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, std::function<IMap<K, V> (Arg1)> strategy)
    {
        group_.Chain<K,V,Arg1>(masterCacheId, this->dataAccessPolicy_, strategy, arg1);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, std::function<IMap<K, V> (Arg1, Arg2)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, Arg3 arg3, std::function<IMap<K, V> (Arg1, Arg2, Arg3)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2, arg3);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4, Arg5)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4, Arg5>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4, arg5);
        return *this;
    }


    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template <typename K, typename V>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, typename Templates::Strategy0<IMap<K, V>>::Ptr strategy)
    {
        group_.Chain<K,V>(masterCacheId, this->dataAccessPolicy_, strategy);
        return *this;
    }

    template <typename K, typename V, typename Arg1>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, typename Templates::Strategy1<IMap<K, V>, Arg1>::Ptr strategy)
    {
        group_.Chain<K,V,Arg1>(masterCacheId, this->dataAccessPolicy_, strategy, arg1);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, typename Templates::Strategy2<IMap<K, V>, Arg1, Arg2>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, Arg3 arg3, typename Templates::Strategy3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2, arg3);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, typename Templates::Strategy4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, typename Templates::Strategy5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4, Arg5>(masterCacheId, this->dataAccessPolicy_, strategy, arg1, arg2, arg3, arg4, arg5);
        return *this;
    }

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template <typename K, typename V>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, std::function<IMap<K, V> ()> strategy)
    {
        group_.Chain<K,V>(masterCacheId, dataAccessPolicy, strategy);
        return *this;
    }

    template <typename K, typename V, typename Arg1>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, std::function<IMap<K, V> (Arg1)> strategy)
    {
        group_.Chain<K,V,Arg1>(masterCacheId, dataAccessPolicy, strategy, arg1);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, std::function<IMap<K, V> (Arg1, Arg2)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, Arg3 arg3, std::function<IMap<K, V> (Arg1, Arg2, Arg3)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2, arg3);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2, arg3, arg4);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, std::function<IMap<K, V> (Arg1, Arg2, Arg3, Arg4, Arg5)> strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4, Arg5>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2, arg3, arg4, arg5);
        return *this;
    }

    // --------------------------------------------------
    // chaining of strategies
    // --------------------------------------------------

    template <typename K, typename V>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, typename Templates::Strategy0<IMap<K, V>>::Ptr strategy)
    {
        group_.Chain<K,V>(masterCacheId, dataAccessPolicy, strategy);
        return *this;
    }

    template <typename K, typename V, typename Arg1>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, typename Templates::Strategy1<IMap<K, V>, Arg1>::Ptr strategy)
    {
        group_.Chain<K,V,Arg1>(masterCacheId, dataAccessPolicy, strategy, arg1);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, typename Templates::Strategy2<IMap<K, V>, Arg1, Arg2>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, Arg3 arg3, typename Templates::Strategy3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2, arg3);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, typename Templates::Strategy4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2, arg3, arg4);
        return *this;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    StreamerGroup& Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy dataAccessPolicy, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, typename Templates::Strategy5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr strategy)
    {
        group_.Chain<K, V, Arg1, Arg2, Arg3, Arg4, Arg5>(masterCacheId, dataAccessPolicy, strategy, arg1, arg2, arg3, arg4, arg5);
        return *this;
    }

private:
    const RxData::CacheDescription      masterCacheId_;
    const DataAccessPolicy              dataAccessPolicy_;
    RxDataAccessGroup                   group_;
};

}
