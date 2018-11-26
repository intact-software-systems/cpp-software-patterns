#include"BaseLib/Duration.h"

namespace BaseLib {

Duration::Duration(int64 nanosecs)
    : std::chrono::nanoseconds{nanosecs}
{ }

Duration::Duration(std::chrono::nanoseconds nanosecs)
    : std::chrono::nanoseconds(nanosecs)
{ }

Duration::~Duration()
{ }

int64 Duration::InMillis() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(*this).count();
}

int64 Duration::InMicrosecs() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(*this).count();
}

int64 Duration::InNanosecs() const
{
    return this->count();
}

int64 Duration::InSecs() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(*this).count();
}

long Duration::InMinutes() const
{
    return std::chrono::duration_cast<std::chrono::minutes>(*this).count();
}

long Duration::InHours() const
{
    return std::chrono::duration_cast<std::chrono::hours>(*this).count();
}

bool Duration::IsInfinite() const
{
    return InMillis() >= std::numeric_limits<long>::max();
}

// -----------------------------------------
// Factory functions
// -----------------------------------------

Duration Duration::Infinite()
{
    return Duration(std::numeric_limits<long long>::max());
}

Duration Duration::Zero()
{
    return Duration(0);
}

Duration Duration::FromMicroseconds(const int64& micros)
{
    if(micros >= LONG_MAX)
    {
        return Duration(std::chrono::nanoseconds::max());
    }
    else
    {
        auto dur = std::chrono::microseconds{micros};
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(dur));
    }
}

Duration Duration::FromMilliseconds(const int64& millisecs)
{
    if(millisecs >= LONG_MAX)
    {
        return Duration(std::chrono::nanoseconds::max());
    }
    else
    {
        auto dur = std::chrono::milliseconds{millisecs};
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(dur));
    }
}

Duration Duration::FromSeconds(const int64& secs)
{
    if(secs >= LONG_MAX)
    {
        return Duration(std::chrono::nanoseconds::max());
    }
    else
    {
        auto dur = std::chrono::seconds{secs};
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(dur));
    }
}

Duration Duration::FromMinutes(const long& mins)
{
    if(mins >= LONG_MAX)
    {
        return Duration(std::chrono::nanoseconds::max());
    }
    else
    {
        auto dur = std::chrono::minutes{mins};
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(dur));
    }
}

Duration Duration::FromHours(const long& hrs)
{
    if(hrs >= LONG_MAX)
    {
        return Duration(std::chrono::nanoseconds::max());
    }
    else
    {
        auto dur = std::chrono::hours{hrs};
        return Duration(std::chrono::duration_cast<std::chrono::nanoseconds>(dur));
    }
}

}

