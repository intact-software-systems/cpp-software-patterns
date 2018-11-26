#include "DDS/Discovery/PublicationBuiltinTopicData.h"

namespace DDS { namespace Discovery
{

PublicationBuiltinTopicData::PublicationBuiltinTopicData()
    : BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>()
{
}

PublicationBuiltinTopicData::~PublicationBuiltinTopicData()
{

}

Elements::TopicKey PublicationBuiltinTopicData::key() const
{
    return this->Clone();
}

Elements::TopicKey PublicationBuiltinTopicData::participantKey() const
{
    return participantKey_;
}

void PublicationBuiltinTopicData::setParticipantKey(const Elements::TopicKey &participantKey)
{
    participantKey_ = participantKey;
}
Elements::TopicName PublicationBuiltinTopicData::topicName() const
{
    return topicName_;
}

void PublicationBuiltinTopicData::setTopicName(const Elements::TopicName &topicName)
{
    topicName_ = topicName;
}

Elements::TypeName PublicationBuiltinTopicData::typeName() const
{
    return typeName_;
}

void PublicationBuiltinTopicData::setTypeName(const Elements::TypeName &typeName)
{
    typeName_ = typeName;
}

PublisherQoS PublicationBuiltinTopicData::qos() const
{
    return qos_;
}

void PublicationBuiltinTopicData::setQos(const PublisherQoS &qos)
{
    qos_ = qos;
}

}}
