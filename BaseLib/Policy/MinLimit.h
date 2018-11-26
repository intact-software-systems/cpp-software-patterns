#ifndef BaseLib_Policy_MinLimit_h_IsIncluded
#define BaseLib_Policy_MinLimit_h_IsIncluded

#include"BaseLib/Policy/CommonDefines.h"
#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Debug.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

template <typename T = long>
class MinLimit
        : public Templates::ComparableImmutableType<T>
{
public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * User defined limit. Default is 0.
     */
    MinLimit(LimitKind::Type type, T minLimit = 0)
        : Templates::ComparableImmutableType<T>(minLimit)
        , type_(type)
    { }

    virtual ~MinLimit()
    { }

    // ----------------------------------
    // Getters and checkers
    // ----------------------------------

    T Limit() const
    {
        return this->Clone();
    }

    LimitKind::Type Type() const
    {
        return type_;
    }

    bool IsWithin(T limit) const
    {
        switch(type_)
        {
        case LimitKind::INCLUSIVE:
            return this->Clone() <= limit;
        case LimitKind::EXCLUSIVE:
            return this->Clone() < limit;
        default:
            IWARNING() << "LimitKind not recognized";
            break;
        }

        return this->Clone() >= limit;
    }

    bool IsInclusive() const
    {
        return type_ == LimitKind::INCLUSIVE;
    }

    bool IsExclusive() const
    {
        return type_ == LimitKind::EXCLUSIVE;
    }

    // ----------------------------------
    // Static functions
    // ----------------------------------

    /**
     * @return MinLimit set to LONG_MAX
     */
    template <typename U>
    static MinLimit<U> Infinite()
    {
        return MinLimit<U>(LimitKind::INCLUSIVE, std::numeric_limits<U>::max());
    }

    /**
     * @brief LimitTo
     * @param MinLimit
     * @return
     */
    template <typename U>
    static MinLimit<U> LimitTo(LimitKind::Type type, U minLimit)
    {
        return MinLimit<U>(type, minLimit);
    }

    /**
     * @brief Zero
     * @return
     */
    template <typename U>
    static MinLimit<U> InclusiveZero()
    {
        return MinLimit<U>(LimitKind::INCLUSIVE, 0);
    }

private:
    LimitKind::Type type_;
};

}}

#endif


