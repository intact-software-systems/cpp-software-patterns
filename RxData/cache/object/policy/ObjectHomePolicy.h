#ifndef RxData_Cache_ObjectHomePolicy_h_IsIncluded
#define RxData_Cache_ObjectHomePolicy_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/policy/Ejection.h"
#include"RxData/cache/policy/Lifespan.h"
#include"RxData/cache/policy/ResourceLimits.h"

#include"RxData/Export.h"
namespace RxData
{
/**
 * The policies used to administer the objects belonging to an ObjectHome.
 *
 * @author KVik
 */
class DLL_STATE ObjectHomePolicy
{
private:

    // ----------------------------------
    // Attributes
    // ----------------------------------

    Lifespan 		lifespan;
    ResourceLimits 	resourceLimits;
    Ejection 		ejection;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * Default constructor.
     */
    ObjectHomePolicy()
        : lifespan(Lifespan::Infinite())
        , resourceLimits(ResourceLimits::Infinite())
        , ejection(Ejection::LeastRecentlyUsed())
    { }


    /**
     * @param lifespan
     * @param resourceLimits
     */
    ObjectHomePolicy(
            Lifespan lifespan,
            ResourceLimits resourceLimits,
            Ejection ejection)
        : lifespan(lifespan)
        , resourceLimits(resourceLimits)
        , ejection(ejection)
    { }

    // ----------------------------------
    // Getters
    // ----------------------------------

    Lifespan getLifespan() const
    {
        return lifespan;
    }

    ResourceLimits getResourceLimits() const
    {
        return resourceLimits;
    }

    Ejection getEjection() const
    {
        return ejection;
    }

    /**
     * Returns default ObjectHomePolicy.
     *
     * @return
     */
    static ObjectHomePolicy Default()
    {
        return ObjectHomePolicy();
    }
};

}

#endif

