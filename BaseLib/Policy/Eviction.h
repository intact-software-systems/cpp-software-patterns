#ifndef BaseLib_Policy_Eviction_h_IsIncluded
#define BaseLib_Policy_Eviction_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

/**
 * Enumerated value
 */
namespace EvictionKind
{
    enum Type
    {
        LEAST_RECENTLY_USED,
        LEAST_FREQUENTLY_USED
    };
}

/**
 * Policy to decide which objects to evict from the cache once ResourceLimits
 * or Lifespan policies trigger.
 *
 * Also known as Replacement policy in cache algorithms.
 *
 * TODO: Add "number of items to evict"?
 *
 * @author KVik
 */
class DLL_STATE Eviction : public Templates::ComparableImmutableType<EvictionKind::Type>
{
public:
    /**
     * User defined ejection strategy, otherwise the ejection strategy is default.
     *
     * @param ejectionKind
     */
    Eviction(EvictionKind::Type kind = EvictionKind::LEAST_RECENTLY_USED)
        : Templates::ComparableImmutableType<EvictionKind::Type>(kind)
    { }

    virtual ~Eviction()
    { }

    EvictionKind::Type Kind() const
    {
        return this->Clone();
    }

    // ----------------------------------
    // Static convenience functions
    // ----------------------------------

    /**
     * @return Ejection with policy to evict objects based on least recently used
     */
    static Eviction LeastRecentlyUsed()
    {
        return Eviction(EvictionKind::LEAST_RECENTLY_USED);
    }

    /**
     * @return Ejection with policy to evict objects based on least frequently used
     */
    static Eviction LeastFrequentlyAccessed()
    {
        return Eviction(EvictionKind::LEAST_FREQUENTLY_USED);
    }

    /**
     * Default is least recently used.
     */
    static Eviction Default()
    {
        return LeastRecentlyUsed();
    }
};

}}

#endif

