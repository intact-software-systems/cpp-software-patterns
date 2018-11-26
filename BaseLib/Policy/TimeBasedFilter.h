#pragma once

#include"BaseLib/Duration.h"
#include"BaseLib/Templates/BaseTypes.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

class DLL_STATE TimeBasedFilter
    : public Templates::ComparableImmutableType<Duration>
{
public:
    TimeBasedFilter(Duration minSeparation = Duration::Zero())
        : Templates::ComparableImmutableType<Duration>(minSeparation)
    { }

    Duration MinSeparation() const
    {
        return this->Clone();
    }

    static TimeBasedFilter WithMinSeparation(Duration minSeparation)
    {
        return TimeBasedFilter(minSeparation);
    }
};

}}
