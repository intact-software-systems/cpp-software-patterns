#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessControllerPolicy.h"

namespace Reactor
{

template <typename T>
class DataAccessStrategyPolicyBase
{
public:
    virtual ~DataAccessStrategyPolicyBase() {}

    virtual T Strategy() const = 0;

    virtual DataAccessControllerPolicy Policy() const = 0;

    virtual RxData::CacheDescription CacheId() const = 0;
};

template <typename T>
class DataAccessStrategyPolicy : public DataAccessStrategyPolicyBase<T>
{
public:
    DataAccessStrategyPolicy(RxData::CacheDescription cacheId, DataAccessControllerPolicy policy, T strategy)
        : cacheId_(cacheId)
        , policy_(policy)
        , strategy_(strategy)
    { }

    CLASS_TRAITS(DataAccessStrategyPolicy)

    virtual T Strategy() const
    {
        return strategy_;
    }

    virtual DataAccessControllerPolicy Policy() const
    {
        return policy_;
    }

    RxData::CacheDescription CacheId() const
    {
        return cacheId_;
    }

private:
    const RxData::CacheDescription cacheId_;
    const DataAccessControllerPolicy policy_;
    const T strategy_;
};

}
