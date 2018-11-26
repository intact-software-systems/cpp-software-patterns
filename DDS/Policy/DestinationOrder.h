/*
 * DestinationOrder.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_DestinationOrder_h_Included
#define DDS_Policy_DestinationOrder_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
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
class DLL_STATE DestinationOrder : public QosPolicyBase
{
public:
    DestinationOrder(Policy::DestinationOrderKind::Type the_kind)
        : kind_(the_kind)
    { }
    DestinationOrder()
        : kind_(Policy::DestinationOrderKind::BY_SOURCE_TIMESTAMP)
    { }

    DEFINE_POLICY_TRAITS(DestinationOrder, 12, DDS::Policy::RequestedOfferedKind::COMPATIBLE, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32((int32_t) kind_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        kind_ = (Policy::DestinationOrderKind::Type) reader->ReadInt32();
    }

public:
    void SetKind(Policy::DestinationOrderKind::Type the_kind) 	{ kind_ = the_kind; }

    Policy::DestinationOrderKind::Type& GetKind() 				{ return kind_; }
    Policy::DestinationOrderKind::Type 	GetKind() 	const 		{ return kind_; }

public:
    static DestinationOrder SourceTimestamp()
    {
        return DestinationOrder(Policy::DestinationOrderKind::BY_SOURCE_TIMESTAMP);
    }

    static DestinationOrder ReceptionTimestamp()
    {
        return DestinationOrder(Policy::DestinationOrderKind::BY_RECEPTION_TIMESTAMP);
    }

private:
    Policy::DestinationOrderKind::Type kind_;
};

}} //  DDS::Policy

#endif // DDS_Policy_DestinationOrder_h_Included

