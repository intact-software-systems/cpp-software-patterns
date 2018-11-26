#include "DCPS/Discovery/ParticipantDiscoveryController.h"
#include "DCPS/Domain/DomainParticipant.h"

namespace DCPS { namespace Discovery
{

// ------------------------------------------------------------------------------
// The state of the controller
// ------------------------------------------------------------------------------

/**
 * @brief ParticipantDiscoveryControllerState::ParticipantDiscoveryControllerState
 * @param description
 */
ParticipantDiscoveryControllerState::ParticipantDiscoveryControllerState()
{ }

ParticipantDiscoveryControllerState::~ParticipantDiscoveryControllerState()
{ }

DDS::Factory::DDSConfig::ParticipantBuiltinTopicDataAccess ParticipantDiscoveryControllerState::instances(const DDS::Elements::DomainId &domainId)
{
    RxData::Cache::Ptr cache =
            RxData::CacheFactory::Instance().getOrCreateCache(DDS::Factory::DDSConfig::DDSCacheDescription(domainId));

    ASSERT(cache);

    return DDS::Factory::DDSConfig::ParticipantBuiltinTopicDataAccess(cache, DDS::Factory::DDSConfig::ParticipantBuiltinTopicDataTypeName());
}

// ------------------------------------------------------------------------------
// The controller implementation
// ------------------------------------------------------------------------------

/**
 * @brief ParticipantDiscoveryController::ParticipantDiscoveryController
 */
ParticipantDiscoveryController::ParticipantDiscoveryController()
    : Templates::ContextData<ParticipantDiscoveryControllerState>()
{ }

/**
 * @brief ParticipantDiscoveryController::~ParticipantDiscoveryController
 */
ParticipantDiscoveryController::~ParticipantDiscoveryController()
{ }

/**
 * @brief ParticipantDiscoveryController::OnObjectCreated
 * @param data
 * @return
 */
bool ParticipantDiscoveryController::OnObjectCreated(Domain::DomainParticipantPtr data)
{
    DDS::Discovery::ParticipantBuiltinTopicData topicData(DDS::Elements::TopicKey(data->GetInstanceHandle()), data->config().GetQoS());

    return this->context().instances(data->GetDomainId()).Write(topicData, topicData.Key());
}

/**
 * @brief ParticipantDiscoveryController::OnObjectDeleted
 * @param data
 * @return
 */
bool ParticipantDiscoveryController::OnObjectDeleted(Domain::DomainParticipantPtr data)
{
    return this->context().instances(data->GetDomainId()).Dispose(DDS::Elements::TopicKey(data->GetInstanceHandle()));
}

/**
 * @brief ParticipantDiscoveryController::OnObjectModified
 * @param data
 * @return
 */
bool ParticipantDiscoveryController::OnObjectModified(Domain::DomainParticipantPtr data)
{
    DDS::Discovery::ParticipantBuiltinTopicData topicData(DDS::Elements::TopicKey(data->GetInstanceHandle()), data->config().GetQoS());

    return this->context().instances(data->GetDomainId()).Write(topicData, topicData.Key());
}

}}
