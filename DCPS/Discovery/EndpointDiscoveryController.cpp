#include "DCPS/Discovery/EndpointDiscoveryController.h"
#include "DCPS/Pub/IncludeLibs.h"
#include "DCPS/Sub/IncludeLibs.h"

namespace DCPS { namespace Discovery
{

// ---------------------------------------------------------------
// Endpoint discovery state
// ---------------------------------------------------------------

EndpointDiscoveryControllerState::EndpointDiscoveryControllerState()
{ }

EndpointDiscoveryControllerState::~EndpointDiscoveryControllerState()
{ }

DDS::Factory::DDSConfig::PublicationBuiltinTopicDataAccess EndpointDiscoveryControllerState::publications(const DDS::Elements::DomainId &domainId)
{
    return DDS::Factory::DDSConfig::PublicationBuiltinTopicDataAccess(this->cache(domainId), DDS::Factory::DDSConfig::PublicationBuiltinTopicDataTypeName());
}

DDS::Factory::DDSConfig::SubscriptionBuiltinTopicDataAccess EndpointDiscoveryControllerState::subscriptions(const DDS::Elements::DomainId &domainId)
{
    return DDS::Factory::DDSConfig::SubscriptionBuiltinTopicDataAccess(this->cache(domainId), DDS::Factory::DDSConfig::SubscriptionBuiltinTopicDataTypeName());
}

DDS::Factory::DDSConfig::TopicDataAccess EndpointDiscoveryControllerState::topics(const DDS::Elements::DomainId &domainId)
{
    return DDS::Factory::DDSConfig::TopicDataAccess(this->cache(domainId), DDS::Factory::DDSConfig::TopicDataTypeName());
}

RxData::Cache::Ptr EndpointDiscoveryControllerState::cache(const DDS::Elements::DomainId &domainId)
{
    return RxData::CacheFactory::Instance().getOrCreateCache(DDS::Factory::DDSConfig::DDSCacheDescription(domainId));
}

// ---------------------------------------------------------------
// Controller implementation
// ---------------------------------------------------------------

EndpointDiscoveryController::EndpointDiscoveryController()
    : Templates::ContextData<EndpointDiscoveryControllerState>()
{ }

EndpointDiscoveryController::~EndpointDiscoveryController()
{ }

// ---------------------------------------------------------------
// Callbacks when subscriber cache is modified
// ---------------------------------------------------------------

bool EndpointDiscoveryController::OnObjectCreated(Subscription::SubscriberPtr data)
{
    IDEBUG() << " Created";
    return true;
}

bool EndpointDiscoveryController::OnObjectDeleted(Subscription::SubscriberPtr data)
{

    return true;
}

bool EndpointDiscoveryController::OnObjectModified(Subscription::SubscriberPtr data)
{

    return true;
}

// ---------------------------------------------------------------
// Callbacks when reader cache is modified
// ---------------------------------------------------------------

/*
Contents of the RTPS HistoryCache for the SEDPbuiltinSubscriptionsWriter and SEDPbuiltinSubscriptionsReader

RTPS-DataType               DiscoveredReaderData
Cardinality                 The number of DataReaders contained by the DomainParticipant.
                            There is a one-to-one correspondence between each DataReaders
                            in the Participant and a data-object that describes the DataReaders
                            stored in the WriterHistoryCache for the SEDPbuiltinSubscriptionsWriter.
Data-Object insertion       Each time a DataReader is created in the DomainParticipant.
Data-Object modification    Each time the QoS of an existing DataReader is modified.
Data-Object deletion        Each time an existing DataReader belonging to the DomainParticipant is deleted.
*/
bool EndpointDiscoveryController::OnObjectCreated(Subscription::AnyDataReaderHolderBasePtr data)
{

    IDEBUG() << " Created";
    return true;
}

bool EndpointDiscoveryController::OnObjectDeleted(Subscription::AnyDataReaderHolderBasePtr data)
{

    return true;
}

bool EndpointDiscoveryController::OnObjectModified(Subscription::AnyDataReaderHolderBasePtr data)
{

    return true;
}

// ---------------------------------------------------------------
// Callbacks when publisher cache is modified
// ---------------------------------------------------------------

bool EndpointDiscoveryController::OnObjectCreated(Publication::PublisherPtr data)
{

    IDEBUG() << " Created";
    return true;
}

bool EndpointDiscoveryController::OnObjectDeleted(Publication::PublisherPtr data)
{

    return true;
}

bool EndpointDiscoveryController::OnObjectModified(Publication::PublisherPtr data)
{

    return true;
}

// ---------------------------------------------------------------
// Callbacks when writer cache is modified
// ---------------------------------------------------------------

/*
Contents of the RTPS HistoryCache for the SEDPbuiltinPublicationsWriter and SEDPbuiltinPublicationsReader

RTPS-DataType               DiscoveredWriterData
Cardinality                 The number of DataWriters contained by the DomainParticipant.
                            There is a one-to-one correspondence between each DataWriter
                            in the participant and a data-object that describes the DataWriter
                            stored in the WriterHistoryCache for the SEDPbuiltinPublicationsWriter.
Data-Object insertion       Each time a DataWriter is created in the DomainParticipant.
Data-Object modification    Each time the QoS of an existing DataWriter is modified.
Data-Object deletion        Each time an existing DataWriter belonging to the DomainParticipant is deleted.
*/
bool EndpointDiscoveryController::OnObjectCreated(Publication::AnyDataWriterHolderBasePtr data)
{

    IDEBUG() << " Created";
    // Write PublicationBuiltinTopicData to RTPS HistoryCache, i.e., write to cache with that type
    return true;
}

bool EndpointDiscoveryController::OnObjectDeleted(Publication::AnyDataWriterHolderBasePtr data)
{
    // Dispose writer in RTPS HistoryCache
    return true;
}

bool EndpointDiscoveryController::OnObjectModified(Publication::AnyDataWriterHolderBasePtr data)
{
    // Update PublicationBuiltinTopicData in RTPS HistoryCache
    return true;
}

// ---------------------------------------------------------------
// Callbacks when topic cache is modified
// ---------------------------------------------------------------
/*
Contents of the RTPS HistoryCache for the SEDPbuiltinTopicsWriter and SEDPbuiltinTopicsReader

RTPS-DataType               DiscoveredTopicData
Cardinality                 The number of Topics created by the DomainParticipant.
                            There is a one-to-one correspondence between each Topic created
                            by the DomainParticipant and a data-object that describes the
                            Topic stored in the WriterHistoryCache for the builtinTopicsWriter.
Data-Object insertion       Each time a Topic is created in the DomainParticipant.
Data-Object modification    Each time the QoS of an existing Topic is modified.
Data-Object deletion        Each time an existing Topic belonging to the DomainParticipant is deleted.
*/
bool EndpointDiscoveryController::OnObjectCreated(Topic::TopicHolderPtr data)
{

    IDEBUG() << " Created";
    // Write TopicBuiltinTopicData to the RTPS HistoryCache
    return true;
}

bool EndpointDiscoveryController::OnObjectDeleted(Topic::TopicHolderPtr data)
{
    // Dispose TopicBuiltinTopicData in the RTPS HistoryCache
    return true;
}

bool EndpointDiscoveryController::OnObjectModified(Topic::TopicHolderPtr data)
{
    // Update TopicBuiltinTopicData in the RTPS HistoryCache
    return true;
}

}}

