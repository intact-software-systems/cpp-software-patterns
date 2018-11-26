#ifndef RxData_Cache_Impl_ObjectReference_h_IsIncluded
#define RxData_Cache_Impl_ObjectReference_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/interfaces/ObjectReference.h"
#include"RxData/cache/object/ObjectHomeBase.h"
#include"RxData/cache/object/ObjectState.h"
#include"RxData/cache/object/ObjectRoot.h"

#include"RxData/Export.h"
namespace RxData { namespace Impl
{

/**
 * Holder for the actual cached DATA object. 
 * 
 * @author KVik
 *
 * TODO: Use HOLDER = std::shared_ptr<DATA>, ref ThreadPool to hold the shared data object
 *
 * @param <DATA>
 */
template <typename DATA>
class ObjectReference : public ObjectRoot
                      , public RxData::ObjectReference<DATA>
{
private:
	// ----------------------------------
	// Attributes
	// ----------------------------------
	
	/**
	 * Actual data object
	 */
	DATA dataObject_;
	
public:
	// ----------------------------------
	// Constructor
	// ----------------------------------
    /**
     * Constructs a holder for the cached DATA object.
     */
	ObjectReference(
			const DATA &dataObject,
			ObjectId objectId,
			ObjectState::Type readState,
			ObjectState::Type writeState,
			ObjectHomeBase::Ptr objectHome,
			std::string className,
			CacheBaseInterface::WeakPtr owner)
        : ObjectRoot(objectId, readState, writeState, objectHome, className, owner)
        , dataObject_(dataObject)
	{ }

    CLASS_TRAITS(ObjectReference)

private:
	/**
	 * Used by cloning
	 */
    ObjectReference(Impl::ObjectReference<DATA> *dataObject)
        : ObjectRoot(dataObject->getObjectId(),
                     dataObject->getReadState(),
                     dataObject->getWriteState(),
                     dataObject->getObjectHome(),
                     dataObject->getClassName(),
                     dataObject->getOwner())
        , dataObject_(dataObject->get())
	{ }

public:
	// ----------------------------------
	// Get and set
	// ----------------------------------

    /**
	 * Returns the data held by this class.
	 */
	virtual const DATA& get() const
	{
		updateAccessStates();

		return dataObject_;
	}

	/**
	 * Sets (updates) the data to be held by this class.
	 */
	void set(const DATA &data)
	{
		updateAccessStates();
		
		dataObject_ = data;
	}
	
	// ----------------------------------
	// Creation and clone
	// ----------------------------------
	
    /**
     * @brief createObjectReference
     */
    static typename Impl::ObjectReference<DATA>::Ptr createObjectReference(
			const DATA &data,
			ObjectId key,
            ObjectState::Type readState,
			ObjectState::Type writeState,
            ObjectHomeBase::Ptr objectHome,
			std::string typeName,
			CacheBaseInterface::WeakPtr owner)
    {
        return typename Impl::ObjectReference<DATA>::Ptr( new Impl::ObjectReference<DATA>(
				data,
				key,
				readState,
				writeState,
				objectHome,
				typeName,
				owner));
    }

    /**
     * @brief clone
     * @return
     */
    virtual ObjectRoot::Ptr clone()
    {
        Impl::ObjectReference<DATA>::Ptr cloned = Impl::ObjectReference<DATA>::Ptr( new Impl::ObjectReference<DATA>(this) );

        return cloned->GetPtr();
    }


    /**
     * @brief isNull
     * @return
     */
    virtual bool isNull()
    {
        //if(std::is_pointer<DATA>()) {}
        return false;
    }

//    /**
//     * @brief cloneToCache
//     * @param cache
//     */
//    virtual void cloneToCache(ObjectCache::Ptr cache)
//    {
//        Impl::ObjectReference<DATA> clonedObject = clone();
//        cache->write<DATA>(clonedObject);
//    }

    /**
     * @brief operator ==
     * @param id
     * @return
     */
    virtual bool operator==(const RxData::ObjectReference<DATA> &id) const
    {
        return dataObject_ == id.get();
    }

    /**
     * @brief operator !=
     * @param id
     * @return
     */
    virtual bool operator!=(const RxData::ObjectReference<DATA> &id) const
    {
        return !(dataObject_ == id.get());
    }

    /**
     * @brief operator ==
     * @param id
     * @return
     */
    virtual bool operator==(ObjectRoot::Ptr obj) const
    {
        Impl::ObjectReference<DATA>::Ptr objRef = std::dynamic_pointer_cast<Impl::ObjectReference<DATA>>(obj);

        ASSERT(objRef);

        return dataObject_ == objRef->dataObject_ && getObjectId() == objRef->getObjectId();
    }

    /**
     * @brief operator !=
     * @param id
     * @return
     */
    virtual bool operator!=(ObjectRoot::Ptr obj) const
    {
        Impl::ObjectReference<DATA>::Ptr objRef = std::dynamic_pointer_cast<Impl::ObjectReference<DATA>>(obj);

        ASSERT(objRef);

        return !(dataObject_ == objRef->dataObject_ && getObjectId() == objRef->getObjectId());
    }

    /**
     * @brief operator <<
     * @param ostr
     * @param id
     * @return
     */
    friend std::ostream& operator<<(std::ostream& ostr, typename RxData::ObjectReference<DATA>::Ptr id)
    {
        Impl::ObjectReference<DATA>::Ptr objRef = std::dynamic_pointer_cast<Impl::ObjectReference<DATA>>(id);

        if(objRef)
        {
            ostr << "(" << objRef->get() << ")";
        }
        else
        {
            ostr << "Couldn't cast ObjectReference";
        }

        return ostr;
    }
	
	//@Override
	/*public void update(ObjectReference<DATA> objRef) 
	{
		this->set(objRef.get());
		this->setReadState(objRef.getReadState());
		this->setWriteState(objRef.getWriteState());
	}*/
};

}}

#endif

