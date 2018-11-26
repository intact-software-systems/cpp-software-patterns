#ifndef BaseLib_Policy_Throughput_h_IsIncluded
#define BaseLib_Policy_Throughput_h_IsIncluded

#include"BaseLib/Status/Rate.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

class DLL_STATE Throughput
        : public Templates::ComparableImmutableType<Status::TimeRate>
{
public:
    Throughput(Status::TimeRate rate)
        : Templates::ComparableImmutableType<Status::TimeRate>(rate)
    {
        ASSERT(rate.Clone() >= 0);
    }
    virtual ~Throughput()
    { }

    Status::TimeRate Rate() const
    {
        return this->Clone();
    }

    // -------------------------------------
    // Factory functions
    // -------------------------------------

    static Throughput PerMillis(int limit)
    {
        return Throughput(Status::TimeRate(limit, Duration::FromMilliseconds(1)));
    }

    static Throughput PerSecond(int limit)
    {
        return Throughput(Status::TimeRate(limit, Duration::FromSeconds(1)));
    }

    static Throughput PerMinute(int limit)
    {
        return Throughput(Status::TimeRate(limit, Duration::FromMinutes(1)));
    }

    static Throughput PerHour(int limit)
    {
        return Throughput(Status::TimeRate(limit, Duration::FromHours(1)));
    }

    // ---------------------------------------
    // print out
    // ---------------------------------------

    friend std::ostream& operator <<(std::ostream &ostr, const Throughput &count)
    {
        ostr << TYPE_NAME(Throughput) << "(" << count.delegate() << ")";
        return ostr;
    }
};

}}

#endif
