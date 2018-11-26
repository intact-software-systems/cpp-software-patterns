#include "BaseLib/Policy/Reliability.h"

namespace BaseLib { namespace Policy
{

Reliability::Reliability(ReliabilityKind kind)
    : Templates::ComparableImmutableType<ReliabilityKind>(kind)
{}

Reliability::~Reliability()
{}

ReliabilityKind Reliability::Kind() const
{
    return this->Clone();
}

bool Reliability::IsBestEffort() const
{
    return this->delegate() == ReliabilityKind::BestEffort;
}

bool Reliability::IsReliable() const
{
    return this->delegate() == ReliabilityKind::Reliable;
}

Reliability Reliability::BestEffort()
{
    return ReliabilityKind(ReliabilityKind::BestEffort);
}

Reliability Reliability::Reliable()
{
    return Reliability(ReliabilityKind::Reliable);
}

}}
