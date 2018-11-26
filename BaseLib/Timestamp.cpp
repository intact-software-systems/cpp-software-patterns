#include"BaseLib/Timestamp.h"
#include"BaseLib/Duration.h"

namespace BaseLib {

Timestamp::Timestamp(TimePointNs timestamp)
    : TimePointNs{timestamp}
{ }

Timestamp::~Timestamp()
{ }

Duration Timestamp::TimeSinceEpoch() const
{
    return time_since_epoch();
}

Duration Timestamp::SinceStamped() const
{
    return Duration(Clock::now() - *this);
}

// -----------------------------------------
// Factory functions
// -----------------------------------------

Timestamp Timestamp::Now()
{
    return Timestamp(Clock::now());
}

Timestamp Timestamp::NowPlusMilliseconds(int64 msecs)
{
    auto dur = Duration::FromMilliseconds(msecs);

    return Timestamp(Clock::now() + dur);
}

Timestamp Timestamp::NowMinusMilliseconds(int64 msecs)
{
    auto dur = Duration::FromMilliseconds(msecs);

    return Timestamp(Clock::now() - dur);
}

std::string Timestamp::ToString() const
{
    std::stringstream ostr;
    ostr << "Timestamp(SinceEpoch=" << TimeSinceEpoch() << ")";
    return ostr.str();
}

}
