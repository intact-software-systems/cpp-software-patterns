#include "BaseLib/Status/Rate.h"

namespace BaseLib {
namespace Status {

Fraction CalcFractionMicroSec(Count total, Duration duration)
{
    int64 den = std::max<int64>(duration.InMicrosecs(), 1ll);
    return Fraction(total.Number(), den);
}

TimeRate::TimeRate()
    : Templates::ComparableImmutableType<Fraction>()
    , total_(Count::Zero())
    , duration_(Duration::Zero())
{ }

TimeRate::TimeRate(Count total, Duration duration)
    : Templates::ComparableImmutableType<Fraction>(CalcFractionMicroSec(total, duration))
    , total_(total)
    , duration_(duration)
{ }

TimeRate::~TimeRate()
{ }

const Count& TimeRate::Total() const
{
    return total_;
}

const Duration& TimeRate::Timespan() const
{
    return duration_;
}

// -------------------------------------
// Calculate unit rates
// -------------------------------------

long TimeRate::PerMillis() const
{
    return std::lround(this->delegate().Ratio() * 1000);
}

long TimeRate::PerSecond() const
{
    return std::lround(this->delegate().Ratio() * 1000000);
}

long TimeRate::PerMinute() const
{
    return PerSecond() * 60;
}

long TimeRate::PerHour() const
{
    return PerMinute() * 60;
}

long TimeRate::PerDay() const
{
    return PerHour() * 24;
}

}
}
