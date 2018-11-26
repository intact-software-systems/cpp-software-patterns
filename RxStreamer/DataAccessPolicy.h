#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

/**
 * TODO: Attempt with retry more than once and NoReload does not work because there is no "automatic retry mechanism".
 * We need this in a monitor/supervisor to automatic retry (i.e., subscribe) upon failures.
 */
class DLL_STATE DataAccessPolicy
{
public:
    DataAccessPolicy(
            CommandControllerPolicy commandControllerPolicy,
            RxData::ObjectHomePolicy cachePolicy,
            Policy::Reload reload,
            Policy::Attempt attempt,
            Policy::Lifespan lifetime,
            Policy::LeasePlan leasePlan,
            Policy::Interval refreshInterval);

    Policy::Reload Reload() const;
    Policy::Attempt attempt() const;
    Policy::Lifespan lifetime() const;
    Policy::LeasePlan leasePlan() const;
    Policy::Interval refreshInterval() const;

    CommandControllerPolicy ControllerPolicy() const;
    RxData::ObjectHomePolicy CachePolicy() const;

    static DataAccessPolicy NoReload(CommandControllerPolicy commandControllerPolicy, RxData::ObjectHomePolicy cachePolicy);
    static DataAccessPolicy ReloadOnExpiry(CommandControllerPolicy commandControllerPolicy, RxData::ObjectHomePolicy cachePolicy);
    static DataAccessPolicy ReloadOnModify(CommandControllerPolicy commandControllerPolicy, RxData::ObjectHomePolicy cachePolicy);
    static DataAccessPolicy Default();

private:
    CommandControllerPolicy controllerPolicy_;
    RxData::ObjectHomePolicy cachePolicy_;

private:
    Policy::Reload reload_;
    Policy::Attempt attempt_;
    Policy::Lifespan lifetime_;
    Policy::LeasePlan leasePlan_;
    Policy::Interval refreshInterval_;
};

}
