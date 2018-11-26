/*
 * DomainParticipantFactory.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#include "DCPS/Domain/DomainParticipantFactory.h"
#include "DCPS/Discovery/DiscoveryController.h"
#include "DCPS/Factory/DCPSConfig.h"

using namespace RxData;

namespace DCPS { namespace Domain
{

// -----------------------------------------------------------------------
// The domain participant factory config
// -----------------------------------------------------------------------

/**
 * @brief DomainParticipantFactoryConfig::defaultDomainParticipantQos
 * @return
 */
DomainParticipantFactoryConfig::DomainParticipantFactoryConfig()
    : domainParticipantFactoryQos_()
    , defaultDomainParticipantQos_()
{ }

DomainParticipantFactoryConfig::DomainParticipantFactoryConfig(DDS::DomainParticipantFactoryQoS factoryQos,
                                                               DDS::DomainParticipantQoS defaultDomainParticipantQos)
    : domainParticipantFactoryQos_(factoryQos)
    , defaultDomainParticipantQos_(defaultDomainParticipantQos)
{ }

/**
 * @brief DomainParticipantFactoryConfig::~DomainParticipantFactoryConfig
 */
DomainParticipantFactoryConfig::~DomainParticipantFactoryConfig()
{ }

/**
 * @brief DomainParticipantFactoryConfig::DefaultDomainParticipantQos
 * @return
 */
DDS::DomainParticipantQoS DomainParticipantFactoryConfig::DefaultDomainParticipantQos() const
{
    return defaultDomainParticipantQos_;
}

/**
 * @brief DomainParticipantFactoryConfig::setDefaultDomainParticipantQos
 * @param defaultDomainParticipantQos
 */
void DomainParticipantFactoryConfig::SetDefaultDomainParticipantQos(const DDS::DomainParticipantQoS &defaultDomainParticipantQos)
{
    defaultDomainParticipantQos_ = defaultDomainParticipantQos;
}

/**
 * @brief DomainParticipantFactoryConfig::DomainParticipantFactoryQos
 * @return
 */
DDS::DomainParticipantFactoryQoS DomainParticipantFactoryConfig::DomainParticipantFactoryQos() const
{
    return domainParticipantFactoryQos_;
}

/**
 * @brief DomainParticipantFactoryConfig::SetDomainParticipantFactoryQos
 * @param domainParticipantFactoryQos
 */
void DomainParticipantFactoryConfig::SetDomainParticipantFactoryQos(const DDS::DomainParticipantFactoryQoS &domainParticipantFactoryQos)
{
    domainParticipantFactoryQos_ = domainParticipantFactoryQos;
}

// -----------------------------------------------------------------------
// The domain participant factory state
// -----------------------------------------------------------------------

/**
 * @brief DomainParticipantFactoryState::DomainParticipantFactoryState
 * @param description
 */
DomainParticipantFactoryState::DomainParticipantFactoryState()
    : cache_()
{ }

DomainParticipantFactoryState::DomainParticipantFactoryState(CacheDescription description)
    : cache_(CacheFactory::Instance().getOrCreateCache(description))
{ }

DomainParticipantFactoryState::~DomainParticipantFactoryState()
{ }

/**
 * @brief DomainParticipantFactoryState::participants
 * @return
 */
DCPSConfig::DomainParticipantAccess DomainParticipantFactoryState::participants()
{
    return DCPSConfig::DomainParticipantAccess(cache_, DCPSConfig::DomainParticipantCacheTypeName());
}

// -----------------------------------------------------------------------
// The domain participant factory is a singleton
// -----------------------------------------------------------------------

/**
 * @brief DomainParticipantFactory::factory_
 */
Singleton<DomainParticipantFactory> DomainParticipantFactory::factory_;

/**
 * @brief DomainParticipantFactory::DomainParticipantFactory
 */
DomainParticipantFactory::DomainParticipantFactory()
    : Templates::ContextObject<DomainParticipantFactoryConfig, DomainParticipantFactoryState, BaseLib::Templates::NullCriticalState>
      (
          DomainParticipantFactoryConfig(DDS::DomainParticipantFactoryQoS::Default(), DDS::DomainParticipantQoS::Default()),
          DomainParticipantFactoryState(DDS::Factory::DDSConfig::DDSCacheDescription(DDS::Elements::DomainId::NIL()))
      )
{ }

DomainParticipantFactory::DomainParticipantFactory(
        DDS::DomainParticipantQoS defaultParticipantQos,
        DDS::DomainParticipantFactoryQoS defaultFactoryQos)
    : Templates::ContextObject<DomainParticipantFactoryConfig, DomainParticipantFactoryState, BaseLib::Templates::NullCriticalState>
      (
          DomainParticipantFactoryConfig(defaultFactoryQos, defaultParticipantQos),
          DomainParticipantFactoryState(DDS::Factory::DDSConfig::DDSCacheDescription(DDS::Elements::DomainId::NIL()))
      )
{ }

DomainParticipantFactory::~DomainParticipantFactory()
{ }

/**
 * @brief DomainParticipantFactory::Instance
 * @return
 */
DomainParticipantFactory &DomainParticipantFactory::Instance()
{
    return factory_.GetRef();
}

/**
 * @brief DomainParticipantFactory::CreateParticipant
 */
DomainParticipant::Ptr DomainParticipantFactory::CreateParticipant()
{
    return createParticipant(
                DDS::Elements::DomainId::NIL(),
                this->config().DefaultDomainParticipantQos(),
                DomainParticipantListener::Ptr(),
                Status::StatusMask::All());
}

/**
 * @brief DomainParticipantFactory::CreateParticipant
 */
DomainParticipant::Ptr DomainParticipantFactory::CreateParticipant(const DDS::Elements::DomainId &domainId)
{
    return createParticipant(
                domainId,
                this->config().DefaultDomainParticipantQos(),
                DomainParticipantListener::Ptr(),
                Status::StatusMask::All());
}

/**
 * @brief DomainParticipantFactory::CreateParticipant
 */
DomainParticipant::Ptr DomainParticipantFactory::CreateParticipant(
        const DDS::Elements::DomainId &domainId,
        const DDS::DomainParticipantQoS &qos,
        const DomainParticipantListener::Ptr &listener,
        const Status::StatusMask &mask)
{
    return createParticipant(
                domainId,
                qos,
                listener,
                mask);
}

// -----------------------------------------------------------------------
// Create participant
// -----------------------------------------------------------------------
/*DomainParticipant::Ptr DomainParticipantFactory::CreateParticipant(
                                    const DomainId &domainId,
                                    const std::string &qosLibraryName,
                                    const std::string qosProfileName,
                                    const DomainParticipantListener::Ptr &listener,
                                    const Status::StatusMask &statuses)
{

    return DomainParticipant::Ptr();
}*/

/**
 * @brief DomainParticipantFactory::createParticipant is the private implementation
 * that constructs a new domain participant with given domainId or returns an already
 * created.
 */
DomainParticipant::Ptr DomainParticipantFactory::createParticipant(
        const DDS::Elements::DomainId &domainId,
        const DDS::DomainParticipantQoS &qos,
        const DomainParticipantListener::Ptr &listener,
        const Status::StatusMask &mask)
{
    // -----------------------------------------------
    // If Domainid already exist then return that
    // -----------------------------------------------
    if(this->state().participants().containsKey(domainId))
    {
        IWARNING() << "Returning already created participant";

        return this->state().participants().findObject(domainId);
    }

    // -----------------------------------------------
    // Create a new participant with domainId
    // -----------------------------------------------
    DomainParticipant::Ptr participant(new DomainParticipant(domainId, qos, listener, mask));

    // -----------------------------------------------
    // Discovery listens to domain participant caches
    // TODO: To avoid dependencies, use an observer pattern instead, ex: signal->OnCreatedParticipant();
    // -----------------------------------------------
    Discovery::DiscoveryController::Instance().ObserveDomainCaches(participant->GetDomainId());

    this->state().participants().Write(participant->GetPtr(), domainId);

    return participant->GetPtr();
}

// -----------------------------------------------------------------------
// Lookup participant
// -----------------------------------------------------------------------
DomainParticipant::Ptr DomainParticipantFactory::LookupParticipant(const DDS::Elements::DomainId &domainId)
{
    // -----------------------------------------------
    // If Domainid already exist then return that
    // -----------------------------------------------
    if(this->state().participants().containsKey(domainId))
    {
        return this->state().participants().findObject(domainId);
    }

    IWARNING() << "Could not find participant in domain " << domainId;

    return DomainParticipant::Ptr();
}

}}
