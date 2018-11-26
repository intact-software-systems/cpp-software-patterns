#ifndef RxData_Cache_ObjectId_h_IsIncluded
#define RxData_Cache_ObjectId_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Parameterized object identifier, which is used by ObjectRoot.
 *
 * @author KVik
 */
class ObjectId : public Templates::ComparableImmutableType<Templates::AnyKey, NullMutex>
{
public:
	/**
	 * Parameterized object identifier, which is used by ObjectRoot.
	 *
	 * @param key
	 */
	template <typename KEY>
    ObjectId(KEY key)
        : Templates::ComparableImmutableType<Templates::AnyKey, NullMutex>(Templates::AnyKey(key))
	{ }

    ObjectId()
        : Templates::ComparableImmutableType<Templates::AnyKey, NullMutex>()
    {}

    /**
     * @brief get
     * @return
     */
    template <typename KEY>
    KEY get() const
    {
        const KEY *key = BaseLib::Templates::AnyKeyCast<KEY>(this->operator ->());

        ASSERT(key);

        if(key == NULL)
            return KEY();

        return *key;
    }

    /**
     * @brief operator <<
     * @param ostr
     * @param id
     * @return
     */
    friend std::ostream& operator<<(std::ostream& ostr, const ObjectId &id)
    {
        ostr << id.delegate().type().name();
        return ostr;
    }
};

}

#endif

