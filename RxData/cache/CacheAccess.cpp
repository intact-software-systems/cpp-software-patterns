#include"RxData/cache/CacheAccess.h"

namespace RxData {

/**
 * @brief CacheAccess::CacheAccess
 * @param cacheUsage
 * @param cacheKind
 * @param owner
 */
CacheAccess::CacheAccess(CacheUsage::Type cacheUsage, CacheKind::Type cacheKind, Cache::WeakPtr owner)
    : CacheBase(cacheUsage, cacheKind)
    , owner_(owner)
    , contracts_()
    , typeNamesToCount_()
{ }

/**
 * @brief CacheAccess::~CacheAccess
 */
CacheAccess::~CacheAccess()
{}

/**
 * Refresh the contents of the Cache with respect to its origins.
 *
 * Cache in case of a CacheAccess.
 */
void CacheAccess::refresh()
{
    MutexTypeLocker<ListContract> lock(&contracts_);

    // -----------------------------------------------
    // No contracts means no data to refresh
    // -----------------------------------------------
    if(contracts_.empty()) return;

    Cache::Ptr owner = owner_.lock();

    if(!owner)
    {
        IWARNING() << "Owner cache is NULL! Ignoring data";
        return;
    }

    // --------------------------------------------------
    // Iterate through contracts and read from owner Cache into this
    // --------------------------------------------------
    for(auto & contract : contracts_)
    {
        // --------------------------------------------------
        // Get the contracted object
        // --------------------------------------------------
        ObjectRoot::Ptr obj = contract.getContractedObject();

        // -- debug --
        ASSERT(obj);
        // -- debug --

        // --------------------------------------------------
        // Get object in main Cache
        // --------------------------------------------------
        ObjectCache::Ptr ownerObjectCache = owner->getOrCreateObjectCache(obj->getClassName());

        if(ownerObjectCache->containsKey<ObjectId>(obj->getObjectId()))
        {
            // ------------------------------------------------------------------------
            // Use CacheAccess.ObjectCache to either clone or update state of Object
            // Same operation whether exists or not, the ObjectCache takes care of the update.
            // ------------------------------------------------------------------------
            ownerObjectCache->copyObjectToCache(obj->getObjectId(), getOrCreateObjectCache(obj->getClassName()));
        }
    }
}

/**
 * Write objects. If the CacheAccess::cacheUsage allows write operation, those objects can be modified
 * and/or new objects created for that access and eventually all the performed modifications written
 * for publications.
 *
 * @return success or not
 */
bool CacheAccess::write()
{
    Cache::Ptr owner = owner_.lock();

    if(!owner)
    {
        IWARNING() << "Owner cache is NULL! Ignoring data";
        return false;
    }

    // -----------------------------------------------
    // Can a CacheAccess ever be only READ ONLY?
    // -----------------------------------------------
    if(getCacheUsage() == CacheUsage::READ_ONLY) return false;

    // -----------------------------------------------
    // No contracts means no data to write
    // -----------------------------------------------
    if(contracts_.empty()) return true;

    // --------------------------------------------------
    // Iterate through contracts and write to owner Cache
    // TODO: How to trigger the Cache listeners?
    // --------------------------------------------------
    {
        MutexTypeLocker<ListContract> lock(&contracts_);

        for(auto & contract : contracts_)
        {
            // --------------------------------------------------
            // Get the contracted object
            // --------------------------------------------------
            ObjectRoot::Ptr obj = contract.getContractedObject();

            // -- debug --
            ASSERT(obj);
            // -- debug --

            // ------------------------------------------------------------------------
            // Use CacheAccess.ObjectCache to either clone or update state of Object
            // Same operation whether exists or not, the ObjectCache takes care of the update.
            // ------------------------------------------------------------------------
            ObjectCache::Ptr objectCache = getOrCreateObjectCache(obj->getClassName());

            if(objectCache->containsKey<ObjectId>(obj->getObjectId()))
            {
                // --------------------------------------------------
                // Write object to owner Cache
                // --------------------------------------------------
                bool isCopied = objectCache->copyObjectToCache(obj->getObjectId(), owner->getOrCreateObjectCache(obj->getClassName()) );

                ASSERT(isCopied);
            }
            else
            {
                // -- debug --
                // TODO: Can this be NULL?
                // Answer: I guess it should be allowed to write NULL to main Cache?
                // assert(objRef != NULL);
                // -- debug --
            }
        }
    }

    return true;
}

/**
 * Detach all contracts (including the contracted cache Objects themselves) from the CacheAccess (purge).
 */
void CacheAccess::purge()
{
    // --------------------------------
    // clear out contracts
    // --------------------------------
    {
        MutexTypeLocker<MapTypeNames> lock(&typeNamesToCount_);
        typeNamesToCount_.clear();
    }

    // ----------------------------------
    // Clear contracts
    // ----------------------------------
    {
        MutexTypeLocker<ListContract> lock(&contracts_);
        contracts_.clear();
    }

    // --------------------------------
    // Clear the CacheObjects
    // --------------------------------
    clearAllObjects();
}

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
Contract CacheAccess::createContract(ObjectRoot::Ptr dataReference, ObjectScope::Type scope, int depth)
{
    Cache::Ptr owner = owner_.lock();

    if(!owner)
    {
        IWARNING() << "Owner cache is NULL! Ignoring data";
        return Contract::Invalid();
    }

    // --------------------------------------------------
    // Get owner's object cache
    // --------------------------------------------------
    ObjectCache::Ptr ownerObjectCache = owner->getOrCreateObjectCache(dataReference->getClassName());

    // ---------------------------------------------------------------
    // If the object is not in main cache then return Invalid contract
    // ---------------------------------------------------------------
    if(!ownerObjectCache->containsKey<ObjectId>(dataReference->getObjectId()))
        return Contract::Invalid();

    // --------------------------------------------------
    // Create the contract and add to contracts
    //
    // TODO: Check if identical contract exists?
    // --------------------------------------------------
    Contract contract(depth, scope, dataReference);

    {
        MutexTypeLocker<ListContract> lock(&contracts_);
        contracts_.push_back(contract);
    }

    // ----------------------------------
    // Update typename counters
    // ----------------------------------
    incTypeNameCount(dataReference->getClassName());

    return contract;
}

/**
 * This method deletes a contract from the CacheAccess. When the CacheAccess is refreshed,
 * the objects covered by the specified contract will no longer appear in the CacheAccess
 * (unless also covered in another Contract). The specified Contract must be attached to
 * this CacheAccess, otherwise FALSE is returned.
 *
 * @param contract
 * @return
 */
bool CacheAccess::deleteContract(Contract contract)
{
    bool isRemoved = false;

    // ----------------------------------
    // synchronize access to contracts
    // ----------------------------------
    {
        MutexTypeLocker<ListContract> lock(&contracts_);

        size_t sz = contracts_.size();

        contracts_.remove(contract);

        isRemoved = (sz > contracts_.size());
    }

    // ----------------------------------
    // Update typename counters
    // ----------------------------------
    ObjectRoot::Ptr contractedObject = contract.getContractedObject();

    // -- debug --
    ASSERT(contractedObject);
    // -- debug --

    if(contractedObject)
    {
        decTypeNameCount(contractedObject->getClassName());
    }

    return isRemoved;
}

/**
 * @brief CacheAccess::getCacheUsage
 * @return
 */
CacheUsage::Type CacheAccess::getCacheUsage() const
{
    return CacheBase::getCacheUsage();
}

/**
 * @brief CacheAccess::getCacheKind
 * @return
 */
CacheKind::Type CacheAccess::getCacheKind() const
{
    return CacheBase::getCacheKind();
}

// --------------------------------------------------------------------
// Private houskeeping functions
// --------------------------------------------------------------------

void CacheAccess::incTypeNameCount(std::string typeName)
{
    MutexTypeLocker<MapTypeNames> lock(&typeNamesToCount_);

    int typeCount = typeNamesToCount_.get(typeName);
    if(typeCount != 0)
    {
        ++typeCount;
        typeNamesToCount_.put(typeName, typeCount);
    }
    else //if(typeCount == NULL)
    {
        typeNamesToCount_.put(typeName, 0);
    }
}

void CacheAccess::decTypeNameCount(std::string typeName)
{
    MutexTypeLocker<MapTypeNames> lock(&typeNamesToCount_);

    int typeCount = typeNamesToCount_.get(typeName);

    // ---------------------------------------------
    // Decrement the count, ensure it is not below 0
    // ---------------------------------------------
    typeCount = std::max(0, typeCount - 1);

    if(typeCount == 0)
    {
        typeNamesToCount_.remove(typeName);
    }
    else
    {
        typeNamesToCount_.put(typeName, typeCount);
    }
}


}
