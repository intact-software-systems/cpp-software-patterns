#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessGroupPolicy.h"
#include"RxStreamer/ReactorFactory.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

// -----------------------------------------------------------
// class DataAccessGroupState
// -----------------------------------------------------------

class DLL_STATE DataAccessGroupState
{
public:
    typedef std::vector<DataAccessBase::Ptr> DataAccessList;

public:
    DataAccessGroupState();
    virtual ~DataAccessGroupState();

    void Chain(DataAccessBase::Ptr access);

    const DataAccessList&   accesses() const;
    DataAccessList&         accesses();

    const DataAccessList&   CurrentFinishedExecution() const;
    DataAccessList&         CurrentFinishedExecution();

private:
    DataAccessList chain;
    DataAccessList currentFinishedExecution;
};

// -----------------------------------------------------------
// class DataAccessGroup
// -----------------------------------------------------------

class DLL_STATE DataAccessGroup
        : public Templates::IsDoneMethod
        , public Templates::IsExecutingMethod
        , public Templates::StartMethod
        , public Templates::SizeMethod<size_t>
        , public Templates::LockableType<DataAccessGroup>
        , public Templates::ReactorMethods<bool, DataAccessBase::Ptr, BaseLib::GeneralException>
        , public Templates::ContextObjectShared<DataAccessGroupPolicy, DataAccessGroupState, Status::ExecutionStatus>
{
public:
    DataAccessGroup(DataAccessGroupPolicy policy = DataAccessGroupPolicy::Sequential());
    virtual ~DataAccessGroup();

    CLASS_TRAITS(DataAccessGroup)

    // -----------------------------------------------------------
    // Factory functions
    // -----------------------------------------------------------

    static DataAccessGroup Sequential();
    static DataAccessGroup Parallel();

    // -----------------------------------------------------------
    // Access statuses and state
    // -----------------------------------------------------------

    std::vector<DataAccessBase::Ptr> getList();

    virtual bool IsExecuting() const;
    virtual bool IsDone() const;
    virtual bool IsReady() const;
    virtual bool IsFailure() const;

    virtual size_t Size() const;

    virtual void Start();

    // -----------------------------------------------------------
    // reactor functions - add dataAccess to current group execution status (similar to TCP acknowledgments, "Execution control protocol")
    // -----------------------------------------------------------

    virtual bool Next(DataAccessBase::Ptr dataAccess);
    virtual bool Error(GeneralException exception);
    virtual bool Complete();

    // ---------------------------------------------------
    // Chain functions
    // ---------------------------------------------------

    template <typename K, typename V>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, CommandsGroup commands)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V>(masterCacheId, rxPolicy, commands);

        this->data()->accesses().push_back(access);
        return access;
    }

    template <typename K, typename V>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action0<IMap<K, V>>::Ptr action)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V>(masterCacheId, rxPolicy, action);

        this->data()->accesses().push_back(access);
        return access;
    }

    template <typename K, typename V, typename Arg1>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action1<IMap<K, V>, Arg1>::Ptr action, Arg1 arg1)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V, Arg1>(masterCacheId, rxPolicy, action, arg1);

        this->data()->accesses().push_back(access);
        return access;
    }

    template <typename K, typename V, typename Arg1, typename Arg2>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action2<IMap<K, V>, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V, Arg1, Arg2>(masterCacheId, rxPolicy, action, arg1, arg2);

        this->data()->accesses().push_back(access);
        return access;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action3<IMap<K, V>, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V, Arg1, Arg2, Arg3>(masterCacheId, rxPolicy, action, arg1, arg2, arg3);

        this->data()->accesses().push_back(access);
        return access;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action4<IMap<K, V>, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V, Arg1, Arg2, Arg3, Arg4>(masterCacheId, rxPolicy, action, arg1, arg2, arg3, arg4);

        this->data()->accesses().push_back(access);
        return access;
    }

    template <typename K, typename V, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    typename DataAccess<K,V>::Ptr Chain(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy, typename Templates::Action5<IMap<K, V>, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        Locker lock(this);

        if(this->status()->IsExecuting()) return nullptr;

        typename DataAccess<K,V>::Ptr access = ReactorFactory::Instance().Create<K,V, Arg1, Arg2, Arg3, Arg4, Arg5>(masterCacheId, rxPolicy, action, arg1, arg2, arg3, arg4, arg5);

        this->data()->accesses().push_back(access);
        return access;
    }

private:
    bool isDone() const;
};

}
