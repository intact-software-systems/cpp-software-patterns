/*
 * Presentation.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Presentation_h_Included
#define DDS_Policy_Presentation_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @abstract
 *
 * This QoS policy controls the extent to which changes to data-instances can be made dependent on each other and also the
 * kind of dependencies that can be propagated and maintained by the Service.
 * The setting of coherent_access controls whether the Service will preserve the groupings of changes made by the
 * publishing application by means of the operations begin_coherent_change and end_coherent_change.
 * The setting of ordered_access controls whether the Service will preserve the order of changes.
 * The granularity is controlled by the setting of the access_scope.
 */
class DLL_STATE Presentation : public QosPolicyBase
{
public:
    Presentation()
		: accessScope_(Policy::PresentationAccessScopeKind::INSTANCE)
		, coherentAccess_(false)
		, orderedAccess_(false)
    { }

    Presentation(Policy::PresentationAccessScopeKind::Type the_access_scope,
                 bool the_coherent_access,
                 bool the_ordered_access)
		: accessScope_(the_access_scope)
		, coherentAccess_(the_coherent_access)
		, orderedAccess_(the_ordered_access)
    { }

	DEFINE_POLICY_TRAITS(Presentation, 3, DDS::Policy::RequestedOfferedKind::COMPATIBLE, false)

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt32((int32_t) accessScope_);
		writer->WriteInt8(coherentAccess_);
		writer->WriteInt8(orderedAccess_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		accessScope_ = (Policy::PresentationAccessScopeKind::Type) reader->ReadInt32();
		coherentAccess_ = reader->ReadInt8();
		orderedAccess_ = reader->ReadInt8();
	}

public:
    void SetAccessScope(Policy::PresentationAccessScopeKind::Type as) 	{ accessScope_ = as; }
    void SetCoherentAccess(bool on) 									{ coherentAccess_ = on; }
    void SetOrderedAccess(bool on) 										{ orderedAccess_ = on; }

    Policy::PresentationAccessScopeKind::Type GetAccessScope() 			const 	{ return accessScope_; }
    bool GetCoherentAccess() 											const 	{ return coherentAccess_; }
    bool GetOrderedAccess() 											const 	{ return orderedAccess_; }

public:
	static Presentation GroupAccessScope(bool coherent = false, bool ordered = false)
	{
		return Presentation(Policy::PresentationAccessScopeKind::GROUP, coherent, ordered);
	}

	static Presentation InstanceAccessScope(bool coherent = false, bool ordered = false)
	{
		return Presentation(Policy::PresentationAccessScopeKind::INSTANCE, coherent, ordered);
	}

	static Presentation TopicAccessScope(bool coherent = false, bool ordered = false)
	{
		return Presentation(Policy::PresentationAccessScopeKind::TOPIC, coherent, ordered);
	}

private:
    Policy::PresentationAccessScopeKind::Type accessScope_;

    bool coherentAccess_;
    bool orderedAccess_;
};

}} //  DDS::Policy

#endif // DDS_Policy_Presentation_h_Included

