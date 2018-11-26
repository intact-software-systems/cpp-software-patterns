#ifndef DCPS_Factory_DCPSConfig_h_IsIncluded
#define DCPS_Factory_DCPSConfig_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS
{

// ------------------------------------------------------------------------------
// DCPS Config provide static configurations of: bultin topics, cache names, etc.
// ------------------------------------------------------------------------------

/**
 * @brief The DCPSConfig class
 */
class DLL_STATE DCPSConfig
{
private:
    DCPSConfig();
    virtual ~DCPSConfig();

    friend class Singleton<DCPSConfig>;

public:
    /**
     * @brief Instance gives access to the singleton (if necessary).
     * @return
     */
    static DCPSConfig& Instance();

    // -----------------------------------------------------------
    // Cache initialization helper functions
    // -----------------------------------------------------------
    static std::string DomainParticipantCacheTypeName();
    static std::string PublisherCacheTypeName();
    static std::string SubscriberCacheTypeName();
    static std::string DataReaderCacheTypeName();
    static std::string DataWriterCacheTypeName();
    static std::string TopicCacheTypeName();

    // -----------------------------------------------------------
    // Cache access typedefs for DCPS entities
    // -----------------------------------------------------------
    typedef RxData::ObjectAccessProxy<DCPS::Domain::DomainParticipantPtr, DDS::Elements::DomainId>                   DomainParticipantAccess;
    typedef RxData::ObjectAccessProxy<DCPS::Publication::PublisherPtr, InstanceHandle>                               PublisherAccess;
    typedef RxData::ObjectAccessProxy<DCPS::Subscription::SubscriberPtr, InstanceHandle>                             SubscriberAccess;
    typedef RxData::ObjectAccessProxy<DCPS::Topic::TopicHolderPtr, InstanceHandle>                                   TopicAccess;
    typedef RxData::ObjectAccessProxy<DCPS::Publication::AnyDataWriterHolderBasePtr, DDS::Elements::TopicName>       DataWriterAccess;
    typedef RxData::ObjectAccessProxy<DCPS::Subscription::AnyDataReaderHolderBasePtr, DDS::Elements::TopicName>      DataReaderAccess;

private:
    /**
     * @brief instance_ is the singleton of this class
     */
    static Singleton<DCPSConfig> instance_;
};

}

#endif
