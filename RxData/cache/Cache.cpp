#include"RxData/cache/Cache.h"
#include"RxData/cache/CacheAccess.h"

namespace RxData
{

Cache::Cache(CacheDescription cacheDescription, bool updatesEnabled)
    : CacheBase(CacheUsage::READ_ONLY, CacheKind::CACHE_KIND)
    , cacheDescription_(cacheDescription)
    , updatesEnabled_(updatesEnabled)
    , subAccesses_()
    , objectHomes_()
    , cacheObserverSignaller_( new Signaller1<void, CacheDescription>() )
{ }

Cache::~Cache()
{ }

/**
 * Refresh the contents of the Cache with respect to its origins.
 *
 * DCPS in case of a main Cache
 */
void Cache::refresh()
{
    // -------------------------------------------------
    // Grab DataSource: Get new data and updated ObjectCache objects
    // -------------------------------------------------
    MutexTypeLocker<MapObjectHome> lock(&objectHomes_);

    for(MapObjectHome::iterator it = objectHomes_.begin(), it_end = objectHomes_.end(); it != it_end; ++it)
    {
        ObjectHomeBase::Ptr objectHome = it->second;
        objectHome->refresh(this);
    }
}

/**
 * Clears the data in the cache for all data-types registered.
 *
 * When the cache is cleared (empty) it can still be used without side-effects.
 */
void Cache::clearAllData()
{
    CacheBase::clearAllObjects();
}

/**
 * Register an ObjectHome (register_home). This method registers the ObjectHome in the
 * Cache.
 *
 * A number of preconditions must be satisfied when invoking the register_home method:
 * the Cache must have a pubsub_state set to INITIAL, the specified ObjectHome may not
 * yet be registered before (either to this Cache or to another Cache), and no other
 * instance of the same class as the specified ObjectHome may already have been registered
 * to this Cache. If these preconditions are not satisfied, FALSE is returned.
 */
bool Cache::registerHome(ObjectHomeBase::Ptr objectHome)
{
    MutexTypeLocker<MapObjectHome> lock(&objectHomes_);

    if(objectHomes_.containsKey(objectHome->getTypeName()))
        return false;

    objectHomes_.put(objectHome->getTypeName(), objectHome);

    return true;
}

/**
 * Attach the CacheObserver to the Cache.
 *
 * @param observer
 */
SlotHolder::Ptr Cache::Connect(CacheObserver *observer)
{
    cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnBeginUpdates);
    cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnEndUpdates);
    cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnUpdatesDisabled);
    return cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnUpdatesEnabled);
}

/**
 * Detach the CacheObserver from the Cache.
 *
 * @param observer
 */
bool Cache::Disconnect(CacheObserver *observer)
{
    return cacheObserverSignaller_->Disconnect<CacheObserver>(observer);
}


void Cache::DisconnectAll()
{
    cacheObserverSignaller_->DisconnectAll();
}

/**
 * Causes the registered (and thus not applied) updates to be taken into account, and thus to trigger
 * the attached Listener, if any.
 */
void Cache::enableUpdates()
{
    // -----------------------------------
    // If flag already set then return
    // -----------------------------------
    if(updatesEnabled_) return;

    // -----------------------------------
    // Set flag and notify listeners
    // -----------------------------------
    updatesEnabled_ = true;
    notifyOnUpdatesEnabled();
}

/**
 * Causes incoming but not yet applied updates to be registered for further application. If it is called
 * in the middle of a set of updates (see Listener operations), the Listener will receive end_updates
 * with a parameter that indicates that the updates have been interrupted.
 */
void Cache::disableUpdates()
{
    // -----------------------------------
    // If flag already set then return
    // -----------------------------------
    if(!updatesEnabled_) return;

    // -----------------------------------
    // Set flag and notify listeners
    // -----------------------------------
    updatesEnabled_ = false;
    notifyOnUpdatesDisabled();
}

/**
 * Explicitly request taking into account the waiting incoming updates (load).
 *
 * In case updates_enabled is TRUE, the load operation does nothing because the updates are taken into account
 * on the fly; in case updates_enabled is FALSE, the load operation 'takes' all the waiting incoming updates
 * and applies them in the Cache. The load operation does not trigger any listener (while automatic taking
 * into account of the updates does - see Section 8.1.6.4, "Listeners Activation," on page 208 for more details
 * on listener activation) and may therefore be useful in particular for global initialization of the Cache.
 *
 * @see TODO: Not yet implemented.
 */
bool Cache::load()
{
    if(updatesEnabled_) return false;

    // -----------------------------------
    // Should I Notify listeners? Read specification
    // -----------------------------------
    notifyOnBeginUpdates();

    // -------------------------------------------
    // Load waiting updates into cache
    // -------------------------------------------
    {

    }

    // -----------------------------------
    // Should I Notify listeners? Read specification
    // -----------------------------------
    notifyOnEndUpdates();

    return false;
}

/**
 * Create new CacheAccess objects dedicated to a given purpose (create_access).
 *
 * This method allows the application to create sub-accesses and takes as a parameter the purpose of that
 * sub-access, namely:
 *
 * - write allowed (WRITE ONLY or READ WRITE) to isolate a thread of modifications.
 *
 * - write forbidden (READ ONLY) - to take a consistent view of a set of objects and isolate it
 *   from incoming updates.
 *
 * @param cacheUsage
 * @return
 */
CacheAccessInterface::Ptr Cache::createAccess(CacheUsage::Type cacheUsage)
{
    CacheBase::Ptr base = this->GetPtr();

    Cache::Ptr self = std::dynamic_pointer_cast<Cache>(base);

    CacheAccess::Ptr cacheAccess( new CacheAccess(cacheUsage, CacheKind::CACHEACCESS_KIND, self)); //this->GetPtr()) );

    subAccesses_.push_back(cacheAccess);

    return cacheAccess;
}

/**
 * Delete sub-accesses (delete_access). Deleting a CacheAccess will purge all its contents.
 *
 * Deleting a CacheAccess that is not created by this Cache will raise a PreconditionNotMet.
 *
 * @param cacheAccess
 */
bool Cache::deleteAccess(CacheAccessInterface::Ptr cacheAccess)
{
    // -- debug --
    ASSERT(cacheAccess);
    // -- debug --

    if(!subAccesses_.contains(cacheAccess))
        return false;

    cacheAccess->purge();

    size_t prevSz = subAccesses_.size();

    subAccesses_.remove(cacheAccess);

    return subAccesses_.size() == (prevSz - 1);
}

CacheUsage::Type Cache::getCacheUsage() const
{
    return CacheBase::getCacheUsage();
}

CacheKind::Type Cache::getCacheKind() const
{
    return CacheBase::getCacheKind();
}

// -----------------------------------
// Notify listeners operations
// -----------------------------------

void Cache::notifyOnBeginUpdates()
{
    cacheObserverSignaller_->Signal<CacheObserver>(cacheDescription_, &CacheObserver::OnBeginUpdates);
}

void Cache::notifyOnEndUpdates()
{
    cacheObserverSignaller_->Signal<CacheObserver>(cacheDescription_, &CacheObserver::OnEndUpdates);
}

void Cache::notifyOnUpdatesEnabled()
{
    cacheObserverSignaller_->Signal<CacheObserver>(cacheDescription_, &CacheObserver::OnUpdatesEnabled);
}

void Cache::notifyOnUpdatesDisabled()
{
    cacheObserverSignaller_->Signal<CacheObserver>(cacheDescription_, &CacheObserver::OnUpdatesDisabled);
}

}
