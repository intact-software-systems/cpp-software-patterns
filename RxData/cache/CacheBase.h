#ifndef RxData_Cache_CacheBase_h_IsIncluded
#define RxData_Cache_CacheBase_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/AccessObjectCache.h"
#include"RxData/cache/object/ObjectCache.h"

#include"RxData/Export.h"
namespace RxData
{

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
class DLL_STATE CacheBase : public AccessObjectCache
                          , public ENABLE_SHARED_FROM_THIS(CacheBase)
{
private:
    // ----------------------------------
	// Typedefs
	// ----------------------------------
    typedef IMap<std::string, ObjectCache::Ptr> MapObjectCache;

    // ----------------------------------
	// Attributes
	// ----------------------------------
	
    CacheUsage::Type cacheUsage_;
    CacheKind::Type cacheKind_;
	
	/**
	 * Map of the ObjectCaches, where String represents the className/typeName 
	 */
    MapObjectCache objects_;
	
public:
	// ----------------------------------
	// Constructor
	// ----------------------------------

    CacheBase(CacheUsage::Type cacheUsage, CacheKind::Type cacheKind);

    virtual ~CacheBase();

    CLASS_TRAITS(CacheBase)

    /**
     * @brief GetPtr
     * @return
     */
    CacheBase::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

	// ----------------------------------
	// Operations
	// ----------------------------------
	
	/**
	 * Thread-safe function to get or create an ObjectCache as identified by the typeName.
	 * 
	 * @param 	typeName
	 * @return 	the new ObjectCache
	 */
	virtual ObjectCache::Ptr getOrCreateObjectCache(std::string typeName);
	
	/**
	 * Thread-safe function to remove and clear the ObjectCache identified by the typeName.
	 * 
	 * No side-effects when calling this function and ObjectCache does not exist.
	 * 
	 * @param 	typeName
	 * @return 	the removed ObjectCache
	 */
    virtual ObjectCache::Ptr removeAndClearObjectCache(std::string typeName);

protected:
	/**
	 * Clears out all objects stored in this cache.
	 */
	void clearAllObjects();
	
public:
	// ----------------------------------
	// Getters
	// ----------------------------------
	
    virtual CacheUsage::Type getCacheUsage() const;

    virtual CacheKind::Type getCacheKind() const;
};

}

#endif



