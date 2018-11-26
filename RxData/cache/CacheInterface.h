#ifndef RxData_Cache_CacheInterface_h_IsIncluded
#define RxData_Cache_CacheInterface_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/CacheKind.h"
#include"RxData/cache/CacheUsage.h"
#include"RxData/cache/CacheDescription.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * @brief The CacheBaseInterface class
 */
class DLL_STATE CacheBaseInterface
{
public:
    CLASS_TRAITS(CacheBaseInterface)

    // ----------------------------------
	// Getters
	// ----------------------------------

    /**
     * @brief getCacheUsage
     * @return
     */
    virtual CacheUsage::Type getCacheUsage() const = 0;

    /**
     * @brief getCacheKind
     * @return
     */
    virtual CacheKind::Type getCacheKind() const = 0;


    // ----------------------------------
	// Signaling functions
	// ----------------------------------

	/**
	 * Call OnBeginUpdates on attached CacheListeners
	 */
	virtual void notifyOnBeginUpdates() = 0;

	/**
	 * Call OnEndUpdates on attached CacheListeners
	 */
	virtual void notifyOnEndUpdates() = 0;
};

/**
 * CacheBase is the base class for all Cache classes. It contains the common functionality that supports
 * Cache and CacheAccess.
 *
 * The public attributes give:
 *
 * - "The cache_usage indicates whether the cache is intended to support write operations (WRITE_ONLY or
 * READ_WRITE) or not (READ_ONLY)." This attribute is given at creation time and cannot be changed afterwards.
 *
 * - "A list of (untyped) objects that are contained in this CacheBase." To obtain objects by type, see the
 * get_objects method in the typed ObjectHome.
 *
 *
 * The kind describes whether a CacheBase instance represents a Cache or a CacheAccess. It offers methods to:
 *
 * - "Refresh the contents of the Cache with respect to its origins (DCPS in case of a main Cache, Cache in
 * case of a CacheAccess)."
 *
 * @author KVik
 */
class DLL_STATE CacheInterface
{
public:
    CLASS_TRAITS(CacheInterface)

	// ----------------------------------
	// Abstract operations
	// ----------------------------------

	/**
	 * Refresh the contents of the Cache with respect to its origins (DCPS in case of
	 * a main Cache, Cache in case of a CacheAccess).
	 */
	virtual void refresh() = 0;

	/**
	 * Call OnBeginUpdates on attached CacheListeners
	 */
	virtual void notifyOnBeginUpdates() = 0;

	/**
	 * Call OnEndUpdates on attached CacheListeners
	 */
	virtual void notifyOnEndUpdates() = 0;

    /**
	 * Clears out all objects stored in this cache.
	 */
    virtual void clearAllData() = 0;

    /**
     * @brief enableUpdates
     */
    virtual void enableUpdates() = 0;

    /**
     * @brief disableUpdates
     */
    virtual void disableUpdates() = 0;

    /**
     * @brief load
     * @return
     */
    virtual bool load() = 0;

    // ----------------------------------
	// Getters
	// ----------------------------------

    /**
     * @brief isUpdatesEnabled
     * @return
     */
    virtual bool isUpdatesEnabled() const = 0;

    /**
     * @brief getCacheName
     * @return
     */
	virtual std::string getCacheName() const = 0;

    /**
     * @brief getCacheDescription
     * @return
     */
	virtual CacheDescription getCacheDescription() const = 0;

    /**
     * @brief getTypeNames
     * @return
     */
	virtual std::set<std::string> getTypeNames() const = 0;

    /**
     * @brief getCacheUsage
     * @return
     */
    virtual CacheUsage::Type getCacheUsage() const = 0;

    /**
     * @brief getCacheKind
     * @return
     */
    virtual CacheKind::Type getCacheKind() const = 0;
};

}

#endif // CACHEINTERFACE_H
