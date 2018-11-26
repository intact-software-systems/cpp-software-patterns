#pragma once

#include<RxData/cache/CacheAccess.h>
#include"RxData/cache/Cache.h"

#include"RxData/Export.h"

namespace RxData
{

/**
 * An instance of this class gathers a set of objects that are managed, published,
 * and/or subscribed consistently.
 *
 * @author KVik
 */
class DLL_STATE CacheDecorator
{
private:
	// ----------------------------------
	// Attributes
	// ----------------------------------

    /**
     * brief cacheDescription_
     */
	Cache::Ptr cache_;

public:
	// ----------------------------------
	// Constructor
	// ----------------------------------

	CacheDecorator(Cache::Ptr cache)
        : cache_(cache)
    {}

    CLASS_TRAITS(CacheDecorator)

	// ----------------------------------
	// Callbacks
	// ----------------------------------

	/**
	 * Callback by the data-producer (Example: subscriber).
     *
     * TODO: Move this function to an implementation of this listener that holds a Ptr to the Cache.
	 *
	 * @param <DATA>
	 * @param <KEY>
	 * @param data
	 * @param key
	 * @return
	 */
	template <typename DATA, typename KEY>
    bool OnDataAvailable(DATA data, KEY key)
	{
        return cache_->OnDataAvailable<DATA, KEY>(data, key);
	}

	// ----------------------------------
	// Operations
	// ----------------------------------

	/**
	 * Refresh the contents of the Cache with respect to its origins.
	 *
	 * DCPS in case of a main Cache
	 */
	void refresh()
	{
        cache_->refresh();
	}

	/**
	 * Clears the data in the cache for all data-types registered.
	 *
	 * When the cache is cleared (empty) it can still be used without side-effects.
	 */
	void clearAllData()
	{
		cache_->clearAllData();
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
	template <typename DATA, typename KEY>
    bool registerHome(typename ObjectHome<DATA, KEY>::Ptr objectHome)
	{
        return cache_->registerHome<DATA, KEY>(objectHome);
	}

	/**
	 * Create a new ObjectHome and attach it to the cache, or get an existing ObjectHome.
	 *
	 * Synchronized access to objetHomes, it can be called from multiple CacheAccess and Cache.
	 *
	 * @param <DATA>
	 * @param typeName
	 * @return
	 */
	template <typename DATA, typename KEY>
    typename ObjectHome<DATA, KEY>::Ptr getOrCreateHome(std::string typeName)
	{
		return cache_->getOrCreateHome<DATA, KEY>(typeName, ObjectHomePolicy::Default());
	}

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
    typename ObjectHome<DATA, KEY>::Ptr getOrCreateHome(std::string typeName, ObjectHomePolicy policy)
	{
		return cache_->getOrCreateHome<DATA, KEY>(typeName, policy);
	}

	/**
	 * Retrieve an already registered ObjectHome based on its name (find_home_by_name) or based
	 * on its index of registration (find_home_by_index). If no registered home can be found that
	 * satisfies the specified name or index, a NULL is returned.
	 *
	 * @param typeName
	 * @return
	 */
    template <typename DATA, typename KEY>
    typename ObjectHome<DATA, KEY>::Ptr getHomeByName(std::string typeName)
	{
        return cache_->getHomeByeName<DATA, KEY>(typeName);
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
	 * Attach the CacheListener to the Cache.
	 *
	 * @param listener
	 */
	bool attachListener(CacheListener::Ptr listener)
	{
        return cache_->attachListener(listener);
	}

	/**
	 * Detach the CacheListener from the Cache.
	 *
	 * @param listener
	 */
	bool detachListener(CacheListener::Ptr listener)
	{
        return cache_->detachListener(listener);
	}

	/**
	 * Causes the registered (and thus not applied) updates to be taken into account, and thus to trigger
	 * the attached Listener, if any.
	 */
	void enableUpdates()
	{
        return cache_->enableUpdates();
	}

	/**
	 * Causes incoming but not yet applied updates to be registered for further application. If it is called
	 * in the middle of a set of updates (see Listener operations), the Listener will receive end_updates
	 * with a parameter that indicates that the updates have been interrupted.
	 */
	void disableUpdates()
	{
        return cache_->disableUpdates();
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
	bool load()
	{
        return cache_->load();
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
    CacheAccess::Ptr createAccess(CacheUsage::Type cacheUsage)
	{
        return cache_->createAccess(cacheUsage);
	}

	/**
	 * Delete sub-accesses (delete_access). Deleting a CacheAccess will purge all its contents.
	 *
	 * Deleting a CacheAccess that is not created by this Cache will raise a PreconditionNotMet.
	 *
	 * @param cacheAccess
	 */
	bool deleteAccess(CacheAccess::Ptr cacheAccess)
	{
        return cache_->deleteAccess(cacheAccess);
    }

	// ----------------------------------
	// Getters
	// ----------------------------------

	bool isUpdatesEnabled() const
	{
        return cache_->isUpdatesEnabled();
	}

	std::string getCacheName() const
	{
        return cache_->getCacheName();
	}

	CacheDescription getCacheDescription() const
	{
        return cache_->getCacheDescription();
	}

	std::set<std::string> getTypeNames() const
	{
        return cache_->getTypeNames();
	}

	// ----------------------------------
	// Signaling functions
	// ----------------------------------

	/**
	 * Call OnBeginUpdates on attached CacheListeners
	 */
	void notifyOnBeginUpdates()
	{
        cache_->notifyOnBeginUpdates();
	}

	/**
	 * Call OnEndUpdates on attached CacheListeners
	 */
	void notifyOnEndUpdates()
	{
        cache_->notifyOnEndUpdates();
	}

};

}
