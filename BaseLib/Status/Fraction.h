#pragma once

#include"BaseLib/Count.h"
#include"BaseLib/Templates/BaseTypes.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status {

/**
 * @brief The Fraction class represents a numerator/denominator.
 */
class DLL_STATE Fraction : public Templates::ComparableImmutableType<float>
{
public:
    Fraction();
    Fraction(CountInt64 numerator, CountInt64 denominator);
    virtual ~Fraction();

    const CountInt64& Numerator() const;
    const CountInt64& Denominator() const;

    float Ratio() const;

    friend std::ostream& operator<<(std::ostream& ostr, const Fraction& count)
    {
        ostr << "Fraction(" << count.numerator_ << "/" << count.denominator_ << " = " << count.delegate() << ")";
        return ostr;
    }

private:
    CountInt64 numerator_;
    CountInt64 denominator_;
};

}}
