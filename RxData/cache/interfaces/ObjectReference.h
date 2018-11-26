#ifndef RxData_Cache_ObjectReference_h_IsIncluded
#define RxData_Cache_ObjectReference_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Interface for the data holder class used in ObjectCache<DATA>.  
 * 
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA>
class ObjectReference
{
public:
    CLASS_TRAITS(ObjectReference)

	/**
	 * Retrieves the data from this holder class.
	 * 
	 * @return
	 */
	//virtual DATA get() const = 0;


    /**
     * @brief get
     * @return
     */
    virtual const DATA& get() const = 0;

//    /**
//     * @brief createObjectReference
//     */
//    virtual ObjectReference<DATA>::Ptr createObjectReference(
//			DATA data,
//			ObjectId key,
//            ObjectState::Type readState,
//			ObjectState::Type writeState,
//            ObjectHomeBase::Ptr objectHome,
//			std::string typeName,
//			CacheInterface::WeakPtr owner) = 0;

    /**
     * @brief operator ==
     * @param id
     * @return
     */
    virtual bool operator==(const ObjectReference<DATA> &) const = 0;

    /**
     * @brief operator !=
     * @param id
     * @return
     */
    virtual bool operator!=(const ObjectReference<DATA> &) const = 0;
};

}

#endif
