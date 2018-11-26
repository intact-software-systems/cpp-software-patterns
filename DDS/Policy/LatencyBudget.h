/*
 * LatencyBudget.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_LatencyBudget_h_Included
#define DDS_Policy_LatencyBudget_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @abstract
 *
 * This policy provides a means for the application to indicate to the middleware the "urgency" of the data-communication.
 * By having a non-zero duration the Service can optimize its internal operation.
 *
 * This policy is considered a hint. There is no specified mechanism as to how the service should take advantage of this hint.
 *
 * The value offered is considered compatible with the value requested if and only if the inequality "offered duration <=
 * requested duration" evaluates to "TRUE".
 */
class DLL_STATE LatencyBudget : public QosPolicyBase
{
public:
    LatencyBudget(const BaseLib::Duration& d)
		: duration_(d)
    { }
    LatencyBudget()
		: duration_(BaseLib::Duration::Zero())
    { }

	DEFINE_POLICY_TRAITS(LatencyBudget, 5, DDS::Policy::RequestedOfferedKind::COMPATIBLE, true)

public:
	virtual void Write(NetworkLib::SerializeWriter *) const
	{
		// TODO: Write Duration
	}

	virtual void Read(NetworkLib::SerializeReader *)
	{
		// TODO: Read Duration
	}

public:
    void SetDuration(const BaseLib::Duration& d) 		{ duration_ = d; }
    const BaseLib::Duration 	GetDuration() 		const 	{ return duration_; }
    BaseLib::Duration& 		GetDuration()  				{ return duration_; }

private:
    BaseLib::Duration duration_;
};

}} //  DDS::Policy

#endif // DDS_Policy_LatencyBudget_h_Included
