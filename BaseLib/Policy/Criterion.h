#pragma once

#include"BaseLib/Policy/MinLimit.h"
#include"BaseLib/Policy/Reaction.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * Usage:
 * - Execution criterion for commands
 * - Circuit breaker
 */
class DLL_STATE Criterion
{
public:
    enum class DLL_STATE Kind : char
    {
        ALL           = 'A',
        MINIMUM       = 'M',
        UNCONDITIONAL = 'U'
    };

public:
    Criterion(MinLimit<int> limit, Reaction reaction, Kind kind, Interval withinInterval)
        : limit_(limit)
        , withinInterval_(withinInterval)
        , reaction_(reaction)
        , kind_(kind)
    { }

    ~Criterion()
    { }

    // -----------------------------------------
    // Getters
    // -----------------------------------------

    const MinLimit<int>& Limit() const
    {
        return limit_;
    }

    const Interval& InInterval() const
    {
        return withinInterval_;
    }

    const Reaction& GetReaction() const
    {
        return reaction_;
    }

    Kind GetKind() const
    {
        return kind_;
    }

    bool IsAll() const
    {
        return Kind::ALL == kind_;
    }

    bool IsMinimum() const
    {
        return Kind::MINIMUM == kind_;
    }

    bool IsUnconditional() const
    {
        return Kind::UNCONDITIONAL == kind_;
    }

    // --------------------------------------------
    // Static constructors
    // --------------------------------------------

    static Criterion All()
    {
        return Criterion(
            MinLimit<int>::LimitTo<int>(LimitKind::INCLUSIVE, std::numeric_limits<int>::max()),
            Reaction::Stop(),
            Kind::ALL,
            Policy::Interval::Infinite()
        );
    }

    static Criterion MinimumAnd(MinLimit<int> minLimit, Reaction reaction)
    {
        return Criterion(
            minLimit,
            reaction,
            Kind::MINIMUM,
            Policy::Interval::Infinite()
        );
    }

    static Criterion Unconditional()
    {
        return Criterion(
            MinLimit<int>::LimitTo<int>(LimitKind::INCLUSIVE, 0),
            Reaction::Resume(),
            Kind::UNCONDITIONAL,
            Policy::Interval::Infinite()
        );
    }

    // -----------------------------------------
    // friend operators
    // -----------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const Criterion& t)
    {
        ostr << "Criterion(" << t.limit_.Limit() << "," << t.reaction_ << "," << char(t.kind_) << ")";
        return ostr;
    }

private:
    MinLimit<int> limit_;
    Interval      withinInterval_;
    Reaction      reaction_;
    Kind          kind_;
};

}}
