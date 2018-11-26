#include "DDS/Discovery/TopicBuiltinTopicData.h"

namespace DDS { namespace Discovery
{

TopicBuiltinTopicData::TopicBuiltinTopicData()
    : BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>()
{ }

TopicBuiltinTopicData::~TopicBuiltinTopicData()
{ }

Elements::TopicKey TopicBuiltinTopicData::key() const
{
    return this->Clone();
}

Elements::TopicName TopicBuiltinTopicData::topicName() const
{
    return topicName_;
}

void TopicBuiltinTopicData::setTopicName(const Elements::TopicName &topicName)
{
    topicName_ = topicName;
}

Elements::TypeName TopicBuiltinTopicData::typeName() const
{
    return typeName_;
}

void TopicBuiltinTopicData::setTypeName(const Elements::TypeName &typeName)
{
    typeName_ = typeName;
}

TopicQoS TopicBuiltinTopicData::qos() const
{
    return qos_;
}

void TopicBuiltinTopicData::setQos(const TopicQoS &qos)
{
    qos_ = qos;
}

}}
