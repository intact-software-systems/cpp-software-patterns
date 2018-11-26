#include "DCPS/Discovery/DiscoveryController.h"
#include "DCPS/Factory/DCPSConfig.h"

#include "DCPS/Pub/Publisher.h"
#include "DCPS/Pub/AnyDataWriterHolder.h"
#include "DCPS/Sub/Subscriber.h"
#include "DCPS/Sub/AnyDataReaderHolder.h"
#include "DCPS/Topic/TopicHolder.h"
#include "DCPS/Domain/DomainParticipant.h"

namespace DCPS { namespace Discovery
{

// ------------------------------------------------------------------------------
// Discovery controller state
// ------------------------------------------------------------------------------

DiscoveryControllerState::DiscoveryControllerState()
    : participantDiscovery_(new ParticipantDiscoveryController())
    , endpointDiscovery_(new EndpointDiscoveryController())
{ }

DiscoveryControllerState::~DiscoveryControllerState()
{
    delete participantDiscovery_;
    delete endpointDiscovery_;

    participantDiscovery_ = NULL;
    endpointDiscovery_ = NULL;
}

ParticipantDiscoveryController *DiscoveryControllerState::ParticipantDiscovery() const
{
    return participantDiscovery_;
}

EndpointDiscoveryController *DiscoveryControllerState::EndpointDiscovery() const
{
    return endpointDiscovery_;
}


// ------------------------------------------------------------------------------
// DiscoveryController implementation
// ------------------------------------------------------------------------------

Singleton<DiscoveryController> DiscoveryController::discoveryController_;

DiscoveryController::DiscoveryController()
    : Templates::ContextData<DiscoveryControllerState>()
{ }

DiscoveryController::~DiscoveryController()
{
}

DiscoveryController& DiscoveryController::Instance()
{
    return DiscoveryController::discoveryController_.GetRef();
}

void DiscoveryController::run()
{

}

void DiscoveryController::ObserveDomainCaches(DDS::Elements::DomainId domainId)
{
    RxData::Cache::Ptr cache = RxData::CacheFactory::Instance().getOrCreateCache(DDS::Factory::DDSConfig::DDSCacheDescription(domainId));

    ASSERT(cache);

    cache->getOrCreateHome<DCPS::Domain::DomainParticipantPtr, DDS::Elements::DomainId>( DCPSConfig::DomainParticipantCacheTypeName())->Connect(this);
}

// ------------------------------------------------------------------------
// Callbacks
// ------------------------------------------------------------------------

bool DiscoveryController::OnObjectCreated(Domain::DomainParticipantPtr data)
{
    //TODO: Connect here leads to deadlock when signaller is synchronous!
    this->addDomainCacheObservers(data->GetDomainId());
    return true;
}

bool DiscoveryController::OnObjectDeleted(Domain::DomainParticipantPtr data)
{
    //TODO: RemoveObserver here leads to deadlock when signaller is synchronous!
    this->removeDomainCacheObservers(data->GetDomainId());
    return true;
}

bool DiscoveryController::OnObjectModified(Domain::DomainParticipantPtr data)
{
    IDEBUG() << " domain participant modified?";
    return true;
}

// ------------------------------------------------------------------------
// Add remove observers depending on call back event
// ------------------------------------------------------------------------

void DiscoveryController::addDomainCacheObservers(const DDS::Elements::DomainId &domainId)
{
    RxData::Cache::Ptr cache = RxData::CacheFactory::Instance().findCacheByDescription(DDS::Factory::DDSConfig::DDSCacheDescription(domainId));

    ASSERT(cache);

    cache->getOrCreateHome<DCPS::Domain::DomainParticipantPtr, DDS::Elements::DomainId>( DCPSConfig::DomainParticipantCacheTypeName())->Connect(this->context().ParticipantDiscovery());
    cache->getOrCreateHome<DCPS::Publication::AnyDataWriterHolderBasePtr, DDS::Elements::TopicName>(DCPSConfig::DataWriterCacheTypeName())->Connect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Subscription::AnyDataReaderHolderBasePtr, DDS::Elements::TopicName>(DCPSConfig::DataReaderCacheTypeName())->Connect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Subscription::SubscriberPtr, InstanceHandle>(DCPSConfig::SubscriberCacheTypeName())->Connect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Publication::PublisherPtr, InstanceHandle>(DCPSConfig::PublisherCacheTypeName())->Connect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Topic::TopicHolderPtr, InstanceHandle>(DCPSConfig::TopicCacheTypeName())->Connect(this->context().EndpointDiscovery());
}

void DiscoveryController::removeDomainCacheObservers(const DDS::Elements::DomainId &domainId)
{
    RxData::Cache::Ptr cache = RxData::CacheFactory::Instance().findCacheByDescription(DDS::Factory::DDSConfig::DDSCacheDescription(domainId));

    ASSERT(cache);

    cache->getOrCreateHome<DCPS::Domain::DomainParticipantPtr, DDS::Elements::DomainId>( DCPSConfig::DomainParticipantCacheTypeName())->Disconnect(this->context().ParticipantDiscovery());
    cache->getOrCreateHome<DCPS::Publication::AnyDataWriterHolderBasePtr, DDS::Elements::TopicName>(DCPSConfig::DataWriterCacheTypeName())->Disconnect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Subscription::AnyDataReaderHolderBasePtr, DDS::Elements::TopicName>(DCPSConfig::DataReaderCacheTypeName())->Disconnect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Subscription::SubscriberPtr, InstanceHandle>(DCPSConfig::SubscriberCacheTypeName())->Disconnect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Publication::PublisherPtr, InstanceHandle>(DCPSConfig::PublisherCacheTypeName())->Disconnect(this->context().EndpointDiscovery());
    cache->getOrCreateHome<DCPS::Topic::TopicHolderPtr, InstanceHandle>(DCPSConfig::TopicCacheTypeName())->Disconnect(this->context().EndpointDiscovery());
}

}}

