#ifndef BaseLib_Policy_SizeLimits_h_IsIncluded
#define BaseLib_Policy_SizeLimits_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

class DLL_STATE SizeLimits
{
private:
    // ----------------------------------
    // Attributes
    // ----------------------------------

    int maxBytes_;
    int minBytes_;
    int maxBytesPerInstance_;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * Constructor sets the resource limits to maximum.
     */
    SizeLimits()
        : maxBytes_(INT_MAX)
        , minBytes_(INT_MAX)
        , maxBytesPerInstance_(INT_MAX)
    { }

    /**
     * User defined resource limits.
     */
    SizeLimits(int maxBytes, int minBytes, int maxBytesPerInstance)
        : maxBytes_(maxBytes)
        , minBytes_(minBytes)
        , maxBytesPerInstance_(maxBytesPerInstance)
    { }

    /**
     * Set one limit for all attributes: maxBytes, minBytes and maxBytesPerInstance.
     */
    SizeLimits(int fixedLimit)
        : maxBytes_(fixedLimit)
        , minBytes_(fixedLimit)
        , maxBytesPerInstance_(fixedLimit)

    { }

    ~SizeLimits()
    { }

    // ----------------------------------
    // Getters
    // ----------------------------------

    int GetMaxBytes() const
    {
        return maxBytes_;
    }

    int GetMinBytes() const
    {
        return minBytes_;
    }

    int GetMaxBytesPerInstance() const
    {
        return maxBytesPerInstance_;
    }

    // ----------------------------------
    // Static functions
    // ----------------------------------

    /**
     * @return SizeLimits with infinite samples
     */
    static SizeLimits Infinite()
    {
        return SizeLimits();
    }

    /**
     * @brief FixedLimit
     * @param fixedLimit
     * @return SizeLimits with fixedLimit samples
     */
    static SizeLimits FixedLimit(int fixedLimit)
    {
        return SizeLimits(fixedLimit);
    }


    /**
     * @brief MinMax
     * @param bytesLimit
     * @return SizeLimits with equal minimum, maximum and per instance size
     */
    static SizeLimits MinMax(int bytesLimit)
    {
        return SizeLimits(bytesLimit, bytesLimit, bytesLimit);
    }

    /**
     * @brief MinMaxAndMaxPerInstance
     * @param bytesLimit
     * @param bytesPerInstance
     * @return SizeLimits with equal minimu and maximum size and explicit bytes per instance size. Can be used with UDP Datagrams.
     */
    static SizeLimits MinMaxAndMaxPerInstance(int bytesLimit, int bytesPerInstance)
    {
        return SizeLimits(bytesLimit, bytesLimit, bytesPerInstance);
    }
};

}}

#endif


