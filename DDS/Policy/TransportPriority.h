/*
 * TransportPriority.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_TransportPriority_h_Included
#define DDS_Policy_TransportPriority_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * The purpose of this QoS is to allow the application to take advantage of
 * transports capable of sending messages with different priorities.
 * This policy is considered a hint. The policy depends on the ability of the
 * underlying transports to set a priority on the messages they send.
 * Any value within the range of a 32-bit signed integer may be chosen;
 * higher values indicate higher priority. However, any further interpretation
 * of this policy is specific to a particular transport and a particular
 * implementation of the Service. For example, a particular transport is
 * permitted to treat a range of priority values as equivalent to one another.
 * It is expected that during transport configuration the application would
 * provide a mapping between the values of the TRANSPORT_PRIORITY set on
 * DataWriter and the values meaningful to each transport. This mapping would
 * then be used by the infrastructure when propagating the data written by
 * the DataWriter.
 */
class DLL_STATE TransportPriority : public QosPolicyBase
{
public:
    TransportPriority(uint32_t prio)
        : value_(prio)
    { }
    TransportPriority()
        : value_(0)
    { }
    virtual ~TransportPriority()
    { }

    DEFINE_POLICY_TRAITS(TransportPriority, 20, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32(value_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        value_ = reader->ReadInt32();
    }

public:
    void SetValue(uint32_t prio) 			{ value_ = prio; }
    uint32_t GetValue() 			const 	{ return value_; }

private:
    uint32_t value_;
};

}}

#endif
