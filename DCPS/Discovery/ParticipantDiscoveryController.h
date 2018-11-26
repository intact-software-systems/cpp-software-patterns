#ifndef DCPS_Discovery_ParticipantDiscoveryController_h_IsIncluded
#define DCPS_Discovery_ParticipantDiscoveryController_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Factory/DCPSConfig.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Discovery
{

/**
 * @brief The ParticipantDiscoveryControllerState class
 */
class ParticipantDiscoveryControllerState
{
public:
    ParticipantDiscoveryControllerState();
    virtual ~ParticipantDiscoveryControllerState();

    DDS::Factory::DDSConfig::ParticipantBuiltinTopicDataAccess instances(const DDS::Elements::DomainId &domainId);
};

/**
 * @brief The ParticipantDiscoveryObserver class
 *
 * For all domains - domain participants are in different domains per definition
 */
class DLL_STATE ParticipantDiscoveryController
        : public RxData::ObjectObserver<Domain::DomainParticipantPtr>
        , public Templates::ContextData<ParticipantDiscoveryControllerState>
{
public:
    ParticipantDiscoveryController();
    virtual ~ParticipantDiscoveryController();

    CLASS_TRAITS(ParticipantDiscoveryController)

    /**
     * @brief OnObjectCreated
     * @param data
     */
    virtual bool OnObjectCreated(Domain::DomainParticipantPtr data);

    /**
     * @brief OnObjectDeleted
     * @param data
     */
    virtual bool OnObjectDeleted(Domain::DomainParticipantPtr data);

    /**
     * @brief OnObjectModified
     * @param data
     */
    virtual bool OnObjectModified(Domain::DomainParticipantPtr data);
};

}}

#endif
