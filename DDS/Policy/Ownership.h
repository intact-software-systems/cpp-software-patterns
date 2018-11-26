/*
 * Ownership.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Ownership_h_Included
#define DDS_Policy_Ownership_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @abstract
 *
 * This policy controls whether the Service allows multiple DataWriter objects to update the same instance (identified by
 * Topic + key) of a data-object.
 *
 * There are two kinds of OWNERSHIP selected by the setting of the kind: SHARED and EXCLUSIVE.
 */
class DLL_STATE Ownership : public QosPolicyBase
{
public:
    Ownership(Policy::OwnershipKind::Type kind)
        : kind_(kind)
    { }
    Ownership()
        : kind_(Policy::OwnershipKind::SHARED)
    { }

    DEFINE_POLICY_TRAITS(Ownership, 6, DDS::Policy::RequestedOfferedKind::COMPATIBLE, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32((int32_t) kind_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        kind_ = (Policy::OwnershipKind::Type) reader->ReadInt32();
    }

public:
    void SetKind(Policy::OwnershipKind::Type kind) 			{ kind_ = kind; }

    Policy::OwnershipKind::Type GetKind() 				const 	{ return kind_; }

public:
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

private:
    Policy::OwnershipKind::Type kind_;
};

}}

#endif
