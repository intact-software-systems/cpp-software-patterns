#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Thread.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

class DLL_STATE Priority
{
public:
    Priority(ThreadDetails::Priority priority = ThreadDetails::InheritPriority)
        : priority_(priority)
    { }

    ~Priority()
    { }

    const ThreadDetails::Priority& Get() const
    {
        return priority_;
    }

    /**
     * Default priority is ThreadDetails::NormalPriority
     */
    static Priority Default()
    {
        static Priority qosp(ThreadDetails::NormalPriority);

        return qosp;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Priority& qos)
    {
        ostr << TYPE_NAME(qos) << "(" << qos.priority_ << ")";
        return ostr;
    }

private:
    ThreadDetails::Priority priority_;
};

}}

