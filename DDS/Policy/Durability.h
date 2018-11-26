/*
 * Durability.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Durability_h_Included
#define DDS_Policy_Durability_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
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
class DLL_STATE Durability : public QosPolicyBase
{
public:
    Durability(Policy::DurabilityKind::Type the_kind)
        : kind_(the_kind)
    { }
    Durability()
        : kind_(Policy::DurabilityKind::VOLATILE)
    { }

    DEFINE_POLICY_TRAITS(Durability, 2, DDS::Policy::RequestedOfferedKind::COMPATIBLE, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32((int32_t) kind_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        kind_ = (Policy::DurabilityKind::Type) reader->ReadInt32();
    }

public:
    void SetDurability(Policy::DurabilityKind::Type the_kind) { kind_ = the_kind; }
    void SetKind(Policy::DurabilityKind::Type the_kind) { kind_ = the_kind; }

    Policy::DurabilityKind::Type GetDurability() 	const { return kind_; }
    Policy::DurabilityKind::Type GetKind() 			const { return kind_; }

public:
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

public:
    Policy::DurabilityKind::Type kind_;
};

}} //  DDS::Policy

#endif // DDS_Policy_Durability_h_Included

