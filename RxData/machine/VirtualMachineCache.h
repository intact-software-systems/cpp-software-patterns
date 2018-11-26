#pragma once

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/observer/CacheListener.h"
#include"RxData/cache/CacheDescription.h"
#include"RxData/cache/object/ObjectCache.h"
#include"RxData/cache/object/policy/ObjectHomePolicy.h"

#include"RxData/Export.h"
namespace RxData
{

/*
DDS::DomainParticipant_var dp;
DLRL::CacheFactory_var cf;

// Init phase

DLRL::Cache_var c = cf->create_cache (WRITE_ONLY, dp);
RadarHome_var rh;
TrackHome_var th;
Track3DHome_var t3dh;
c->register_home (rh);
c->register_home (th);
c->register_home (t3dh);
c->register_all_for_pubsub();


// some QoS settings if needed
c->enable_all_for_pubsub();
//Creation, modifications and publication

Radar_var r1 = rh->create_object(c);
Track_var t1 = th->create-object (c);
Track3D_var t2 = t3dh->create-object (c);
t1->w(12); // setting of a pure local attribute
t1->x(1000.0); // some DLRL attributes settings
t1->y(2000.0);
t2->a_radar->put(r1);// modifies r1->tracks accordingly
t2->x(1000.0);
t2->y(2000.0);
t2->z(3000.0);
t2->a_radar->put(r1);// modifies r1->tracks accordingly
c->write(); // all modifications are published
};
*/

/**
 * This is a singleton virtual machine that monitors all caches and enforces the policies.
 *
 * The singleton is attached as a CacheListener to all created Caches
 */
class DLL_STATE VirtualMachineCache
    : public RxData::CacheObserver
    , public Runnable
    , public Templates::SupervisorSingleton<VirtualMachineCache>
{
private:
    /**
     * The set of ObjectHomes that are updated (i.e. written to).
     */
    ISet<RxData::CacheDescription>	setUpdatedCaches_;

    /**
     * Initialization performed once.
     */
    bool isInitialized_;

public:

    VirtualMachineCache();
    virtual ~VirtualMachineCache();

    /**
     * Initializes and starts the virtual machine. Called once.
     */
    virtual bool Initialize();

    virtual bool IsInitialized() const;

    /**
     * Iterate through the Cache and ObjectHomes to enforce the policies
     * that each ObjectHome has attached.
     */
    virtual void run();

    /**
     * Iterate through the updated caches and enforce their policies.
     *
     * TODO: To have a complete and fault-tolerant cache that regards the specific as well as overall
     * resource limits, we need to:
     *
     *  - Police individual types through ObjectHomes
     *  - Police groups of types through Caches
     *  - Police all Caches as the final overall group.
     *  - Police according to system limitations. For example, the available memory.
     *
     * Only when the caches are policed as individuals and at group level, we can achieve complete control.
     * The order can be from smaller entity to the biggest.
     *
     * @param updatedCacheSet
     */
    void policeTheUpdatedCaches(ISet<RxData::CacheDescription> updatedCacheSet);

    /**
     * Enforce the ObjectHomePolicy attached to an ObjectHome. Replacement strategies,
     * and cleanup.
     *
     * @param policy
     * @param objectCache
     */
    void enforceCachePolicy(const RxData::ObjectHomePolicy &policy,
                            RxData::ObjectCache::Ptr objectCache,
                            const std::string &typeName);

    // ------------------------------------
    // Callbacks
    // ------------------------------------

    virtual void OnBeginUpdates(RxData::CacheDescription description);

    virtual void OnEndUpdates(RxData::CacheDescription description);

    virtual void OnUpdatesEnabled(RxData::CacheDescription description);

    virtual void OnUpdatesDisabled(RxData::CacheDescription description);

private:
    /**
     * Insert CacheDescription to set
     *
     * @param description
     */
    void addToUpdatedCache(RxData::CacheDescription description);
};

}
