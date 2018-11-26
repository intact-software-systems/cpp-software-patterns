#include"RxData/machine/VirtualMachineCache.h"

#include"RxData/cache/Cache.h"
#include"RxData/cache/CacheFactory.h"
#include"RxData/cache/object/ObjectHome.h"
#include"RxData/cache/object/ObjectState.h"
#include"RxData/cache/policy/Ejection.h"

namespace RxData
{

/**
 * @brief VirtualMachineCache::VirtualMachineCache
 */
VirtualMachineCache::VirtualMachineCache()
    : setUpdatedCaches_()
    , isInitialized_(false)
{ }

VirtualMachineCache::~VirtualMachineCache()
{ }

/**
 * Initializes and starts the virtual machine. Called once.
 */
bool VirtualMachineCache::Initialize()
{
    BaseLib::Concurrent::ThreadPoolFactory::Instance().GetDefault()->Schedule((Runnable*)this, TaskPolicy::RunForever());
    isInitialized_ = true;
    return isInitialized_;
}

bool VirtualMachineCache::IsInitialized() const
{
    return isInitialized_;
}

/**
 * Iterate through the Cache and ObjectHomes to enforce the policies
 * that each ObjectHome has attached.
 */
void VirtualMachineCache::run()
{
    ISet<CacheDescription> updatedCacheSet;

    {
        MutexTypeLocker< ISet<CacheDescription> > lock(&setUpdatedCaches_);

        updatedCacheSet = ISet<CacheDescription>(setUpdatedCaches_);

        setUpdatedCaches_.clear();
    }

    // -----------------------------------------------------------
    // Iterate through the Cache and ObjectHomes
    // enforce the policies that each ObjectHome has attached.
    // -----------------------------------------------------------
    try
    {
        policeTheUpdatedCaches(updatedCacheSet);
    }
    catch(Exception e)
    {
        ICRITICAL() << "Exception was thrown while policing caches!";
    }
}

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
void VirtualMachineCache::policeTheUpdatedCaches(ISet<CacheDescription> updatedCacheSet)
{
    if(updatedCacheSet.empty()) return;

    // -----------------------------------------------------------
    // Iterate through the Cache and ObjectHomes
    // enforce the policies that each ObjectHome has attached.
    // -----------------------------------------------------------
    for(ISet<CacheDescription>::iterator it = updatedCacheSet.begin(), it_end = updatedCacheSet.end(); it != it_end; ++it)
    {
        CacheDescription description = *it;

        RxData::Cache::Ptr cache = CacheFactory::Instance().findCacheByDescription(description);

        if(!cache)
        {
            // It is allowed, but should only happen as a rare race condition
            IWARNING() << "Cannot find cache description " << description.getName();
            continue;
        }

        // ------------------------------------------------------------
        // Cache-eviction policy
        // ------------------------------------------------------------
        std::set<std::string> setOfTypeNames = cache->getTypeNames();
        for(std::set<std::string>::iterator it = setOfTypeNames.begin(), it_end = setOfTypeNames.end(); it != it_end; ++it)
        {
            std::string typeName = *it;

            ObjectHomeBase::Ptr objectHome = cache->getHomeByName(typeName);

            if(!objectHome)
            {
                // It is allowed, but should only happen as a rare race condition
                IWARNING() << "Cannot find ObjectHome for type-name " << typeName;
                continue;
            }

            // -----------------------------------------------------------
            // ObjectCache and Policy is needed to "clean up caches"
            // -----------------------------------------------------------
            ObjectCache::Ptr objectCache = cache->getOrCreateObjectCache(typeName);
            ObjectHomePolicy policy = objectHome->getPolicy();

            // -- debug --
            ASSERT(objectCache);
            // -- debug --

            // -----------------------------------------------------------
            // Enforce the policy that the ObjectHome has attached.
            // -----------------------------------------------------------
            enforceCachePolicy(policy, objectCache, typeName);
        }
    }
}

/**
 * Enforce the ObjectHomePolicy attached to an ObjectHome. Replacement strategies,
 * and cleanup.
 *
 * @param policy
 * @param objectCache
 */
void VirtualMachineCache::enforceCachePolicy(const ObjectHomePolicy &policy, ObjectCache::Ptr objectCache, const std::string &)
{
    // -- debug --
    ASSERT(objectCache);
    // -- debug --

    // -----------------------------------------------------------
    // ResourceLimits policy
    // -----------------------------------------------------------
    if(objectCache->size() <= policy.getResourceLimits().GetMaxSamples()) return;

    // -----------------------------------------------------------
    // Remove deleted items first
    // -----------------------------------------------------------
    if(objectCache->size() > policy.getResourceLimits().GetMaxSamples())
    {
        // -- debug --
        // size_t prevSz 		= objectCache->size();
        // int64 currentTimeMs = Utility::GetCurrentTimeMs();
        // -- debug --

        std::set<ObjectId> listOfDeleted = objectCache->readKeysWithReadState<ObjectId>(ObjectState::OBJECT_DELETED);

        if(!listOfDeleted.empty())
        {
            objectCache->remove<ObjectId>(listOfDeleted);

//            // -- debug --
//            long timeSpent = Utility::GetCurrentTimeMs() - currentTimeMs;
//            IDEBUG()  << "Time spent "
//                      << timeSpent
//                      << " ResourceLimits("
//                      << policy.getResourceLimits().GetMaxSamples()
//                      << ") Removing deleted "
//                      << typeName
//                      << " sz "
//                      << prevSz
//                      << " to "
//                      << objectCache->size()
//                      << " elements. Keys erased: "
//                      << listOfDeleted;
//            // -- debug --
        }
    }

    // -----------------------------------------------------------
    // TODO: Use Lifespan policy to evict old data
    // -----------------------------------------------------------
    //if(objectCache->size() > policy.getResourceLimits().getMaxSamples())
    //{}

    // -----------------------------------------------------------
    // Use ejection/replacement policy to clean out cache
    // -----------------------------------------------------------
    if(objectCache->size() > policy.getResourceLimits().GetMaxSamples())
    {
        // -- debug --
        // size_t prevSz 		= objectCache->size();
        // int64 currentTimeMs = Utility::GetCurrentTimeMs();
        // -- debug --

        long numSamplesToRemove = objectCache->size() - policy.getResourceLimits().GetMaxSamples();

        // -----------------------------------------------------------
        // LRU and LFU are well-known cache replacement strategies
        // -----------------------------------------------------------
        if(numSamplesToRemove > 0)
        {
            std::set<ObjectId> setOfRemoved;

            if(policy.getEjection().getEjectionKind() == EjectionKind::LEAST_RECENTLY_USED)
            {
                setOfRemoved = objectCache->removeNLeastRecentlyUsed<ObjectId>(numSamplesToRemove);
            }
            else if(policy.getEjection().getEjectionKind() == EjectionKind::LEAST_FREQUENTLY_USED)
            {
                setOfRemoved = objectCache->removeNLeastFrequentlyUsed<ObjectId>(numSamplesToRemove);
            }
            else
            {
                // -- debug --
                IWARNING() << "No ejection policy given, or not recognized :" << (int)policy.getEjection().getEjectionKind();
                // -- debug --
            }

//            // -- debug --
//            if(!setOfRemoved.empty())
//            {
//                long timeSpent = System.currentTimeMillis() - currentTimeMs;
//                IDEBUG() << "Time spent "
//                         << timeSpent
//                         << " ResourceLimits("
//                         << policy.getResourceLimits().getMaxSamples()
//                         << ") and Ejection("
//                         << policy.getEjection().getEjectionKind()
//                         << ") "
//                         << typeName
//                         << " sz "
//                         << prevSz
//                         << " to "
//                         << objectCache->size()
//                         << " elements. Keys erased: "
//                         << setOfRemoved;
//            }
//            // -- debug --
        }
    }
}

// ------------------------------------------------------------------------
// Callbacks
// ------------------------------------------------------------------------

void VirtualMachineCache::OnBeginUpdates(CacheDescription )
{
    // Stop monitoring thread?
    // Make sure there is space in cache according to policy?
    // Evict objects according to policy.
}

void VirtualMachineCache::OnEndUpdates(CacheDescription description)
{
    addToUpdatedCache(description);
}

void VirtualMachineCache::OnUpdatesEnabled(CacheDescription )
{
    // Nothing to do?
}

void VirtualMachineCache::OnUpdatesDisabled(CacheDescription )
{
    // Nothing to do?
}

// ------------------------------------------------------------------------
// Private functions
// ------------------------------------------------------------------------

void VirtualMachineCache::addToUpdatedCache(CacheDescription description)
{
    // ----------------------------------------
    // Flag to see if anything was added to set
    // ----------------------------------------
    bool isUpdatedSet = false;

    // ----------------------------------------
    // Add description
    // ----------------------------------------
    {
        MutexTypeLocker< ISet<CacheDescription> > lock(&setUpdatedCaches_);

        isUpdatedSet = !setUpdatedCaches_.contains(description);
        setUpdatedCaches_.insert(description);
    }

    // ----------------------------------------
    // Q: Anything to do if set is updated?
    // A: Currently the run() function is executed every n seconds regardless
    // ----------------------------------------
    if(isUpdatedSet)
    {
        // No reaction implemented
    }
}

}

