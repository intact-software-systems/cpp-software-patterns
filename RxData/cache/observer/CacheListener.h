#pragma once

#include"RxData/CommonDefines.h"
#include"RxData/cache/CacheDescription.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 *
 * CacheListener is an interface that must be implemented by the application in order to be made aware 
 * of the arrival of updates on the cache.
 * 
 * In between, the updates are reported on home or selection listeners. Section 8.1.6.4, "Listeners Activation," 
 * on page 208 describes which notifications are performed and in what order.
 * 
 * @author KVik
 */
class DLL_STATE CacheObserver
{

public:
    CLASS_TRAITS(CacheObserver)

	/**
	 * Indicates that updates are following. Actual modifications in the cache will be performed only 
	 * when exiting this method (assuming that updates_enabled is true).
	 */
	virtual void OnBeginUpdates(CacheDescription description) = 0;
	
	/**
	 * Indicates that no more update is foreseen.
	 */
	virtual void OnEndUpdates(CacheDescription description) = 0;
	
	/**
	 * Indicates that the Cache has switched to automatic update mode. Incoming data will now trigger 
	 * the corresponding Listeners.
	 */
	virtual void OnUpdatesEnabled(CacheDescription description) = 0;
	
	/**
	 * Indicates that the Cache has switched to manual update mode. Incoming data will no longer trigger 
	 * the corresponding Listeners, and will only be taken into account during the next refresh operation.
	 */
	virtual void OnUpdatesDisabled(CacheDescription description) = 0;
};

}
