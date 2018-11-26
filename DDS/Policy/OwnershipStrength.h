/*
 * OwnershipStrength.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_OwnershipStrength_h_Included
#define DDS_Policy_OwnershipStrength_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @abstract
 *
 * This QoS policy should be used in combination with the OWNERSHIP policy. It only applies to the situation case where
 * OWNERSHIP kind is set to EXCLUSIVE.
 *
 * The value of the OWNERSHIP_STRENGTH is used to determine the ownership of a data-instance (identified by the
 * key). The arbitration is performed by the DataReader. The rules used to perform the arbitration are described in
 * Section 7.1.3.9.2, "EXCLUSIVE kind", on page 112.
 */
class DLL_STATE OwnershipStrength : public QosPolicyBase
{
public:
    OwnershipStrength(int32_t s)
		: s_(s)
    { }

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt32(s_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		s_ = reader->ReadInt32();
	}

	DEFINE_POLICY_TRAITS(OwnershipStrength, 7, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    int32_t 	GetStrength() const { return s_; }
    int32_t& 	GetStrength() 		{ return s_; }

    void SetStrength(int32_t s) 	{ s_ = s; }

private:
    int32_t s_;
};

}} //  DDS::Policy

#endif // DDS_Policy_OwnershipStrength_h_Included
