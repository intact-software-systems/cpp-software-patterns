#pragma once

#include<RxStreamer/DataAccessPolicy.h>
#include"RxStreamer/RxDataAccessGroup.h"
#include"RxStreamer/StreamerGroup.h"
#include"RxStreamer/RxActionChain.h"
#include"RxStreamer/Export.h"

namespace Reactor {

// --------------------------------------------------
// Policy and state classes
// --------------------------------------------------

class DLL_STATE StreamerPolicy
{
public:
    StreamerPolicy(Policy::Lifespan lifetime)
        : lifetime_(lifetime)
    { }

    virtual ~StreamerPolicy()
    { }

    static StreamerPolicy CacheUntilFinalize()
    {
        return StreamerPolicy(Policy::Lifespan::Minutes(0));
    }

    static StreamerPolicy Unrelated()
    {
        return StreamerPolicy(Policy::Lifespan::Infinite());
    }

    bool IsCacheUntilFinalize()
    {
        return lifetime_.InMillis() <= 0;
    }

private:
    const Policy::Lifespan lifetime_;
};

// --------------------------------------------------
// state classes
// --------------------------------------------------

class DLL_STATE StreamerState
{
public:
    StreamerState(RxActionChain actionChain, RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy)
        : actionChain_(actionChain)
        , masterCacheId_(masterCacheId)
        , rxPolicy_(rxPolicy)
    { }

    virtual ~StreamerState()
    { }

    template <typename K, typename V>
    RxData::ObjectReaderStub<V, K> Reader()
    {
        std::shared_ptr<RxData::Cache> cache = Cache<V>();
        return RxData::ObjectReaderStub<V, K>(cache->getHomeByName<V, K>(GetTypeName<V>()), cache);
    }

    template <typename V>
    static std::string GetTypeName()
    {
        return BaseLib::Utility::GetTypeName<V>();
    }

    template <typename V>
    static RxData::Cache::Ptr Cache()
    {
        return RxData::CacheFactory::Instance().getOrCreateCache(GetTypeName<V>());
    }

    void Release()
    {
        actionChain_.Controller()->Cancel();
        RxData::Cache::Ptr cache = RxData::CacheFactory::Instance().findCacheByDescription(masterCacheId_);
        if(cache)
        {
            cache->clearAllData();
        }
    }

    RxActionChain                  actionChain_;
    const RxData::CacheDescription masterCacheId_;
    const DataAccessPolicy         rxPolicy_;
};

// --------------------------------------------------
// Streamer
// --------------------------------------------------

class DLL_STATE Streamer
    : public Templates::ContextObjectShared<StreamerPolicy, StreamerState>
{
public:
    Streamer(
        Templates::ActionContextPtr rxContext,
        RxData::CacheDescription masterCacheId,
        DataAccessControllerPolicy policy,
        DataAccessPolicy dataAccessPolicy,
        StreamerPolicy streamerPolicy
    );
    virtual ~Streamer();

    // --------------------------------------------------
    // Factory functions
    // --------------------------------------------------

    static Streamer Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId, DataAccessControllerPolicy policy, DataAccessPolicy rxPolicy);
    static Streamer Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy);
    static Streamer Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId);
    static Streamer Create(RxData::CacheDescription masterCacheId);

    static Streamer CreateUnique(Templates::ActionContextPtr rxContext);
    static Streamer CreateUnique();
    static Streamer CreateUniqueScoped();

    // --------------------------------------------------
    // Create sequential group
    // --------------------------------------------------

    StreamerGroup Sequential();
    StreamerGroup Sequential(RxData::CacheDescription masterCacheId);
    StreamerGroup Sequential(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy);

    // --------------------------------------------------
    // Create parallel group
    // --------------------------------------------------

    StreamerGroup Parallel();
    StreamerGroup Parallel(RxData::CacheDescription masterCacheId);
    StreamerGroup Parallel(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy);

    // --------------------------------------------------
    // Fluent API
    // --------------------------------------------------

    Streamer& Subscribe();
    bool    Unsubscribe();
    Streamer& Refresh();
    void    Cancel();

    bool IsSubscribed() const;
    bool IsSuccess() const;
    bool IsCancelled() const;

    Streamer& WaitFor(int64 msecs);

    // --------------------------------------------------
    // Release
    // --------------------------------------------------

    void Release();

    // --------------------------------------------------
    // Access cache
    // --------------------------------------------------

    template <typename K, typename V>
    RxData::ObjectReaderStub<V, K> Reader()
    {
        return this->data()->Reader<K, V>();
    }

    template <typename K, typename V>
    V Find(K key, int64 msecs = LONG_MAX)
    {
        RxData::ObjectReaderStub<V, K> reader = this->data()->Reader<K, V>();
        if(!reader.containsKey(key))
        {
            if(!IsSubscribed())
            {
                Subscribe();
            }
            WaitFor(msecs);
        }

        return reader.findObject(key);
    }

    template <typename K, typename V>
    V Find(RxData::CacheDescription description, K key, int64 msecs = LONG_MAX)
    {
        RxData::ObjectReaderStub<V, K> reader = this->data()->Reader<K, V>();
        if(!reader.containsKey(key))
        {
            if(!IsSubscribed())
            {
                Subscribe();
            }
            WaitFor(msecs);
        }

        return reader.findObject(key);
    }
};

}
