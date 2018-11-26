#pragma once

#include"BaseLib/Duration.h"
#include"BaseLib/Templates/BaseTypes.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE OwnershipKind
{
    EXCLUSIVE,
    SHARED
};

/**
 * @abstract
 *
 * This policy controls whether the Service allows multiple DataWriter objects to update the same instance (identified by
 * Topic + key) of a data-object.
 *
 * There are two kinds of OWNERSHIP selected by the setting of the kind: SHARED and EXCLUSIVE.
 */
class DLL_STATE Ownership
    : public Templates::ComparableImmutableType<OwnershipKind>
{
public:
    Ownership(Policy::OwnershipKind kind = Policy::OwnershipKind::SHARED)
        : Templates::ComparableImmutableType<OwnershipKind>(kind)
    { }

    virtual ~Ownership()
    { }

    Policy::OwnershipKind GetKind() const
    {
        return this->Clone();
    }

    static Ownership Exclusive()
    {
        static Ownership ownership(Policy::OwnershipKind::EXCLUSIVE);
        return ownership;
    }

    static Ownership Shared()
    {
        static Ownership ownership(Policy::OwnershipKind::SHARED);
        return ownership;
    }
};

}}
