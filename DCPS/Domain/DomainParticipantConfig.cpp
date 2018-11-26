#include"DCPS/Domain/DomainParticipantConfig.h"

namespace DCPS { namespace Domain
{

DomainParticipantConfig::DomainParticipantConfig()
{ }

DomainParticipantConfig::DomainParticipantConfig(const DDS::Elements::DomainId &domainId,
                                                 const DDS::DomainParticipantQoS &qos)
    : domainId_(domainId)
    , domainQos_(qos)
{ }

DomainParticipantConfig::~DomainParticipantConfig()
{ }

/**
 * @brief DomainParticipantConfig::SetQoS
 * TODO: Verify DomainParticipantQoS is valid by adding an isValid() function for all QoS implementations
 * @param domainQos
 * @return
 */
ReturnCode::Type DomainParticipantConfig::SetQoS(const DDS::DomainParticipantQoS &domainQos)
{
    domainQos_ = domainQos;
    return ReturnCode::Ok;
}

/**
 * @brief DomainParticipantConfig::GetQoS
 * @return
 */
DDS::DomainParticipantQoS DomainParticipantConfig::GetQoS() const
{
    return domainQos_;
}

DDS::PublisherQoS DomainParticipantConfig::GetDefaultPublisherQos() const
{
    return defaultPublisherQos_;
}

bool DomainParticipantConfig::SetDefaultPublisherQos(const DDS::PublisherQoS &qos)
{
    defaultPublisherQos_ = qos;
    return true;
}

DDS::SubscriberQoS DomainParticipantConfig::GetDefaultSubscriberQos() const
{
    return defaultSubscriberQos_;
}

bool DomainParticipantConfig::SetDefaultSubscriberQos(const DDS::SubscriberQoS &qos)
{
    defaultSubscriberQos_ = qos;
    return true;
}

DDS::TopicQoS DomainParticipantConfig::GetDefaultTopicQos() const
{
    return defaultTopicQos_;
}

bool DomainParticipantConfig::SetDefaultTopicQos(const DDS::TopicQoS &qos)
{
    defaultTopicQos_ = qos;
    return true;
}

DDS::Elements::DomainId DomainParticipantConfig::GetDomainId() const
{
    return domainId_;
}


}}
