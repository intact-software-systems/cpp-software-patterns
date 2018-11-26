#ifndef RxData_Cache_ObjectRoot_h_IsIncluded
#define RxData_Cache_ObjectRoot_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/cache/CacheInterface.h"
#include"RxData/cache/scope/ObjectScope.h"

#include"RxData/cache/object/ObjectHomeBase.h"
#include"RxData/cache/object/ObjectState.h"
#include"RxData/cache/object/ObjectId.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * Abstract root class for all the application-defined classes.
 *
 * ObjectRoot is the abstract root for any cache class. It brings all the properties
 * that are needed for cache management. ObjectRoot is used to represent either objects
 * that are in the Cache (also called primary objects) or clones that are attached to a
 * CacheAccess (also called secondary objects). Secondary objects refer to a primary one
 * with which they share the ObjectReference.
 *
 * @author KVik
 */
class DLL_STATE ObjectRoot : public ENABLE_SHARED_FROM_THIS(ObjectRoot)
{
private:
    // ----------------------------------
    // Attributes
    // ----------------------------------
    /**
     * The identity of the object
     */
    ObjectId objectId_;

    /**
     * Lifecycle states
     *
     * TODO: Create Pair<ObjectState, lastAccessTime> readState, writeState;
     */
    ObjectState::Type readState_;
    ObjectState::Type writeState_;

    /**
     * Last access time
     */
    mutable int64 lastAccessTime_;

    /**
     * Number of times accessed
     */
    mutable int64 numTimesAccessed_;

    /**
     * Objects related home.
     */
    ObjectHomeBase::Ptr objectHome_;
    std::string 		className_;

    /**
     * The cache the object belongs to (owner), this can be either a
     * Cache or a CacheAccess.
     */
    CacheBaseInterface::WeakPtr owner_;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------
    ObjectRoot(
            ObjectId objectId,
            ObjectState::Type readState,
            ObjectState::Type writeState,
            ObjectHomeBase::Ptr objectHome,
            std::string className,
            CacheBaseInterface::WeakPtr owner);

    virtual ~ObjectRoot();

    CLASS_TRAITS(ObjectRoot)

    /**
     * @brief GetPtr
     * @return
     */
    ObjectRoot::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

public:

    /**
     * @brief clone
     * @return
     */
    virtual ObjectRoot::Ptr clone() = 0;

    /**
     * @brief isNull
     * @return
     */
    virtual bool isNull() = 0;

    /**
     * @brief operator ==
     * @param id
     * @return
     */
    virtual bool operator==(ObjectRoot::Ptr) const = 0;

    /**
     * @brief operator !=
     * @param id
     * @return
     */
    virtual bool operator!=(ObjectRoot::Ptr) const = 0;

    // ----------------------------------
    // Class operations
    // ----------------------------------

    /**
     *  Mark the object for destruction (destroy), to be executed during a write operation.
     *
     *  If the object is not located in a writeable CacheAccess, FALSE is returned.
     *
     *  Only access DATA object from outside, not ObjectRoot.
     */
    bool destroy();

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
    bool isModified(ObjectScope::Type) const;

    /**
     * Get which contained objects have been modified (which_contained_modified). This method
     * returns a list of descriptions for the relations that point to the modified objects
     * (each description includes the name of the relation and if appropriate the index or key
     * that corresponds to the modified contained object).
     *
     * @return
     */
    // TODO: Implement whichContainedModified
    //List<RelationDescription> whichContainedModified()
    //{
    //	return null;
    //}

//protected:
    /**
     * Called whenever the set() or get() DATA is accessed in ObjectReference,
     * and updates the access statistics for the Object. This is used whenever
     * the cache needs to be cleaned, updated, etc, based on policies.
     */
    void updateAccessStates() const;

public:
    // ----------------------------------
    // Getters
    // ----------------------------------

    ObjectId getObjectId() const
    {
        return objectId_;
    }

    template <typename KEY>
    KEY getObjectKey() const
    {
        return objectId_.get<KEY>();
    }

    ObjectState::Type getReadState() const
    {
        return readState_;
    }

    ObjectState::Type getWriteState() const
    {
        return writeState_;
    }

    //template <typename DATA>
    //ObjectHome<DATA>::Ptr getObjectHome() const
    //{
    //  return DYNAMIC_POINTER_CAST(ObjectHome<DATA>, objectHome_);
    //}

    ObjectHomeBase::Ptr getObjectHome() const
    {
        return objectHome_;
    }

    std::string getClassName() const
    {
        return className_;
    }

    CacheBaseInterface::WeakPtr getOwner() const
    {
        return owner_;
    }

    int64 getLastAccessTime() const
    {
        return lastAccessTime_;
    }

    int64 getNumTimesAccessed() const
    {
        return numTimesAccessed_;
    }

    // ----------------------------------
    // Setters
    // ----------------------------------

    void setReadState(ObjectState::Type readState)
    {
        readState_ = readState;
    }

    void setWriteState(ObjectState::Type writeState)
    {
        writeState_ = writeState;
    }
};

}

#endif

