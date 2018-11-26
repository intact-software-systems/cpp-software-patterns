/*
 * QosPolicy.h
 *
 *  Created on: 10. july 2012
 *      Author: KVik
 */

#ifndef DCPS_Infrastructure_QosPolicy_h_IsIncluded
#define DCPS_Infrastructure_QosPolicy_h_IsIncluded

#include<string>

namespace DCPS { namespace Infrastructure
{

/**
@description

This class is the abstract root for all the QoS policies.

It provides the basic mechanism for an application to specify quality of service parameters. It has an attribute name that
is used to identify uniquely each QoS policy. All concrete QosPolicy classes derive from this root and include a value
whose type depends on the concrete QoS policy.

The type of a QosPolicy value may be atomic, such as an integer or float, or compound (a structure). Compound types are
used whenever multiple parameters must be set coherently to define a consistent value for a QosPolicy.

Each Entity can be configured with a list of QosPolicy. However, any Entity cannot support any QosPolicy. For instance,
a DomainParticipant supports different QosPolicy than a Topic or a Publisher.

QosPolicy can be set when the Entity is created, or modified with the set_qos method. Each QosPolicy in the list is
treated independently from the others. This approach has the advantage of being very extensible. However, there may be
cases where several policies are in conflict. Consistency checking is performed each time the policies are modified via the
set_qos operation.

When a policy is changed after being set to a given value, it is not required that the new value be applied instantaneously;
the Service is allowed to apply it after a transition phase. In addition, some QosPolicy have immutable semantics
meaning that they can only be specified either at Entity creation time or else prior to calling the enable operation on the
Entity.

Section 7.1.3, Supported QoS, on page 96 provides the list of all QosPolicy, their meaning, characteristics and possible
values, as well as the concrete Entity to which they apply.
*/
class QosPolicy
{
public:
    QosPolicy(const std::string &name);
	virtual ~QosPolicy();

private:
    std::string name_;
};

}} // namespace DCPS::Infrastructure

#endif // DCPS_Infrastructure_QosPolicy_h_IsIncluded

