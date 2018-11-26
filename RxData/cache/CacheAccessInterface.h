#ifndef RxData_Cache_CacheAccessInterface_h_IsIncluded
#define RxData_Cache_CacheAccessInterface_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/Contract.h"
#include"RxData/cache/CacheUsage.h"
#include"RxData/cache/CacheKind.h"

#include"RxData/cache/object/ObjectRoot.h"
#include"RxData/cache/scope/ObjectScope.h"

#include"RxData/Export.h"

namespace RxData
{

class DLL_STATE CacheAccessInterface
{
public:
    CLASS_TRAITS(CacheAccessInterface)

    /**
     * @brief write
     * @return
     */
    virtual bool write() = 0;

    /**
     * @brief purge
     */
    virtual void purge() = 0;

    /**
     * @brief createContract
     * @param dataReference
     * @param scope
     * @param depth
     * @return
     */
    virtual Contract createContract(ObjectRoot::Ptr dataReference, ObjectScope::Type scope, int depth) = 0;

    /**
     * @brief deleteContract
     * @param contract
     * @return
     */
    virtual bool deleteContract(Contract contract) = 0;

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
};

}


#endif // CACHEACCESSINTERFACE_H
