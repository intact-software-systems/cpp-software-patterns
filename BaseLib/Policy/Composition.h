#ifndef BaseLib_Policy_Composition_h_IsIncluded
#define BaseLib_Policy_Composition_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

/**
 * Use to combine the results of commands (aggregate, etc). Aka. function composition.
 */
namespace CompositionKind
{
    enum Type
    {
        NO,
        AGGREGATE,
        INDIVIDUAL,
        MINIMUM,
        MAXIMUM
    };
}

/**
 * @brief The Composition class
 */
class DLL_STATE Composition : public Templates::ComparableImmutableType<CompositionKind::Type>
{
public:
    Composition(CompositionKind::Type kind = CompositionKind::INDIVIDUAL)
        : Templates::ComparableImmutableType<CompositionKind::Type>(kind)
    {}
    virtual ~Composition()
    {}

    // -----------------------------------------
    // Getters
    // -----------------------------------------

    CompositionKind::Type Kind() const
    {
        return this->Clone();
    }

    bool IsNo() const
    {
        return this->delegate() == CompositionKind::NO;
    }

    bool IsAggregate() const
    {
        return this->delegate() == CompositionKind::AGGREGATE;
    }

    bool IsIndividual() const
    {
        return this->delegate() == CompositionKind::INDIVIDUAL;
    }

    bool IsMinimum() const
    {
        return this->delegate() == CompositionKind::MINIMUM;
    }

    bool IsMaximum() const
    {
        return this->delegate() == CompositionKind::MAXIMUM;
    }

public:
    // --------------------------------------------
    // Static constructors
    // --------------------------------------------

    static Composition No()
    {
        return Composition(CompositionKind::NO);
    }

    static Composition Aggregate()
    {
        return Composition(CompositionKind::AGGREGATE);
    }

    static Composition Individual()
    {
        return Composition(CompositionKind::INDIVIDUAL);
    }

    static Composition Minimum()
    {
        return Composition(CompositionKind::MINIMUM);
    }

    static Composition Maximum()
    {
        return Composition(CompositionKind::MAXIMUM);
    }
};

}}

#endif
