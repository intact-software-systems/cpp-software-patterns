#include"RxData/cache/object/ObjectRoot.h"

namespace RxData
{
/**
 * @brief ObjectRoot::ObjectRoot
 * @param objectId
 * @param readState
 * @param writeState
 * @param objectHome
 * @param className
 * @param owner
 */
ObjectRoot::ObjectRoot(ObjectId objectId, ObjectState::Type readState, ObjectState::Type writeState, ObjectHomeBase::Ptr objectHome, std::string className, CacheBaseInterface::WeakPtr owner)
    : objectId_(objectId)
    , readState_(readState)
    , writeState_(writeState)
    , lastAccessTime_(Utility::GetCurrentTimeMs())
    , numTimesAccessed_(0)
    , objectHome_(objectHome)
    , className_(className)
    , owner_(owner)
{ }

ObjectRoot::~ObjectRoot()
{

}

/**
 *  Mark the object for destruction (destroy), to be executed during a write operation.
 *
 *  If the object is not located in a writeable CacheAccess, FALSE is returned.
 *
 *  Only access DATA object from outside, not ObjectRoot.
 */
bool ObjectRoot::destroy()
{
    CacheBaseInterface::Ptr owner = owner_.lock();

    // -- debug --
    ASSERT(owner);
    // -- debug --

    if(!owner) return false;

    if(owner->getCacheUsage() == CacheUsage::READ_ONLY)
    {
        return false;
    }

    // -- debug --
    ASSERT(owner->getCacheKind() == CacheKind::CACHEACCESS_KIND);
    // -- debug --

    // -------------------------------------------
    // Update read and write states
    // -------------------------------------------
    if(owner->getCacheUsage() == CacheUsage::READ_WRITE)
    {
        readState_ = ObjectState::OBJECT_DELETED;
    }

    writeState_ = ObjectState::OBJECT_DELETED;

    // -------------------------------------------
    // Should we access ObjectCache and set DATA to null?
    // No. This is done during the write operation!
    // -------------------------------------------

    return true;
}

/**
 * See if the object has been modified by incoming modifications.
 *
 * isModified takes as parameter the scope of the request (i.e., only the object contents,
 * the object and its component objects, the object and all its related objects). In case the
 * object is newly created, this operation returns FALSE 'incoming modifications' should be
 * understood differently for a primary object and for a clone object.
 *
 * - For a primary object, they refer to incoming updates (i.e., coming from the infrastructure).
 *
 * - For a secondary object (cloned), they refer to the modifications applied to the object by
 *   the last CacheAccess::refresh operation.
 *
 * @return
 */
bool ObjectRoot::isModified(ObjectScope::Type ) const
{
    if(readState_ == ObjectState::OBJECT_MODIFIED) return true;
    else if(writeState_ == ObjectState::OBJECT_MODIFIED) return true;

    return false;
}

/**
 * Called whenever the set() or get() DATA is accessed in ObjectReference,
 * and updates the access statistics for the Object. This is used whenever
 * the cache needs to be cleaned, updated, etc, based on policies.
 */
void ObjectRoot::updateAccessStates() const
{
    lastAccessTime_ = Utility::GetCurrentTimeMs();

    numTimesAccessed_++;
}

}
