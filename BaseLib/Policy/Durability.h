#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE DurabilityKind
{
    VOLATILE,
    TRANSIENT_LOCAL,
    TRANSIENT,
    PERSISTENT
};

/**
 * @abstract
 *
 * The decoupling between DataReader and DataWriter offered by the Publish/Subscribe paradigm allows an application to
 * write data even if there are no current readers on the network. Moreover, a DataReader that joins the network after some
 * data has been written could potentially be interested in accessing the most current values of the data as well as potentially
 * some history. This QoS policy controls whether the Service will actually make data available to late-joining readers. Note
 * that although related, this does not strictly control what data the Service will maintain internally. That is, the Service may
 * choose to maintain some data for its own purposes (e.g., flow control) and yet not make it available to late-joining readers
 * if the DURABILITY QoS policy is set to VOLATILE.
 *
 * The value offered is considered compatible with the value requested if and only if the inequality offered kind >=
 * requested kind evaluates to "TRUE." For the purposes of this inequality, the values of DURABILITY kind are considered
 * ordered such that VOLATILE < TRANSIENT_LOCAL < TRANSIENT < PERSISTENT.
 */
class DLL_STATE Durability
    : public Templates::ComparableImmutableType<DurabilityKind>
{
public:
    Durability(Policy::DurabilityKind kind = Policy::DurabilityKind::VOLATILE)
        : Templates::ComparableImmutableType<DurabilityKind>(kind)
    { }

    Policy::DurabilityKind Kind() const
    {
        return this->Clone();
    }

    static Durability Volatile()
    {
        return Durability(Policy::DurabilityKind::VOLATILE);
    }

    static Durability TransientLocal()
    {
        return Durability(Policy::DurabilityKind::TRANSIENT_LOCAL);
    }

    static Durability Transient()
    {
        return Durability(Policy::DurabilityKind::TRANSIENT);
    }

    static Durability Persistent()
    {
        return Durability(Policy::DurabilityKind::PERSISTENT);
    }
};

}}
