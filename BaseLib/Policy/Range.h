#ifndef BaseLib_Policy_Range_h_IsIncluded
#define BaseLib_Policy_Range_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Policy/MinLimit.h"
#include"BaseLib/Policy/MaxLimit.h"

namespace BaseLib { namespace Policy
{

/**
 * @brief The Range class
 */
template <typename T = long>
class Range
{
public:
    Range(MinLimit<T> minLimit, MaxLimit<T> maxLimit)
        : minLimit_(minLimit)
        , maxLimit_(maxLimit)
    {}
    virtual ~Range()
    {}

    bool IsInRange(long)
    {
        return false;
    }

    static Range<T> Inclusive(MinLimit<T> min, MaxLimit<T> max)
    {
        return Range<T>(min, max);
    }

    static Range<T> Exclusive(MinLimit<T> min, MaxLimit<T> max)
    {
        return Range<T>(min, max);
    }

private:
    bool isInInclusiveRange(const T &number)
    {
        return (number >= minLimit_ && number <= maxLimit_);
    }

    bool isInExclusiveRange(const T &number)
    {
        return !isInInclusiveRange(number);
    }

private:
    MinLimit<T> minLimit_;
    MaxLimit<T> maxLimit_;
};

}}

#endif
