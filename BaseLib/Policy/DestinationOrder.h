#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE DestinationOrderKind
{
    BY_SOURCE_TIMESTAMP,
    BY_RECEPTION_TIMESTAMP
};

/**
 * @abstract
 *
 * This policy controls how each subscriber resolves the final value of a data instance that is written by multiple DataWriter
 * objects (which may be associated with different Publisher objects) running on different nodes.
 *
 * The setting BY_RECEPTION_TIMESTAMP indicates that, assuming the OWNERSHIP policy allows it, the latest
 * received value for the instance should be the one whose value is kept. This is the default value.
 *
 * The setting BY_SOURCE_TIMESTAMP indicates that, assuming the OWNERSHIP policy allows it, a timestamp placed
 * at the source should be used. This is the only setting that, in the case of concurrent same-strength DataWriter objects
 * updating the same instance, ensures all subscribers will end up with the same final value for the instance. The mechanism
 * to set the source timestamp is middleware dependent.
 *
 * The value offered is considered compatible with the value requested if and only if the inequality "offered kind >=
 * requested kind" evaluates to "TRUE". For the purposes of this inequality, the values of DESTINATION_ORDER kind are
 * considered ordered such that BY_RECEPTION_TIMESTAMP < BY_SOURCE_TIMESTAMP.
 */
class DLL_STATE DestinationOrder
    : public Templates::ComparableImmutableType<DestinationOrderKind>
{
public:
    DestinationOrder(Policy::DestinationOrderKind the_kind = DestinationOrderKind::BY_SOURCE_TIMESTAMP)
        : Templates::ComparableImmutableType<DestinationOrderKind>(the_kind)
    { }

    Policy::DestinationOrderKind Kind() const
    {
        return this->Clone();
    }

    static DestinationOrder SourceTimestamp()
    {
        return DestinationOrder(Policy::DestinationOrderKind::BY_SOURCE_TIMESTAMP);
    }

    static DestinationOrder ReceptionTimestamp()
    {
        return DestinationOrder(Policy::DestinationOrderKind::BY_RECEPTION_TIMESTAMP);
    }
};

}}
