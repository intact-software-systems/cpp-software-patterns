#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

class DLL_STATE Lifespan
    : public Templates::ComparableImmutableType<Duration>
{
public:
    /**
     * User defined lifespan. Default sets the lifespan to maximum.
     */
    Lifespan(Duration dur = Duration::Infinite())
        : Templates::ComparableImmutableType<Duration>(dur)
    { }

    virtual ~Lifespan()
    { }

    const Duration& AsDuration() const
    {
        return this->delegate();
    }

    /**
     * @return The number of milliseconds in lifespan.
     */
    int64 InMillis() const
    {
        return this->delegate().InMillis();
    }

    /**
     * @return infinite Lifespan.
     */
    static Lifespan Infinite()
    {
        return Lifespan(Duration::Infinite());
    }

    /**
     * @return Lifespan equals to minutes.
     */
    static Lifespan Minutes(int minutes)
    {
        return Lifespan(Duration::FromSeconds(minutes * 60));
    }

    /**
     * @return Lifespan equals to one day (24h).
     */
    static Lifespan Day()
    {
        return Lifespan(Duration::FromMilliseconds(24 * 60 * 60 * 1000));
    }
};

}}

