#include"DCPS/Factory/DCPSConfig.h"

namespace DCPS
{

// ------------------------------------------------------------------------------
// DCPS Config provide static configurations
// ------------------------------------------------------------------------------

/**
 * @brief DCPSFactory::factory_
 */
Singleton<DCPSConfig> DCPSConfig::instance_;

/**
 * @brief DCPSConfig::DCPSConfig
 */
DCPSConfig::DCPSConfig()
{ }

/**
 * @brief DCPSConfig::~DCPSConfig
 */
DCPSConfig::~DCPSConfig()
{ }

/**
 * @brief DCPSFactory::Instance
 * @return
 */
DCPSConfig &DCPSConfig::Instance()
{
    return DCPSConfig::instance_.GetRef();
}

// ------------------------------------------------------------------------------
// DCPS Config type names
// ------------------------------------------------------------------------------

std::string DCPSConfig::DomainParticipantCacheTypeName()
{
    return BaseLib::Utility::GetTypeName<DCPS::Domain::DomainParticipantPtr>();
}

std::string DCPSConfig::PublisherCacheTypeName()
{
    return BaseLib::Utility::GetTypeName<DCPS::Publication::PublisherPtr>();
}

std::string DCPSConfig::SubscriberCacheTypeName()
{
    return BaseLib::Utility::GetTypeName<DCPS::Subscription::SubscriberPtr>();
}

std::string DCPSConfig::DataReaderCacheTypeName()
{
    return BaseLib::Utility::GetTypeName<DCPS::Subscription::AnyDataReaderHolderBasePtr>();
}

std::string DCPSConfig::DataWriterCacheTypeName()
{
    return BaseLib::Utility::GetTypeName<DCPS::Publication::AnyDataWriterHolderBasePtr>();
}

std::string DCPSConfig::TopicCacheTypeName()
{
    return BaseLib::Utility::GetTypeName<DCPS::Topic::TopicHolderPtr>();
}

}

