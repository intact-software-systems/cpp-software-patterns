#ifndef RTPS_Discovery_SEDP_DiscoveryController_h_IsIncluded
#define RTPS_Discovery_SEDP_DiscoveryController_h_IsIncluded

#include "RTPS/IncludeExtLibs.h"
#include "RTPS/Discovery/DiscoveryController.h"
#include "RTPS/Factory/RtpsFactory.h"
#include "RTPS/Export.h"

namespace RTPS { namespace SEDP
{

/**
 * @brief The SEDPDiscoveryControllerState class
 */
class DLL_STATE SEDPDiscoveryControllerState
{
public:
    SEDPDiscoveryControllerState();
    virtual ~SEDPDiscoveryControllerState();
};

/**
 * @brief The SEDPDiscoveryControllerConfig class
 */
class DLL_STATE SEDPDiscoveryControllerConfig
{
public:
    SEDPDiscoveryControllerConfig();
    virtual ~SEDPDiscoveryControllerConfig();

};

/**
 * @brief The DiscoveryController class controls the simple participant discovery protocol (SEDP).
 *
 * TODO:
 * - Control the creation of discovered participants and endpoints
 * - Create the SEDP and SEDP reader/writer
 * - Integration between NetworkLib SocketReactor and reader/writer through a cache interface?
 */
class DLL_STATE SEDPDiscoveryController
        : public BaseLib::Templates::ObjectObserver<DDS::Discovery::PublicationBuiltinTopicData>
        , public BaseLib::Templates::ObjectObserver<DDS::Discovery::SubscriptionBuiltinTopicData>
        , public BaseLib::Templates::ObjectObserver<DDS::Discovery::TopicBuiltinTopicData>
        , public Templates::ContextObject<SEDPDiscoveryControllerConfig, SEDPDiscoveryControllerState, Templates::NullCriticalState>
{
public:
    SEDPDiscoveryController();
    virtual ~SEDPDiscoveryController();

public:
    // -----------------------------------------------------------
    // Interface KeyValueObserver<GUID, DDS::Discovery::PublicationBuiltinTopicData>
    // from DCPS::EndpointDiscoveryController
    // -----------------------------------------------------------
    virtual void OnObjectCreated(DDS::Discovery::PublicationBuiltinTopicData value);
    virtual void OnObjectDeleted(DDS::Discovery::PublicationBuiltinTopicData value);
    virtual void OnObjectModified(DDS::Discovery::PublicationBuiltinTopicData value);

public:
    // -----------------------------------------------------------
    // Interface KeyValueObserver<GUID, DDS::Discovery::SubscriptionBuiltinTopicData>
    // from DCPS::EndpointDiscoveryController
    // -----------------------------------------------------------
    virtual void OnObjectCreated(DDS::Discovery::SubscriptionBuiltinTopicData value);
    virtual void OnObjectDeleted(DDS::Discovery::SubscriptionBuiltinTopicData value);
    virtual void OnObjectModified(DDS::Discovery::SubscriptionBuiltinTopicData value);

public:
    // -----------------------------------------------------------
    // Interface KeyValueObserver<GUID, DDS::Discovery::TopicBuiltinTopicData>
    // from DCPS::EndpointDiscoveryController
    // -----------------------------------------------------------
    virtual void OnObjectCreated(DDS::Discovery::TopicBuiltinTopicData value);
    virtual void OnObjectDeleted(DDS::Discovery::TopicBuiltinTopicData value);
    virtual void OnObjectModified(DDS::Discovery::TopicBuiltinTopicData value);
};

}}

#endif
