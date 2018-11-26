#pragma once

#include"BaseLib/Duration.h"
#include"BaseLib/Templates/BaseTypes.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * @abstract
 *
 * This policy provides a means for the application to indicate to the middleware the "urgency" of the data-communication.
 * By having a non-zero duration the Service can optimize its internal operation.
 *
 * This policy is considered a hint. There is no specified mechanism as to how the service should take advantage of this hint.
 *
 * The value offered is considered compatible with the value requested if and only if the inequality "offered duration <=
 * requested duration" evaluates to "TRUE".
 */
class DLL_STATE LatencyBudget
    : public Templates::ComparableImmutableType<Duration>
{
public:
    LatencyBudget(BaseLib::Duration duration = BaseLib::Duration::Zero())
        : Templates::ComparableImmutableType<Duration>(duration)
    { }

    const BaseLib::Duration& Get() const
    {
        return this->delegate();
    }
};

}}
