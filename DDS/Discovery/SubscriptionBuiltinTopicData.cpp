#include "DDS/Discovery/SubscriptionBuiltinTopicData.h"

namespace DDS { namespace Discovery
{

SubscriptionBuiltinTopicData::SubscriptionBuiltinTopicData()
    : BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>()
{
}

SubscriptionBuiltinTopicData::~SubscriptionBuiltinTopicData()
{

}

Elements::TopicKey SubscriptionBuiltinTopicData::key() const
{
    return this->Clone();
}


Elements::TopicKey SubscriptionBuiltinTopicData::participantKey() const
{
    return participantKey_;
}

void SubscriptionBuiltinTopicData::setParticipantKey(const Elements::TopicKey &participantKey)
{
    participantKey_ = participantKey;
}

Elements::TopicName SubscriptionBuiltinTopicData::topicName() const
{
    return topicName_;
}

void SubscriptionBuiltinTopicData::setTopicName(const Elements::TopicName &topicName)
{
    topicName_ = topicName;
}

Elements::TypeName SubscriptionBuiltinTopicData::typeName() const
{
    return typeName_;
}

void SubscriptionBuiltinTopicData::setTypeName(const Elements::TypeName &typeName)
{
    typeName_ = typeName;
}

SubscriberQoS SubscriptionBuiltinTopicData::qos() const
{
    return qos_;
}

void SubscriptionBuiltinTopicData::setQos(const SubscriberQoS &qos)
{
    qos_ = qos;
}

}}
