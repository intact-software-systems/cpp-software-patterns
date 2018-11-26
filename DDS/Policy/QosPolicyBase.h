/*
 * QosPolicy.h
 *
 *  Created on: 10. july 2012
 *      Author: KVik
 */

#ifndef DDS_QosPolicy_QosPolicyBase_h_IsIncluded
#define DDS_QosPolicy_QosPolicyBase_h_IsIncluded

#include"DDS/CommonDefines.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * This class is the abstract root for all the QoS policies.
 *
 * It provides the basic mechanism for an application to specify quality of service parameters. It has an attribute name that
 * is used to identify uniquely each QoS policy. All concrete QosPolicy classes derive from this root and include a value
 * whose type depends on the concrete QoS policy.
 *
 * The type of a QosPolicy value may be atomic, such as an integer or float, or compound (a structure). Compound types are
 * used whenever multiple parameters must be set coherently to define a consistent value for a QosPolicy.
 *
 * Each Entity can be configured with a list of QosPolicy. However, any Entity cannot support any QosPolicy. For instance,
 * a DomainParticipant supports different QosPolicy than a Topic or a Publisher.
 *
 * QosPolicy can be set when the Entity is created, or modified with the set_qos method. Each QosPolicy in the list is
 * treated independently from the others. This approach has the advantage of being very extensible. However, there may be
 * cases where several policies are in conflict. Consistency checking is performed each time the policies are modified via the
 * set_qos operation.
 *
 * When a policy is changed after being set to a given value, it is not required that the new value be applied instantaneously;
 * the Service is allowed to apply it after a transition phase. In addition, some QosPolicy have immutable semantics
 * meaning that they can only be specified either at Entity creation time or else prior to calling the enable operation on the
 * Entity.
 *
 * Section 7.1.3, Supported QoS, on page 96 provides the list of all QosPolicy, their meaning, characteristics and possible
 * values, as well as the concrete Entity to which they apply.
 */
#define DEFINE_POLICY_TRAITS(NAME, ID, RXO, CHANGEABLE) \
        virtual const std::string& GetPolicyName() const { \
            static std::string the_name = #NAME; \
            return the_name; \
        } \
        virtual uint32_t GetPolicyId() const { \
            static uint32_t id = ID; \
            return id; \
        } \
        virtual DDS::Policy::RequestedOfferedKind::Type GetRxO() const { \
            static DDS::Policy::RequestedOfferedKind::Type rxo = RXO; \
            return rxo; \
        } \
        virtual bool IsChangeable() const { \
            static bool changeable = CHANGEABLE; \
            return changeable; \
        }

class DLL_STATE QosPolicyBase : public NetworkLib::NetObjectBase
{
public:
    QosPolicyBase()
    { }
    virtual ~QosPolicyBase()
    { }

    virtual const std::string& 	GetPolicyName() const = 0;
    virtual uint32_t 			GetPolicyId() const = 0;
    virtual bool                IsChangeable() const = 0;

    virtual DDS::Policy::RequestedOfferedKind::Type GetRxO() const = 0;
};

}}

#endif

