#pragma once

#include"BaseLib/Duration.h"
#include"BaseLib/Templates/BaseTypes.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

/**
 * @brief
 *
 * This policy is useful for cases where a Topic is expected to have each
 * instance updated periodically. On the publishing side this setting
 * establishes a contract that the application must meet. On the subscribing
 * side the setting establishes a minimum requirement for the remote publishers
 * that are expected to supply the data values. When the Service matches a
 * DataWriter and a DataReader it checks whether the settings are compatible
 * (i.e., offered deadline period<= requested deadline period) if they are not,
 * the two entities are informed (via the listener or condition mechanism)
 * of the incompatibility of the QoS settings and communication will not occur.
 * Assuming that the reader and writer ends have compatible settings, the
 * fulfillment of this contract is monitored by the Service and the application
 * is informed of any violations by means of the proper listener or condition.
 * The value offered is considered compatible with the value requested if and
 * only if the inequality offered deadline period <= requested deadline period
 * evaluates to TRUE. The setting of the DEADLINE policy must be set
 * consistently with that of the TIME_BASED_FILTER.
 * For these two policies to be consistent the settings must be such that
 * deadline period>= minimum_separation.
 */
class DLL_STATE Deadline
        : public Templates::ComparableImmutableType<Duration>
{
public:
    Deadline(Duration period = Duration::Infinite())
        : Templates::ComparableImmutableType<Duration>(period)
    { }
    virtual ~Deadline()
    { }

    // -----------------------------------------
    // Getters
    // -----------------------------------------

    const Duration& Period() const
    {
        return this->delegate();
    }

    int64 InMillis() const
    {
        return this->delegate().InMillis();
    }

    // -----------------------------------------
    // Factory functions
    // -----------------------------------------

    static Deadline FromDuration(Duration period)
    {
        return Deadline(period);
    }

    static Deadline FromMilliseconds(unsigned int msecs)
    {
        return Deadline(Duration::FromMilliseconds(msecs));
    }

    static Deadline FromSeconds(unsigned int secs)
    {
        return Deadline(Duration::FromSeconds(secs));
    }
};

}}

