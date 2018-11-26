#include"RxStreamer/DataAccessPolicy.h"

namespace Reactor
{

DataAccessPolicy::DataAccessPolicy(
        CommandControllerPolicy commandControllerPolicy,
        RxData::ObjectHomePolicy cachePolicy,
        Policy::Reload reload,
        Policy::Attempt attempt,
        Policy::Lifespan lifetime,
        Policy::LeasePlan leasePlan,
        Policy::Interval refreshInterval)
    : controllerPolicy_(commandControllerPolicy)
    , cachePolicy_(cachePolicy)
    , reload_(reload)
    , attempt_(attempt)
    , lifetime_(lifetime)
    , leasePlan_(leasePlan)
    , refreshInterval_(refreshInterval)
{ }

Policy::Reload DataAccessPolicy::Reload() const
{
    return reload_;
}

Policy::Attempt DataAccessPolicy::attempt() const
{
    return attempt_;
}

Policy::Lifespan DataAccessPolicy::lifetime() const
{
    return lifetime_;
}

Policy::LeasePlan DataAccessPolicy::leasePlan() const
{
    return leasePlan_;
}

Policy::Interval DataAccessPolicy::refreshInterval() const
{
    return refreshInterval_;
}

CommandControllerPolicy DataAccessPolicy::ControllerPolicy() const
{
    return controllerPolicy_;
}

RxData::ObjectHomePolicy DataAccessPolicy::CachePolicy() const
{
    return cachePolicy_;
}

DataAccessPolicy DataAccessPolicy::NoReload(CommandControllerPolicy commandControllerPolicy, RxData::ObjectHomePolicy cachePolicy)
{
    return DataAccessPolicy(commandControllerPolicy, cachePolicy, Policy::Reload::No(), Policy::Attempt::NumSuccessfulTimes(1, 1), Policy::Lifespan::Minutes(10), Policy::LeasePlan::NoRenew(), Policy::Interval::RunImmediatelyAndThenEvery(10000));
}

DataAccessPolicy DataAccessPolicy::ReloadOnExpiry(CommandControllerPolicy commandControllerPolicy, RxData::ObjectHomePolicy cachePolicy)
{
    return DataAccessPolicy(commandControllerPolicy, cachePolicy, Policy::Reload::OnDelete(), Policy::Attempt::Forever(), Policy::Lifespan::Minutes(10), Policy::LeasePlan::NoRenew(), Policy::Interval::RunImmediatelyAndThenEvery(10000));
}

DataAccessPolicy DataAccessPolicy::ReloadOnModify(CommandControllerPolicy commandControllerPolicy, RxData::ObjectHomePolicy cachePolicy)
{
    return DataAccessPolicy(commandControllerPolicy, cachePolicy, Policy::Reload::OnModify(), Policy::Attempt::Forever(), Policy::Lifespan::Minutes(10), Policy::LeasePlan::NoRenew(), Policy::Interval::RunImmediatelyAndThenEvery(10000));
}

DataAccessPolicy DataAccessPolicy::Default()
{
    return ReloadOnExpiry(CommandControllerPolicy::Default(), RxData::CacheFactory::Instance().defaultCachePolicy());
}

}
