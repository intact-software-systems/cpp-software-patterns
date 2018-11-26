#ifndef RxData_Cache_CacheDescription_h_IsIncluded
#define RxData_Cache_CacheDescription_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * A description of the cache. Typically the type-name of the cached object.
 *
 * Note: Adding a domain (i.e., group) may be applicable in scenarios where you cache the same type-name
 * but want different caches.
 *
 * @author KVik
 */
class DLL_STATE CacheDescription
{
private:
    // ----------------------------------
    // Attributes
    // ----------------------------------

    std::string name_;
    //final Domain domain;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * Only constructor to this immutable object.
     */
    CacheDescription(std::string name);
    ~CacheDescription();

    static CacheDescription Empty()
    {
        return CacheDescription("");
    }

    static CacheDescription CreateUnique()
    {
        return CacheDescription(GUUID::Create().toString());
    }

    // ----------------------------------
    // Getters
    // ----------------------------------

    std::string getName() const;

    /**
     * @brief operator ==
     * @param other
     * @return
     */
    bool operator==(const CacheDescription &other) const;

    /**
     * @brief operator !=
     * @param other
     * @return
     */
    bool operator!=(const CacheDescription &other) const;

    /**
     * @brief operator !=
     * @param other
     * @return
     */
    bool operator<(const CacheDescription &other) const;

    /**
     * @brief operator !=
     * @param other
     * @return
     */
    bool operator>(const CacheDescription &other) const;


    /**
     * @brief operator <<
     * @param ostr
     * @param item
     * @return
     */
    friend std::ostream& operator<<(std::ostream& ostr, const CacheDescription &item)
    {
        ostr << item.name_;
        return ostr;
    }
};

}

#endif


