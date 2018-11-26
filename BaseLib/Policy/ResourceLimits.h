#ifndef BaseLib_Policy_ResourceLimits_h_IsIncluded
#define BaseLib_Policy_ResourceLimits_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
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
     */
    ResourceLimits(int maxSamples, int maxInstances, int maxSamplesPerInstance)
        : maxSamples_(maxSamples)
        , maxInstances_(maxInstances)
        , maxSamplesPerInstance_(maxSamplesPerInstance)
    { }

    /**
     * Set one limit for all attributes: maxSamples, maxInstances and maxSamplesPerInstance.
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

    int MaxSamples() const
    {
        return maxSamples_;
    }

    int MaxInstances() const
    {
        return maxInstances_;
    }

    int MaxSamplesPerInstance() const
    {
        return maxSamplesPerInstance_;
    }

    // ----------------------------------
    // Static functions
    // ----------------------------------

    /**
     * @return ResourceLimits with infinite samples
     */
    static ResourceLimits Infinite()
    {
        return ResourceLimits();
    }

    /**
     * @brief FixedLimit
     * @param fixedLimit
     * @return ResourceLimits with fixedLimit samples
     */
    static ResourceLimits FixedLimit(int fixedLimit)
    {
        return ResourceLimits(fixedLimit);
    }
};

}}

#endif


