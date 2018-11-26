#ifndef DDS_Factory_DDSConfig_h_IsIncluded
#define DDS_Factory_DDSConfig_h_IsIncluded

#include"DDS/IncludeExtLibs.h"
#include"DDS/Discovery/IncludeLibs.h"
#include"DDS/Elements/IncludeLibs.h"
#include"DDS/Export.h"

namespace DDS { namespace Factory
{

class DLL_STATE DDSConfig
{
private:
    DDSConfig();
    virtual ~DDSConfig();

    friend class Singleton<DDSConfig>;

public:
    /**
     * @brief Instance gives access to the singleton (if necessary).
     * @return
     */
    static DDSConfig& Instance();


    /**
     * @brief DCPSCacheDescription
     * @param domainId
     * @return
     */
    static RxData::CacheDescription DDSCacheDescription(const DDS::Elements::DomainId &domainId);

    // -----------------------------------------------------------
    // Discovery topic initialization helper functions
    // -----------------------------------------------------------
    static std::string ParticipantBuiltinTopicDataTypeName();
    static std::string SubscriptionBuiltinTopicDataTypeName();
    static std::string PublicationBuiltinTopicDataTypeName();
    static std::string TopicDataTypeName();

    // -----------------------------------------------------------
    // Cache access typedefs for Discovery topics
    // -----------------------------------------------------------
    typedef RxData::ObjectAccessProxy<DDS::Discovery::ParticipantBuiltinTopicData, DDS::Elements::TopicKey>   ParticipantBuiltinTopicDataAccess;
    typedef RxData::ObjectAccessProxy<DDS::Discovery::PublicationBuiltinTopicData, DDS::Elements::TopicKey>   PublicationBuiltinTopicDataAccess;
    typedef RxData::ObjectAccessProxy<DDS::Discovery::SubscriptionBuiltinTopicData, DDS::Elements::TopicKey>  SubscriptionBuiltinTopicDataAccess;
    typedef RxData::ObjectAccessProxy<DDS::Discovery::TopicBuiltinTopicData, DDS::Elements::TopicKey>         TopicDataAccess;

private:
    /**
     * @brief instance_ is the singleton of this class
     */
    static Singleton<DDSConfig> instance_;
};

}}

#endif

