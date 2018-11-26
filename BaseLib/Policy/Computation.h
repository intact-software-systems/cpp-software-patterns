#ifndef BaseLib_Policy_Computation_h_IsIncluded
#define BaseLib_Policy_Computation_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

/**
 * @brief The ComputationKind
 */
namespace ComputationKind
{
    enum Type
    {
        SEQUENTIAL,
        PARALLEL,
        CONCURRENT
    };
}

/**
 * @brief The Computation class
 *
 * TODO: Concurrent = Interleaving, needs rate of interleaving.
 *
 * TODO: "Level of concurrency" - represent as integer defining "how many concurrent tasks are allowed".
 */
class DLL_STATE Computation
        : public Templates::ComparableImmutableType<ComputationKind::Type>
{
public:
    Computation(ComputationKind::Type kind = ComputationKind::SEQUENTIAL)
        : Templates::ComparableImmutableType<ComputationKind::Type>(kind)
    {}
    virtual ~Computation()
    {}

    // -----------------------------------------
    // Getters
    // -----------------------------------------

    ComputationKind::Type Kind() const
    {
        return this->Clone();
    }

    bool IsSequential() const
    {
        return this->delegate() == ComputationKind::SEQUENTIAL;
    }

    bool IsParallel() const
    {
        return this->delegate() == ComputationKind::PARALLEL;
    }

    bool IsConcurrent() const
    {
        return this->delegate() == ComputationKind::CONCURRENT;
    }

    // --------------------------------------------
    // Static constructors
    // --------------------------------------------

    static Computation Sequential()
    {
        return Computation(ComputationKind::SEQUENTIAL);
    }

    static Computation Parallel()
    {
        return Computation(ComputationKind::PARALLEL);
    }

    static Computation Concurrent()
    {
        return Computation(ComputationKind::CONCURRENT);
    }
};

}}

#endif
