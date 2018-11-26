#ifndef BaseLib_Policy_Reliability_h_IsIncluded
#define BaseLib_Policy_Reliability_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

enum class DLL_STATE ReliabilityKind
{
    UNKNOWN = 0,
    BestEffort = 1,
    Reliable = 3
};

class DLL_STATE Reliability
    : public Templates::ComparableImmutableType<ReliabilityKind>
{
public:
    Reliability(ReliabilityKind kind = ReliabilityKind::Reliable);
    virtual ~Reliability();

    ReliabilityKind Kind() const;

    bool IsBestEffort() const;
    bool IsReliable() const;

    // --------------------------------------------
    // Static constructors
    // --------------------------------------------

    static Reliability BestEffort();
    static Reliability Reliable();
};

}}

#endif
