#ifndef RxData_Cache_ResourceLimits_h_IsIncluded
#define RxData_Cache_ResourceLimits_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

class DLL_STATE ResourceLimits
{
private:
    // ----------------------------------
    // Attributes
    // ----------------------------------

    int maxSamples_;
    int maxInstances_;
    int maxSamplesPerInstance_;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * Constructor sets the cache resource limits to maximum.
     */
    ResourceLimits()
        : maxSamples_(INT_MAX)
        , maxInstances_(INT_MAX)
        , maxSamplesPerInstance_(INT_MAX)
    { }

    /**
     * User defined resource limits.
     *
     * @param maxSamples
     * @param maxInstances
     * @param maxSamplesPerInstance
     */
    ResourceLimits(int maxSamples, int maxInstances, int maxSamplesPerInstance)
        : maxSamples_(maxSamples)
        , maxInstances_(maxInstances)
        , maxSamplesPerInstance_(maxSamplesPerInstance)
    { }

    /**
     * Set one limit for all attributes: maxSamples, maxInstances and maxSamplesPerInstance.
     *
     * @param fixedLimit
     */
    ResourceLimits(int fixedLimit)
        : maxSamples_(fixedLimit)
        , maxInstances_(fixedLimit)
        , maxSamplesPerInstance_(fixedLimit)

    { }

    ~ResourceLimits()
    { }

    // ----------------------------------
    // Getters
    // ----------------------------------

    int GetMaxSamples() const
    {
        return maxSamples_;
    }

    int GetMaxInstances() const
    {
        return maxInstances_;
    }

    int GetMaxSamplesPerInstance() const
    {
        return maxSamplesPerInstance_;
    }

    // ----------------------------------
    // Static functions
    // ----------------------------------

    /**
     * ResourceLimits with infinite samples
     */
    static ResourceLimits Infinite()
    {
        return ResourceLimits();
    }

    /**
     * ResourceLimits with fixedLimit samples
     */
    static ResourceLimits FixedLimit(int fixedLimit)
    {
        return ResourceLimits(fixedLimit);
    }
};

}

#endif


