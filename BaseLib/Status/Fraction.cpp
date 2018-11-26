#include "BaseLib/Status/Fraction.h"

namespace BaseLib { namespace Status {

Fraction::Fraction()
    : Templates::ComparableImmutableType<float>(0)
{}

Fraction::Fraction(CountInt64 numerator, CountInt64 denominator)
    : Templates::ComparableImmutableType<float>( float(numerator.Number()) / float(denominator.Number()))
    , numerator_(numerator)
    , denominator_(denominator)
{ }

Fraction::~Fraction()
{ }

const CountInt64& Fraction::Numerator() const
{
    return numerator_;
}

const CountInt64& Fraction::Denominator() const
{
    return denominator_;
}

float Fraction::Ratio() const
{
    return this->Clone();
}

}}

