#include "DDS/Factory/DDSConfig.h"

namespace DDS { namespace Factory {

// ------------------------------------------------------------------------------
// DCPS Config provide static configurations
// ------------------------------------------------------------------------------

/**
 * @brief DCPSFactory::factory_
 */
Singleton<DDSConfig> DDSConfig::instance_;

/**
 * @brief DDSConfig::DDSConfig
 */
DDSConfig::DDSConfig()
{ }

DDSConfig::~DDSConfig()
{ }

/**
 * @brief DDSConfig::Instance
 * @return
 */
DDSConfig& DDSConfig::Instance()
{
    return DDSConfig::instance_.GetRef();
}

/**
 * @brief DDSConfig::DDSCacheDescription
 * @param domainId
 * @return
 */
RxData::CacheDescription DDSConfig::DDSCacheDescription(const DDS::Elements::DomainId &domainId)
{
    std::stringstream name;
    name << "DDSCache.domain." << domainId;

    IDEBUG() << "Cache name: " << name.str();

    return RxData::CacheDescription(name.str());
}

// ------------------------------------------------------------------------------
// DCPS Config discovery topic type names
// ------------------------------------------------------------------------------

std::string DDSConfig::ParticipantBuiltinTopicDataTypeName()
{
    return BaseLib::Utility::GetTypeName<DDS::Discovery::ParticipantBuiltinTopicData>();
}

std::string DDSConfig::SubscriptionBuiltinTopicDataTypeName()
{
    return BaseLib::Utility::GetTypeName<DDS::Discovery::SubscriptionBuiltinTopicData>();
}

std::string DDSConfig::PublicationBuiltinTopicDataTypeName()
{
    return BaseLib::Utility::GetTypeName<DDS::Discovery::PublicationBuiltinTopicData>();
}

std::string DDSConfig::TopicDataTypeName()
{
    return BaseLib::Utility::GetTypeName<DDS::Discovery::TopicBuiltinTopicData>();
}

}}
