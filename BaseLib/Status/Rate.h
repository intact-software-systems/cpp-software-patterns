#pragma once

#include"Fraction.h"
#include"BaseLib/Count.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Status {

/**
 * @brief The Rate class calculates count/time. For example, throughput, packets/sec, bytes/sec, etc.
 *
 * Rename to TimeRate. It is possible with many kinds of rates.
 */
class DLL_STATE TimeRate : public Templates::ComparableImmutableType<Fraction>
{
public:
    TimeRate();
    TimeRate(Count total, Duration duration);
    virtual ~TimeRate();

    const Count   & Total() const;
    const Duration& Timespan() const;

    // -------------------------------------
    // Calculate unit rates
    // -------------------------------------

    long PerMillis() const;
    long PerSecond() const;
    long PerMinute() const;
    long PerHour() const;
    long PerDay() const;

    // ---------------------------------------
    // print out
    // ---------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const TimeRate& count)
    {
        ostr << "TimeRate(" << count.delegate() << "," << count.total_ << "," << count.duration_ << ")";
        return ostr;
    }

private:
    Count    total_;
    Duration duration_;
};

}}
