#ifndef DDS_Discovery_TopicBuiltinTopicData_h_IsIncluded
#define DDS_Discovery_TopicBuiltinTopicData_h_IsIncluded

#include"DDS/Elements/TopicKey.h"
#include"DDS/Elements/TopicName.h"
#include"DDS/Elements/TypeName.h"
#include"DDS/TopicQoS.h"

#include"DDS/Export.h"

namespace DDS { namespace Discovery
{

/*
- DCPSTopic (entry created when a Topic object is created).

key                 BuiltinTopicKey_t               DDS key to distinguish entries.
name                string                          Name of the Topic.
type_name           string                          Name of the type attached to the Topic.
durability          DurabilityQosPolicy             Policy of the corresponding Topic.
durability_service  DurabilityServiceQosPolicy      Policy of the corresponding Topic.
deadline            DeadlineQosPolicy               Policy of the corresponding Topic.
latency_budget      LatencyBudgetQosPolicy          Policy of the corresponding Topic.
liveliness          LivelinessQosPolicy             Policy of the corresponding Topic.
reliability         ReliabilityQosPolicy            Policy of the corresponding Topic.
transport_priority  TransportPriorityQosPolicy      Policy of the corresponding Topic.
lifespan            LifespanQosPolicy               Policy of the corresponding Topic.
destination_order   DestinationOrderQosPolicy       Policy of the corresponding Topic.
history             HistoryQosPolicy                Policy of the corresponding Topic.
resource_limits     ResourceLimitsQosPolicy         Policy of the corresponding Topic.
ownership           OwnershipQosPolicy              Policy of the corresponding Topic.
topic_data          TopicDataQosPolicy              Policy of the corresponding Topic.
*/

/**
 * @brief The TopicBuiltinTopicData class
 *
 * TopicQos is identical to what is needed in the TopicBuiltinTopicData
 *
 * TODO: Rename TopicBuiltinTopicData because there is a policy with the same name.
 */
class DLL_STATE TopicBuiltinTopicData : public BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>
{
public:
    TopicBuiltinTopicData();
    virtual ~TopicBuiltinTopicData();

    Elements::TopicKey key() const;

    Elements::TopicName topicName() const;
    void setTopicName(const Elements::TopicName &topicName);

    Elements::TypeName typeName() const;
    void setTypeName(const Elements::TypeName &typeName);

    TopicQoS qos() const;
    void setQos(const TopicQoS &qos);

private:
    Elements::TopicName topicName_;
    Elements::TypeName  typeName_;

    TopicQoS qos_;
};

}}

#endif
