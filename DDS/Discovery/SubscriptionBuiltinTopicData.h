#ifndef DDS_Discovery_SubscriptionBuiltinTopicData_h_IsIncluded
#define DDS_Discovery_SubscriptionBuiltinTopicData_h_IsIncluded

#include"DDS/Elements/TopicKey.h"
#include"DDS/Elements/TopicName.h"
#include"DDS/Elements/TypeName.h"

#include"DDS/SubscriberQoS.h"
#include"DDS/DataReaderQoS.h"
#include"DDS/TopicQoS.h"

#include"DDS/Export.h"

namespace DDS { namespace Discovery
{

/*
- DCPSSubscription (entry created when a DataReader is created in association with its Subscriber).

key                     BuiltinTopicKey_t           DDS key to distinguish entries.
participant_key         BuiltinTopicKey_t           DDS key of the participant to which the DataReader belongs.
topic_name              string                      Name of the related Topic.
type_name               string                      Name of the type attached to the related Topic.
durability              DurabilityQosPolicy         Policy of the corresponding DataReader.
deadline                DeadlineQosPolicy           Policy of the corresponding DataReader.
latency_budget          LatencyBudgetQosPolicy      Policy of the corresponding DataReader.
liveliness              LivelinessQosPolicy         Policy of the corresponding DataReader.
reliability             ReliabilityQosPolicy        Policy of the corresponding DataReader.
ownership               OwnershipQosPolicy          Policy of the corresponding DataReader.
destination_order       DestinationOrderQosPolicy   Policy of the corresponding DataReader.
user_data               UserDataQosPolicy           Policy of the corresponding DataReader.
time_based_filter       TimeBasedFilterQosPolicy    Policy of the corresponding DataReader.
presentation            PresentationQosPolicy       Policy of the Subscriber to which the DataReader belongs.
partition               PartitionQosPolicy          Policy of the Subscriber to which the DataReader belongs.
topic_data              TopicDataQosPolicy          Policy of the related Topic.
group_data              GroupDataQosPolicy          Policy of the Subscriber to which the DataReader belongs.
*/

/**
 * @brief The SubscriptionBuiltinTopicData class
 *
 * TODO: The qos for the topic is not identical to datareaderqos + subscriberqos + topicqos
 */
class DLL_STATE SubscriptionBuiltinTopicData : public BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>
{
public:
    SubscriptionBuiltinTopicData();
    virtual ~SubscriptionBuiltinTopicData();

    Elements::TopicKey key() const;

    Elements::TopicKey participantKey() const;
    void setParticipantKey(const Elements::TopicKey &participantKey);

    Elements::TopicName topicName() const;
    void setTopicName(const Elements::TopicName &topicName);

    Elements::TypeName typeName() const;
    void setTypeName(const Elements::TypeName &typeName);

    SubscriberQoS qos() const;
    void setQos(const SubscriberQoS &qos);

private:
    Elements::TopicKey  participantKey_;
    Elements::TopicName topicName_;
    Elements::TypeName  typeName_;

    SubscriberQoS qos_;
    DataReaderQoS dataReaderQos_;
};

}}

#endif
