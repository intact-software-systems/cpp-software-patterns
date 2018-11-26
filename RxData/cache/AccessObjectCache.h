#ifndef RxData_Cache_AccessObjectCache_h_IsIncluded
#define RxData_Cache_AccessObjectCache_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/cache/object/ObjectCache.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * @brief The AccessObjectCache class
 */
class DLL_STATE AccessObjectCache : public CacheBaseInterface
{
public:
    CLASS_TRAITS(AccessObjectCache)

    /**
     * @brief getOrCreateObjectCache
     * @param typeName
     * @return
     */
    virtual ObjectCache::Ptr getOrCreateObjectCache(std::string typeName) = 0;

    /**
     * @brief removeAndClearObjectCache
     * @param typeName
     * @return
     */
    virtual ObjectCache::Ptr removeAndClearObjectCache(std::string typeName) = 0;

//    // ----------------------------------
//	// Getters
//	// ----------------------------------

//    /**
//     * @brief getCacheUsage
//     * @return
//     */
//    virtual CacheUsage::Type getCacheUsage() const = 0;

//    /**
//     * @brief getCacheKind
//     * @return
//     */
//    virtual CacheKind::Type getCacheKind() const = 0;


//    // ----------------------------------
//	// Signaling functions
//	// ----------------------------------

//	/**
//	 * Call OnBeginUpdates on attached CacheListeners
//	 */
//	virtual void notifyOnBeginUpdates() = 0;

//	/**
//	 * Call OnEndUpdates on attached CacheListeners
//	 */
//	virtual void notifyOnEndUpdates() = 0;
};

}

#endif
