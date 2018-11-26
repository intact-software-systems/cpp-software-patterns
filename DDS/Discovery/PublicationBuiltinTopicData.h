#ifndef DDS_Discovery_PublicationBuiltinTopicData_h_IsIncluded
#define DDS_Discovery_PublicationBuiltinTopicData_h_IsIncluded

#include"DDS/Elements/TopicKey.h"
#include"DDS/Elements/TopicName.h"
#include"DDS/Elements/TypeName.h"

#include"DDS/PublisherQoS.h"
#include"DDS/DataWriterQoS.h"

#include"DDS/Export.h"

namespace DDS { namespace Discovery
{

/*
- DCPSPublication (entry created when a DataWriter is created in association with its Publisher).

key                     BuiltinTopicKey_t               DDS key to distinguish entries.
participant_key         BuiltinTopicKey_t               DDS key of the participant to which the DataWriter belongs.
topic_name              string                          Name of the related Topic.
type_name               string                          Name of the type attached to the related Topic.
durability              DurabilityQosPolicy             Policy of the corresponding DataWriter.
durability_service      DurabilityServiceQosPolicy      Policy of the corresponding DataWriter.
deadline                DeadlineQosPolicy               Policy of the corresponding DataWriter.
latency_budget          LatencyBudgetQosPolicy          Policy of the corresponding DataWriter.
liveliness              LivelinessQosPolicy             Policy of the corresponding DataWriter.
reliability             ReliabilityQosPolicy            Policy of the corresponding DataWriter.
lifespan                LifespanQosPolicy               Policy of the corresponding DataWriter.
user_data               UserDataQosPolicy               Policy of the corresponding DataWriter.
ownership               OwnershipQosPolicy              Policy of the corresponding DataWriter.
ownership_ strength     OwnershipStrengthQosPolicy      Policy of the corresponding DataWriter.
destination_order       DestinationOrderQosPolicy       Policy of the corresponding DataWriter.
presentation            PresentationQosPolicy           Policy of the Publisher to which the DataWriter belongs.
partition               PartitionQosPolicy              Policy of the Publisher to which the DataWriter belongs.
topic_data              TopicDataQosPolicy              Policy of the related Topic.
group_data              GroupDataQosPolicy              Policy of the Publisher to which the DataWriter belongs.
*/

/**
 * @brief The PublicationBuiltinTopicData class
 *
 * TODO: The qos for the topic is not identical to datawriterqos + publisherqos + topicqos
 */
class DLL_STATE PublicationBuiltinTopicData : public BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>
{
public:
    PublicationBuiltinTopicData();
    virtual ~PublicationBuiltinTopicData();

    Elements::TopicKey key() const;

    Elements::TopicKey participantKey() const;
    void setParticipantKey(const Elements::TopicKey &participantKey);

    Elements::TopicName topicName() const;
    void setTopicName(const Elements::TopicName &topicName);

    Elements::TypeName typeName() const;
    void setTypeName(const Elements::TypeName &typeName);

    PublisherQoS qos() const;
    void setQos(const PublisherQoS &qos);

private:
    Elements::TopicKey  participantKey_;
    Elements::TopicName topicName_;
    Elements::TypeName  typeName_;

    DataWriterQoS dataWriterQos_;
    PublisherQoS  qos_;
};

}}

#endif
