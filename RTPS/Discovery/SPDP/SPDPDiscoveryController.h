#ifndef RTPS_Discovery_SPDP_DiscoveryController_h_IsIncluded
#define RTPS_Discovery_SPDP_DiscoveryController_h_IsIncluded

#include "RTPS/IncludeExtLibs.h"
#include "RTPS/Discovery/DiscoveryController.h"
#include "RTPS/Factory/RtpsFactory.h"
#include "RTPS/Export.h"

namespace RTPS { namespace SPDP {

/**
 * @brief The SPDPDiscoveryControllerState class
 */
class DLL_STATE SPDPDiscoveryControllerState
{
public:
    SPDPDiscoveryControllerState();
    virtual ~SPDPDiscoveryControllerState();

    RTPS::RtpsParticipant* participant() const;
    void setParticipant(RTPS::RtpsParticipant *participant);

private:
    RTPS::RtpsParticipant *participant_;
};

/**
 * @brief The SPDPDiscoveryControllerConfig class
 */
class DLL_STATE SPDPDiscoveryControllerConfig
{
public:
    SPDPDiscoveryControllerConfig();
    virtual ~SPDPDiscoveryControllerConfig();

};

/**
 * @brief The DiscoveryController class controls the simple participant discovery protocol (SPDP).
 *
 * TODO:
 * - Control the creation of discovered participants and endpoints
 * - Create the SPDP and SEDP reader/writer
 * - Integration between NetworkLib SocketReactor and reader/writer through a cache interface?
 */
class DLL_STATE SPDPDiscoveryController
        : public RTPS::DiscoveryController
        , public BaseLib::Templates::KeyValueObjectObserver<GUID, DDS::Discovery::ParticipantBuiltinTopicData>
        , public Templates::ContextObject<SPDPDiscoveryControllerConfig, SPDPDiscoveryControllerState>
{
public:
    SPDPDiscoveryController();
    virtual ~SPDPDiscoveryController();

public:
    // -----------------------------------------------------------
    // Interface KeyValueObserver - from DCPS::ParticipantDiscoveryController
    // -----------------------------------------------------------
    virtual void OnObjectCreated(GUID key, DDS::Discovery::ParticipantBuiltinTopicData value);

    virtual void OnObjectDeleted(GUID key);

    virtual void OnObjectModified(GUID key, DDS::Discovery::ParticipantBuiltinTopicData value);


public:
    // -----------------------------------------------------------
    // Interface DiscoveryController
    // -----------------------------------------------------------
    virtual bool AddParticipant(const GUID &guid);

    virtual bool RemoveParticipant(const GUID &guid);

private:
    void initialize(const GUID &guid);

};

}}

#endif
