#ifndef RxData_Cache_Cache_h_IsIncluded
#define RxData_Cache_Cache_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/CacheInterface.h"
#include"RxData/cache/CacheBase.h"
#include"RxData/cache/CacheAccessInterface.h"
#include"RxData/cache/CacheDescription.h"
#include"RxData/cache/CacheKind.h"
#include"RxData/cache/CacheUsage.h"

#include"RxData/cache/observer/CacheListener.h"

#include"RxData/cache/object/ObjectHome.h"
#include"RxData/cache/object/ObjectCache.h"
#include"RxData/cache/object/policy/ObjectHomePolicy.h"

#include"RxData/Export.h"

namespace RxData
{

/**
 * An instance of this class gathers a set of objects that are managed, published,
 * and/or subscribed consistently.
 *
 * @author KVik
 */
class DLL_STATE Cache : public CacheBase
                      , public CacheInterface
                      , public Observable<CacheObserver> //public CacheReaderListener
{
private:
    // ----------------------------------
    // Typedefs
    // ----------------------------------

    /**
     * @brief MapObjectHome
     */
    typedef IMap<std::string, ObjectHomeBase::Ptr > MapObjectHome;

    /**
     * @brief ListCacheAccess
     */
    typedef IList<CacheAccessInterface::Ptr> ListCacheAccess;

    // ----------------------------------
    // Attributes
    // ----------------------------------

    /**
     * brief cacheDescription_
     */
    CacheDescription cacheDescription_;

    /**
     * The state of the cache with respect to incoming updates (updates_enabled).
     * This state is modifiable by the applications (see enable_updates,
     * disable_updates) in order to support applications that are both publishing
     * and subscribing.
     */
    bool updatesEnabled_;

    /**
     * The attached CacheAccess objects.
     */
    ListCacheAccess subAccesses_;

    /**
     * The attached ObjectHome objects.
     */
    MapObjectHome objectHomes_;

    /**
     * The attached CacheObserver objects.
     */
    Signaller1<void, CacheDescription>::Ptr cacheObserverSignaller_;

public:
    // ----------------------------------
    // Constructor/Destructor
    // ----------------------------------

    Cache(CacheDescription cacheDescription, bool updatesEnabled);

    virtual ~Cache();

    CLASS_TRAITS(Cache)

    // ----------------------------------
    // Callbacks
    // ----------------------------------

    /**
     * Callback by the data-producer (Example: subscriber).
     *
     * TODO: Move this function to an implementation of this listener that holds a Ptr to the Cache.
     * TODO: Add parameter TypeName, this is now hard-coded
     *
     * @param <DATA>
     * @param <KEY>
     * @param data
     * @param key
     * @return
     */
    template <typename DATA, typename KEY>
    bool OnDataAvailable(const DATA &data, KEY key)
    {
        // -------------------------------------------------
        // Notifies CacheListeners
        // -------------------------------------------------
        notifyOnBeginUpdates();

        // -------------------------------------------------
        // Actual logic of updating cache
        // -------------------------------------------------
        bool isAdded = onDataAvailablePrivate<DATA, KEY>(data, key, TYPE_NAME(data));

        // -------------------------------------------------
        // Notifies CacheListeners
        // -------------------------------------------------
        notifyOnEndUpdates();

        return isAdded;
    }

private:
    /**
     * Callback by the data-producer (Example: subscriber).
     *
     * @param <DATA>
     * @param <KEY>
     * @param data
     * @param key
     * @return
     */
    template <typename DATA, typename KEY>
    bool onDataAvailablePrivate(const DATA &data, KEY key, std::string typeName)
    {
        typename ObjectHome<DATA, KEY>::Ptr objectHome = getOrCreateHome<DATA, KEY>(typeName);

        // -- debug --
        ASSERT(objectHome);
        // -- debug --

        ObjectCache::Ptr objectCache = getOrCreateObjectCache(objectHome->getTypeName());

        // -- debug --
        ASSERT(objectCache);
        // -- debug --

        return objectHome->onDataAvailable(data, key, objectCache);
    }

public:
    // ----------------------------------
    // Operations
    // ----------------------------------

    /**
     * Refresh the contents of the Cache with respect to its origins.
     *
     * DCPS in case of a main Cache
     */
    virtual void refresh();

    /**
     * Clears the data in the cache for all data-types registered.
     *
     * When the cache is cleared (empty) it can still be used without side-effects.
     */
    virtual void clearAllData();

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
    bool registerHome(ObjectHomeBase::Ptr objectHome);

    /**
     * Create a new ObjectHome and attach it to the cache, or get an existing ObjectHome.
     *
     * Synchronized access to objetHomes, it can be called from multiple CacheAccess and Cache.
     *
     * @param <DATA>
     * @param typeName
     * @param policy
     * @return
     */
    template <typename DATA, typename KEY>
    typename ObjectHome<DATA,KEY>::Ptr getOrCreateHome(std::string typeName, ObjectHomePolicy policy = ObjectHomePolicy::Default())
    {
        return getOrCreateHomePrivate<DATA,KEY>(typeName, policy);
    }

private:
    /**
     * Create a new ObjectHome and attach it to the cache, or get an existing ObjectHome.
     *
     * Synchronized access to objetHomes, it can be called from multiple CacheAccess and Cache.
     *
     * @param <DATA>
     * @param typeName
     * @param policy
     * @return
     */
    template <typename DATA, typename KEY>
    typename ObjectHome<DATA, KEY>::Ptr getOrCreateHomePrivate(std::string typeName, ObjectHomePolicy policy)
    {
        // ----------------------------------------------
        // Check if home exists
        // ----------------------------------------------
        MutexTypeLocker<MapObjectHome> lock(&objectHomes_);

        if(objectHomes_.find(typeName) != objectHomes_.end())
        {
            typename ObjectHome<DATA,KEY>::Ptr objectHome = DYNAMIC_POINTER_CAST_T2(ObjectHome, DATA, KEY, objectHomes_.at(typeName) );

            // -- debug --
            ASSERT(objectHome);
            // -- debug --

            return objectHome->GetPtr();
        }

        IDEBUG() << "Creating home for type " << typeName;

        // ----------------------------------------------
        // Create new ObjectHome
        // ----------------------------------------------
        typename ObjectHome<DATA,KEY>::Ptr objectHome( new ObjectHome<DATA,KEY>(typeName, policy) );

        objectHomes_.insert(std::pair<std::string, ObjectHomeBase::Ptr>(objectHome->getTypeName(), objectHome->GetPtr()) );

        return objectHome;
    }

public:
    /**
     * Retrieve an already registered ObjectHome based on its name (find_home_by_name) or based
     * on its index of registration (find_home_by_index). If no registered home can be found that
     * satisfies the specified name or index, a NULL is returned.
     *
     * @param typeName
     * @return
     */
    template <typename DATA, typename KEY>
    typename ObjectHome<DATA,KEY>::Ptr getHomeByName(std::string typeName)
    {
        MutexTypeLocker<MapObjectHome> lock(&objectHomes_);

        if(objectHomes_.find(typeName) != objectHomes_.end())
            return DYNAMIC_POINTER_CAST_T2(ObjectHome, DATA, KEY, objectHomes_.at(typeName) );

        return typename ObjectHome<DATA,KEY>::Ptr();
    }

    /**
     * @brief getHomeByName
     * @param typeName
     * @return
     */
    ObjectHomeBase::Ptr getHomeByName(std::string typeName)
    {
        MutexTypeLocker<MapObjectHome> lock(&objectHomes_);

        if(objectHomes_.find(typeName) != objectHomes_.end())
            return objectHomes_.at(typeName);

        return ObjectHomeBase::Ptr();
    }

    /**
     * Register all known ObjectHome to the Pub/Sub level (register_all_for_pubsub), i.e., create
     * all the needed DCPS entities; registration is performed for publication, for subscription,
     * or for both according to the cache_usage. At this stage, it is the responsibility of the
     * service to ensure that all the object homes are properly linked and set up: that means in
     * particular that all must have been registered before. When an ObjectHome still refers to
     * another ObjectHome that has not yet been registered, a BadHomeDefinition is raised. A number
     * of preconditions must also be satisfied before invoking the register_all_for_pubsub method:
     * at least one ObjectHome needs to have been registered, and the pubsub_state may not yet be
     * ENABLED. If these preconditions are not satisfied, a PreconditionNotMet will be raised.
     *
     * Invoking the register_all_for_pub_sub on a REGISTERED pubsub_state will be considered a no-op.
     */
    //public void registerAllForPubSub() {}

    /**
     * Enable the derived Pub/Sub infrastructure (enable_all_for_pubsub). QoS setting can be performed
     * between those two operations. One precondition must be satisfied before invoking the
     * enable_all_for_pub_sub method: the pubsub_state must already have been set to REGISTERED before.
     * A PreconditionNotMet exception is thrown otherwise.
     *
     * Invoking the enable_all_for_pub_sub method on an ENABLED pubsub_state will be considered a no-op.
     */
    //public void enableAllForPubSub() {}

    /**
     * @brief Connect
     * @param observer
     */
    virtual SlotHolder::Ptr Connect(CacheObserver *observer);

    /**
     * @brief RemoveObserver
     * @param observer
     * @return
     */
    virtual bool Disconnect(CacheObserver *observer);

    /**
     * @brief DisconnectAll
     */
    virtual void DisconnectAll();

    /**
     * Causes the registered (and thus not applied) updates to be taken into account, and thus to trigger
     * the attached Listener, if any.
     */
    virtual void enableUpdates();

    /**
     * Causes incoming but not yet applied updates to be registered for further application. If it is called
     * in the middle of a set of updates (see Listener operations), the Listener will receive end_updates
     * with a parameter that indicates that the updates have been interrupted.
     */
    virtual void disableUpdates();

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
    virtual bool load();

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
    CacheAccessInterface::Ptr createAccess(CacheUsage::Type cacheUsage);

    /**
     * Delete sub-accesses (delete_access). Deleting a CacheAccess will purge all its contents.
     *
     * Deleting a CacheAccess that is not created by this Cache will raise a PreconditionNotMet.
     *
     * @param cacheAccess
     */
    bool deleteAccess(CacheAccessInterface::Ptr cacheAccess);

    // ----------------------------------
    // Getters
    // ----------------------------------

    virtual bool isUpdatesEnabled() const
    {
        return updatesEnabled_;
    }

    virtual std::string getCacheName() const
    {
        return cacheDescription_.getName();
    }

    virtual CacheDescription getCacheDescription() const
    {
        return cacheDescription_;
    }

    virtual std::set<std::string> getTypeNames() const
    {
        return objectHomes_.keySet();
    }

    /**
     * @brief getCacheUsage
     * @return
     */
    virtual CacheUsage::Type getCacheUsage() const;

    /**
     * @brief getCacheKind
     * @return
     */
    virtual CacheKind::Type getCacheKind() const;

    // ----------------------------------
    // Signaling functions
    // ----------------------------------

    /**
     * Call OnBeginUpdates on attached CacheListeners
     */
    virtual void notifyOnBeginUpdates();

    /**
     * Call OnEndUpdates on attached CacheListeners
     */
    virtual void notifyOnEndUpdates();

private:

    /**
     * Call OnUpdatesEnabled on attached CacheListeners
     */
    void notifyOnUpdatesEnabled();

    /**
     * Call OnUpdatesDisabled on attached CacheListeners
     */
    void notifyOnUpdatesDisabled();
};

}

#endif

