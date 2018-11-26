#include "DDS/Discovery/ParticipantBuiltinTopicData.h"

namespace DDS { namespace Discovery
{

ParticipantBuiltinTopicData::ParticipantBuiltinTopicData()
    : BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>()
    , qos_()
{ }

ParticipantBuiltinTopicData::ParticipantBuiltinTopicData(Elements::TopicKey key, const DomainParticipantQoS &qos)
    : BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>(key)
    , qos_(qos)
{ }

ParticipantBuiltinTopicData::~ParticipantBuiltinTopicData()
{ }

Elements::TopicKey ParticipantBuiltinTopicData::Key() const
{
    return this->Clone();
}

DomainParticipantQoS ParticipantBuiltinTopicData::Qos() const
{
    return qos_;
}

void ParticipantBuiltinTopicData::SetQos(const DomainParticipantQoS &qos)
{
    qos_ = qos;
}

}}

