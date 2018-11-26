#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Duration.h"
#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

class DLL_STATE Timeout
    : public Templates::ComparableImmutableType<Duration>
{
public:
    Timeout(Duration duration)
        : Templates::ComparableImmutableType<Duration>(duration)
    { }

    virtual ~Timeout()
    { }

    // ----------------------------------
    // Getters
    // ----------------------------------

    const Duration& AsDuration() const
    {
        return this->delegate();
    }

    int64 InMillis() const
    {
        return this->delegate().InMillis();
    }

    // ----------------------------------
    // factory functions
    // ----------------------------------

    static Timeout FromMilliseconds(int timeoutMs)
    {
        return Timeout(Duration::FromMilliseconds(timeoutMs));
    }

    static Timeout FromSeconds(int timeoutSec)
    {
        return Timeout(Duration::FromSeconds(timeoutSec));
    }

    static Timeout FromMinutes(int timeoutMinutes)
    {
        return Timeout(Duration::FromMinutes(timeoutMinutes));
    }

    static Timeout FromHours(int timeoutHours)
    {
        return Timeout(Duration::FromHours(timeoutHours));
    }

    static Timeout FromDays(int timeoutDays)
    {
        return Timeout(Duration::FromHours(timeoutDays * 24));
    }

    static Timeout No()
    {
        return Timeout(Duration::Infinite());
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Timeout& timeout)
    {
        ostr << TYPE_NAME(timeout) << ": " << timeout.delegate();
        return ostr;
    }
};

}}
