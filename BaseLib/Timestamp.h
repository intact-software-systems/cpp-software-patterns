#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib {

class Duration;

class DLL_STATE Timestamp : public TimePointNs
{
public:
    Timestamp(TimePointNs timestamp = Clock::now());
    virtual ~Timestamp();

    Duration TimeSinceEpoch() const;
    Duration SinceStamped() const;

    // -----------------------------------------
    // Factory functions
    // -----------------------------------------

    static Timestamp Now();
    static Timestamp NowPlusMilliseconds(int64 msecs);
    static Timestamp NowMinusMilliseconds(int64 msecs);

    std::string ToString() const;

    // -----------------------------------------
    // friend operators
    // -----------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const Timestamp& t)
    {
        ostr << t.ToString();
        return ostr;
    }
};

}
