#ifndef RxData_Cache_CacheAccess_h_IsIncluded
#define RxData_Cache_CacheAccess_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/CacheAccessInterface.h"
#include"RxData/cache/Cache.h"
#include"RxData/cache/CacheDescription.h"
#include"RxData/cache/Contract.h"

#include"RxData/cache/object/ObjectHome.h"
#include"RxData/cache/object/ObjectCache.h"
#include"RxData/cache/object/ObjectRoot.h"

#include"RxData/cache/scope/ObjectScope.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * CacheAccess is a class that represents a way to globally manipulate cache objects in isolation
 * 
 * @author KVik
 */
class DLL_STATE CacheAccess : public CacheBase
                            , public CacheAccessInterface // public CacheReaderListener
{
private:
    // ----------------------------------
	// Typedefs
	// ----------------------------------
    typedef IList<Contract>         ListContract;
    typedef IMap<std::string, int>  MapTypeNames;

	// ----------------------------------
	// Attributes
	// ----------------------------------
	/**
	 * The owner of the Cache (owner)
	 */
	Cache::WeakPtr 	owner_;
	
	/**
	 * The contracted objects (contracts). This is the list of all Contracts that are 
	 * attached to this CacheAccess.
	 */
    ListContract contracts_;

	/**
	 * A list of names that represents the types for which the CacheAccess contains at 
	 * least one object (type_names).
	 */
    MapTypeNames typeNamesToCount_;
	
public:
	// ----------------------------------
	// Constructor
	// ----------------------------------
	/**
	 * @param cacheUsage
	 * @param cacheKind
	 * @param owner
	 */
    CacheAccess(CacheUsage::Type cacheUsage, CacheKind::Type cacheKind, Cache::WeakPtr owner);
    virtual ~CacheAccess();

    CLASS_TRAITS(CacheAccess)
	
	// ----------------------------------
	// Callbacks
	// ----------------------------------
	
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
	bool OnDataAvailable(DATA data, KEY key)
	{
		// -----------------------------------------------
		// Can a CacheAccess ever be only READ ONLY?
		// -----------------------------------------------
		if(getCacheUsage() == CacheUsage::READ_ONLY) return false;
		
        Cache::Ptr owner = owner_.lock();

        if(!owner)
        {
            IWARNING() << "Owner cache is NULL! Ignoring data";
            return false;
        }

		// -----------------------------------------------
		// CacheAccess allows writes
		// -----------------------------------------------
		typename ObjectHome<DATA,KEY>::Ptr objectHome = owner->getOrCreateHome<DATA,KEY>(data.getClass().getName());
		
		// -- debug --
		ASSERT(objectHome);
		// -- debug --
		
		ObjectCache::Ptr objectCache = getOrCreateObjectCache(objectHome->getTypeName());
		
		// -- debug --
		ASSERT(objectCache);
		// -- debug --
		
        bool doNotifyListeners = false;

        return objectHome->onDataAvailable(data, key, objectCache, doNotifyListeners);
	}	
	
	// ----------------------------------
	// Class operations
	// ----------------------------------

	/**
	 * Refresh the contents of the Cache with respect to its origins.
	 * 
	 * Cache in case of a CacheAccess.
	 * 
	 */
	virtual void refresh();
	
	/**
	 * Write objects. If the CacheAccess::cacheUsage allows write operation, those objects can be modified 
	 * and/or new objects created for that access and eventually all the performed modifications written 
	 * for publications.
	 *
	 * @return success or not
	 */
	virtual bool write();
	
	/**
	 * Detach all contracts (including the contracted cache Objects themselves) from the CacheAccess (purge).
	 */
	virtual void purge();
	
	/**
	 * This method defines a contract that covers the specified object with all the objects 
	 * in its specified scope. When a CacheAccess is refreshed, all contracted objects will be 
	 * cloned into it. The contracted object must be located in the Cache that owns the 
	 * CacheAccess. If this is not the case, NULL is returned.
	 * 
	 * @param dataReference
	 * @param scope
	 * @param depth
	 * 
	 * @return invalid contract if dataReference not in owner-Cache, Contract otherwise.
	 */	
	virtual Contract createContract(ObjectRoot::Ptr dataReference, ObjectScope::Type scope, int depth);
	
	/**
	 * This method deletes a contract from the CacheAccess. When the CacheAccess is refreshed, 
	 * the objects covered by the specified contract will no longer appear in the CacheAccess 
	 * (unless also covered in another Contract). The specified Contract must be attached to 
	 * this CacheAccess, otherwise FALSE is returned.
	 * 
	 * @param contract
	 * @return 
	 */
	virtual bool deleteContract(Contract contract);
	
	// ----------------------------------
	// Getters
	// ----------------------------------
	
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

protected:
    /**
	 * Call OnBeginUpdates on attached CacheListeners
	 */
	virtual void notifyOnBeginUpdates()
    {
        // no operation because no listeners
    }

	/**
	 * Call OnEndUpdates on attached CacheListeners
	 */
	virtual void notifyOnEndUpdates()
    {
        // no operation because no listeners
    }

private:
	// ----------------------------------
	// Private functions
	// ----------------------------------	
		
	void incTypeNameCount(std::string typeName);

	void decTypeNameCount(std::string typeName);
};

}

#endif

