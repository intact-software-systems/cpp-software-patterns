#include "RxStreamer/Streamer.h"

namespace Reactor {

Streamer::Streamer(
    Templates::ActionContextPtr rxContext,
    RxData::CacheDescription masterCacheId,
    DataAccessControllerPolicy policy,
    DataAccessPolicy dataAccessPolicy,
    StreamerPolicy streamerPolicy
)
    : Templates::ContextObjectShared<StreamerPolicy, StreamerState>
    (
        new StreamerPolicy(streamerPolicy),
        new StreamerState(RxActionChain::Create(rxContext, masterCacheId, policy), masterCacheId, dataAccessPolicy)
    )
{ }

Streamer::~Streamer()
{
    if(this->config()->IsCacheUntilFinalize())
    {
        this->data()->Release();
    }
}

// --------------------------------------------------
// Factory functions
// --------------------------------------------------

Streamer Streamer::Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId, DataAccessControllerPolicy policy, DataAccessPolicy rxPolicy)
{
    return Streamer(
        rxContext,
        masterCacheId,
        policy,
        rxPolicy,
        StreamerPolicy::Unrelated()
    );
}

Streamer Streamer::Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy)
{
    return Streamer(
        rxContext,
        masterCacheId,
        DataAccessControllerPolicy::Default(),
        rxPolicy,
        StreamerPolicy::Unrelated()
    );
}

Streamer Streamer::Create(Templates::ActionContextPtr rxContext, RxData::CacheDescription masterCacheId)
{
    return Streamer(
        rxContext,
        masterCacheId,
        DataAccessControllerPolicy::Default(),
        DataAccessPolicy::Default(),
        StreamerPolicy::Unrelated()
    );
}

Streamer Streamer::CreateUnique(Templates::ActionContextPtr rxContext)
{
    return Streamer(
        rxContext,
        RxData::CacheDescription::CreateUnique(),
        DataAccessControllerPolicy::Default(),
        DataAccessPolicy::Default(),
        StreamerPolicy::Unrelated()
    );
}

Streamer Streamer::Create(RxData::CacheDescription masterCacheId)
{
    return Streamer(
        Templates::ActionContext::Empty(),
        masterCacheId,
        DataAccessControllerPolicy::Default(),
        DataAccessPolicy::Default(),
        StreamerPolicy::Unrelated()
    );
}

Streamer Streamer::CreateUnique()
{
    return Streamer(
        Templates::ActionContext::Empty(),
        RxData::CacheDescription::CreateUnique(),
        DataAccessControllerPolicy::Default(),
        DataAccessPolicy::Default(),
        StreamerPolicy::Unrelated()
    );
}

Streamer Streamer::CreateUniqueScoped()
{
    return Streamer(
        Templates::ActionContext::Empty(),
        RxData::CacheDescription::CreateUnique(),
        DataAccessControllerPolicy::Default(),
        DataAccessPolicy::Default(),
        StreamerPolicy::CacheUntilFinalize()
    );
}

// --------------------------------------------------
// Create sequential group
// --------------------------------------------------

StreamerGroup Streamer::Sequential()
{
    RxDataAccessGroup group = this->data()->actionChain_.Sequential();
    return StreamerGroup::Create(this->data()->masterCacheId_, this->data()->rxPolicy_, group);
}

StreamerGroup Streamer::Sequential(RxData::CacheDescription masterCacheId)
{
    RxDataAccessGroup group = this->data()->actionChain_.Sequential();
    return StreamerGroup::Create(masterCacheId, this->data()->rxPolicy_, group);
}

StreamerGroup Streamer::Sequential(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy)
{
    RxDataAccessGroup group = this->data()->actionChain_.Sequential();
    return StreamerGroup::Create(masterCacheId, rxPolicy, group);
}

// --------------------------------------------------
// Create parallel group
// --------------------------------------------------

StreamerGroup Streamer::Parallel()
{
    RxDataAccessGroup group = this->data()->actionChain_.Parallel();
    return StreamerGroup::Create(this->data()->masterCacheId_, this->data()->rxPolicy_, group);
}

StreamerGroup Streamer::Parallel(RxData::CacheDescription masterCacheId)
{
    RxDataAccessGroup group = this->data()->actionChain_.Parallel();
    return StreamerGroup::Create(masterCacheId, this->data()->rxPolicy_, group);
}

StreamerGroup Streamer::Parallel(RxData::CacheDescription masterCacheId, DataAccessPolicy rxPolicy)
{
    RxDataAccessGroup group = this->data()->actionChain_.Parallel();
    return StreamerGroup::Create(masterCacheId, rxPolicy, group);
}

// --------------------------------------------------
// Fluent API
// --------------------------------------------------

Streamer& Streamer::Subscribe()
{
    this->data()->actionChain_.Subscribe();
    return *this;
}

bool Streamer::Unsubscribe()
{
    return this->data()->actionChain_.Controller()->Unsubscribe();
}

Streamer& Streamer::Refresh()
{
    this->data()->actionChain_.Controller()->Refresh();
    return *this;
}

void Streamer::Cancel()
{
    this->data()->actionChain_.Controller()->Cancel();
}

bool Streamer::IsSubscribed() const
{
    return this->data()->actionChain_.Controller()->IsSubscribed();
}

bool Streamer::IsSuccess() const
{
    return this->data()->actionChain_.Controller()->IsSuccess();
}

bool Streamer::IsCancelled() const
{
    return this->data()->actionChain_.Controller()->IsCancelled();
}

Streamer& Streamer::WaitFor(int64 msecs)
{
    this->data()->actionChain_.Controller()->WaitFor(msecs);
    return *this;
}

void Streamer::Release()
{
    this->data()->Release();
}

}
