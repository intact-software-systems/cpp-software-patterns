#pragma once

#include "CommonDefines.h"

namespace BaseLib {

template <typename Cond, typename Lock>
class WaitConditionType
{
public:
    WaitConditionType() : condition_()
    { }

    ~WaitConditionType()
    { }

    bool wait(Lock* mutex, int64 milliseconds = LONG_MAX)
    {
        return condition_.wait(mutex, milliseconds);
    }

    void wakeAll()
    {
        condition_.wakeAll();
    }

    void wakeOne()
    {
        condition_.wakeOne();
    }

private:
    Cond condition_;
};

}



