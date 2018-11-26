#include"RxData/cache/object/ObjectCache.h"

namespace RxData
{

/**
 * @brief ObjectCache::ObjectCache
 * @param typeName
 * @param owner
 */
ObjectCache::ObjectCache(std::string typeName, CacheBaseInterface::WeakPtr owner)
    : objects_()
    , typeName_(typeName)
    , owner_(owner)
{ }

/**
 * @brief ObjectCache::~ObjectCache
 */
ObjectCache::~ObjectCache()
{

}

/**
 * @brief ObjectCache::clearAll
 */
void ObjectCache::clearAll()
{
    clearAllPrivate();
}

/**
 * @brief ObjectCache::size
 * @return
 */
size_t ObjectCache::size() const
{
    return sizePrivate();
}

/**
 * @brief ObjectCache::isEmpty
 * @return
 */
bool ObjectCache::isEmpty() const
{
    return isEmptyPrivate();
}

/**
 * @brief ObjectCache::copyObjectToCache
 * @param key
 * @param destination
 * @return
 */
bool ObjectCache::copyObjectToCache(ObjectId key, ObjectCache::Ptr destination)
{
    // -- pre conditions --
    if(!destination) return false;
    if(typeName_ != destination->typeName_) return false;
    // -- pre conditions --

    ObjectRoot::Ptr obj = getObject(key);
    if(!obj) return false;

    ObjectRoot::Ptr clone = obj->clone();

    return destination->write(clone);
}

/**
 * @brief ObjectCache::updateObjectState
 * @param isDataIdentical
 * @param isDataNull
 * @param objRef
 * @return
 */
bool ObjectCache::updateObjectState(bool isDataIdentical, bool isDataNull, ObjectRoot::Ptr objRef)
{
    // -- debug --
    // Q: Is it an allowed state to have data = null?
    // A: Yes, it is allowed to clear out data.
    // -- debug --

    if(!objRef) return false;

    // ----------------------------------------------------
    // Determine whether existing data and new data are identical
    // ----------------------------------------------------
    //		bool isDataIdentical = (objRef->get() == data);

    //		if(objRef->get())
    //		{
    //			isDataIdentical = (objRef->get() == data);
    //		}
    //		else if(!data) // objRef->get() is also null
    //		{
    //			isDataIdentical = true;
    //		}

    // ----------------------------------------------------
    // If data is identical then only update object states
    // ----------------------------------------------------
    if(isDataIdentical)
    {
        // ----------------------------------------------------
        // readState implies:
        // owner->getCacheUsage() == (CacheUsage.READ_ONLY || CacheUsage.READ_WRITE)
        // ----------------------------------------------------
        if(objRef->getReadState() != ObjectState::OBJECT_VOID)
        {
            // -- debug --
            //assert (owner->getCacheUsage() == CacheUsage.READ_ONLY || owner->getCacheUsage() == CacheUsage.READ_WRITE);
            // -- debug --

            if(isDataNull)
                objRef->setReadState(ObjectState::OBJECT_DELETED);
            else
                objRef->setReadState(ObjectState::OBJECT_NOT_MODIFIED);
        }

        // ----------------------------------------------------
        // writeState implies:
        // owner->getCacheUsage() == (CacheUsage.WRITE_ONLY || CacheUsage.READ_WRITE)
        // ----------------------------------------------------
        if(objRef->getWriteState() != ObjectState::OBJECT_VOID)
        {
            // -- debug --
            //assert (owner->getCacheUsage() == CacheUsage.WRITE_ONLY || owner->getCacheUsage() == CacheUsage.READ_WRITE);
            // -- debug --

            if(isDataNull)
                objRef->setWriteState(ObjectState::OBJECT_DELETED);
            else
                objRef->setWriteState(ObjectState::OBJECT_NOT_MODIFIED);
        }
    }
    // ----------------------------------------------------
    // If data is not identical then update data and object states
    // ----------------------------------------------------
    else // if(isDataIdentical == false)
    {
        // ----------------------------------------------------
        // Update data regardless of CacheUsage -> Checked earlier
        // ----------------------------------------------------
        //objRef->set(data);

        // ----------------------------------------------------
        // readState implies:
        // owner->getCacheUsage() == (CacheUsage.READ_ONLY || CacheUsage.READ_WRITE)
        // ----------------------------------------------------
        if(objRef->getReadState() != ObjectState::OBJECT_VOID)
        {
            // -- debug --
            //assert (owner->getCacheUsage() == CacheUsage.READ_ONLY || owner->getCacheUsage() == CacheUsage.READ_WRITE);
            // -- debug --

            if(isDataNull)
                objRef->setReadState(ObjectState::OBJECT_DELETED);
            else
                objRef->setReadState(ObjectState::OBJECT_MODIFIED);
        }

        // ----------------------------------------------------
        // writeState implies:
        // owner->getCacheUsage() == (CacheUsage.WRITE_ONLY || CacheUsage.READ_WRITE)
        // ----------------------------------------------------
        if(objRef->getWriteState() != ObjectState::OBJECT_VOID)
        {
            // -- debug --
            //assert (owner->getCacheUsage() == CacheUsage.WRITE_ONLY || owner->getCacheUsage() == CacheUsage.READ_WRITE);
            // -- debug --

            if(isDataNull)
                objRef->setWriteState(ObjectState::OBJECT_DELETED);
            else
                objRef->setWriteState(ObjectState::OBJECT_MODIFIED);
        }
    }

    return true;
}

/**
 * Write ObjectRoot to cache.
 */
bool ObjectCache::write(ObjectRoot::Ptr newObj)
{
    ASSERT(newObj);

    MutexLocker lock(&mutex_);

    ObjectRoot::Ptr obj = this->getObject<ObjectId>(newObj->getObjectId());

    // --------------------------------------------------
    // if exists then update data and meta-data of object
    // --------------------------------------------------
    if(obj)
    {
        bool isDataIdentical = (obj == newObj);
        //bool isUpdated =
        ObjectCache::updateObjectState(isDataIdentical, false, obj); //newObj->isNull(), obj);

        if(!isDataIdentical)
        {
            newObj->setReadState(obj->getReadState());
            newObj->setWriteState(obj->getWriteState());
            newObj->updateAccessStates();

            bool isInserted = objects_.put(newObj->getObjectId(), newObj);

            // -- debug --
            ASSERT(isInserted);
            // -- debug --
        }
    }
    // --------------------------------------------------
    // if new then create object-holder and add to cache
    // --------------------------------------------------
    else
    {
        bool isInserted = objects_.put(newObj->getObjectId(), newObj);

        // -- debug --
        ASSERT(isInserted);
        // -- debug --
    }

    return true;
}

}
