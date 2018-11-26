#pragma once

#include"BaseLib/Timestamp.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Debug.h"
#include"BaseLib/Export.h"

namespace BaseLib {

class ScopedTimer
{
public:
    ScopedTimer(IDebug debug)
        : start_(Timestamp::Now())
        , debug_(debug)
    { }

    ~ScopedTimer()
    {
        Duration elapsed = Timestamp::Now() - start_;
        debug_ << "Time elapsed: " << elapsed.InMillis() << " ms";
    }

    Duration Elapsed() const
    {
        Duration elapsed = Timestamp::Now() - start_;
        return Duration::FromMilliseconds(elapsed.InMillis());
    }

private:
    Timestamp start_;
    IDebug    debug_;
};

class ScopedCompareTimer
{
public:
    ScopedCompareTimer(IDebug debug, Duration comparison)
        : start_(Timestamp::Now())
        , comparison_(comparison)
        , debug_(debug)
    { }

    ~ScopedCompareTimer()
    {
        Duration elapsed = Timestamp::Now() - start_;

        if(comparison_ != elapsed)
        {
            debug_ << "Elapsed " << elapsed.InMillis() << " != " << comparison_.InMillis() << " ms";
        }
    }

    Duration Elapsed() const
    {
        Duration elapsed = Timestamp::Now() - start_;
        return Duration::FromMilliseconds(elapsed.InMillis());
    }

private:
    Timestamp start_;
    Duration  comparison_;
    IDebug    debug_;
};

}
