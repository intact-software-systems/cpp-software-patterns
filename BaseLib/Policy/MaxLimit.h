#pragma once

#include"BaseLib/Policy/CommonDefines.h"
#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Debug.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

template <typename T = long>
class MaxLimit
    : public Templates::ComparableImmutableType<T>
{
public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * User defined limit. Default is 0.
     */
    MaxLimit(LimitKind::Type type, T maxLimit = 0)
        : Templates::ComparableImmutableType<T>(maxLimit)
        , type_(type)
    { }

    /**
     * User defined limit. Default is 0.
     */
    MaxLimit(T maxLimit = 0)
        : Templates::ComparableImmutableType<T>(maxLimit)
        , type_(LimitKind::Type::INCLUSIVE)
    { }

    virtual ~MaxLimit()
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
                return this->Clone() >= limit;
            case LimitKind::EXCLUSIVE:
                return this->Clone() > limit;
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

    static MaxLimit Infinite()
    {
        return MaxLimit(LimitKind::INCLUSIVE, std::numeric_limits<T>::max());
    }

    static MaxLimit LimitTo(LimitKind::Type type, T maxLimit)
    {
        return MaxLimit<T>(type, maxLimit);
    }

    static MaxLimit InclusiveLimitTo(T maxLimit)
    {
        return MaxLimit(LimitKind::INCLUSIVE, maxLimit);
    }

    static MaxLimit InclusiveZero()
    {
        return MaxLimit(LimitKind::INCLUSIVE, 0);
    }

private:
    LimitKind::Type type_;
};

class DLL_STATE MaxLimitULong
    : public MaxLimit<int64>
{
public:
    MaxLimitULong(LimitKind::Type type, long maxLimit = 0)
        : MaxLimit<int64>(type, maxLimit)
    { }

    virtual ~MaxLimitULong()
    { }
};

}}
