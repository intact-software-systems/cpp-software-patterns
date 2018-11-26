#pragma once

#include <stdlib.h>
#include <stdint.h>

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib {

/**
 * This class represents a time interval.
 */
class DLL_STATE Duration : public std::chrono::nanoseconds
{
public:
    Duration(int64 nanosecs = std::numeric_limits<int64>::max());
    Duration(std::chrono::nanoseconds nanosecs);
    ~Duration();

    // -----------------------------------------
    // Getters
    // -----------------------------------------

    int64 InMillis() const;
    int64 InMicrosecs() const;
    int64 InNanosecs() const;

    int64 InSecs() const;
    long  InMinutes() const;
    long  InHours() const;

    bool IsInfinite() const;

    // -----------------------------------------
    // Factory functions
    // -----------------------------------------

    static Duration Zero();       // {0, 0}
    static Duration Infinite();   // {0x7fffffff, 0x7fffffff}

    static Duration FromMicroseconds(const int64& micros);
    static Duration FromMilliseconds(const int64& millis);
    static Duration FromSeconds(const int64& secs);
    static Duration FromMinutes(const long& mins);
    static Duration FromHours(const long& hrs);

    // -----------------------------------------
    // friend operators
    // -----------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const Duration& dur)
    {
        ostr << "Duration(millisecs=" << dur.InMillis() << ")";
        return ostr;
    }
};

}
