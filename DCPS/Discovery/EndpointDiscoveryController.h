#ifndef DCPS_Discovery_EndpointDiscoveryFactory_h_IsIncluded
#define DCPS_Discovery_EndpointDiscoveryFactory_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Factory/DCPSConfig.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Discovery
{

/**
 * @brief The EndpointDiscoveryControllerState class
 */
class EndpointDiscoveryControllerState
{
public:
    EndpointDiscoveryControllerState();
    virtual ~EndpointDiscoveryControllerState();

    /**
     * @brief publishers
     * @return
     */
    DDS::Factory::DDSConfig::PublicationBuiltinTopicDataAccess publications(const DDS::Elements::DomainId &domainId);

    /**
     * @brief subscriptions
     * @return
     */
    DDS::Factory::DDSConfig::SubscriptionBuiltinTopicDataAccess subscriptions(const DDS::Elements::DomainId &domainId);

    /**
     * @brief topics
     * @return
     */
    DDS::Factory::DDSConfig::TopicDataAccess topics(const DDS::Elements::DomainId &domainId);

private:
    RxData::Cache::Ptr cache(const DDS::Elements::DomainId &domainId);
};

/**
 * @brief The ParticipantDiscoveryObserver class
 *
 * Is an observer of the entity caches manipulated by the DCPS entities through create and delete
 *
 * TODO: is the name appropriate? Perhaps LocalEndpointController
 *
 * For all domains
 */
class DLL_STATE EndpointDiscoveryController
        : public RxData::ObjectObserver<Subscription::SubscriberPtr>
        , public RxData::ObjectObserver<Subscription::AnyDataReaderHolderBasePtr>
        , public RxData::ObjectObserver<Publication::PublisherPtr>
        , public RxData::ObjectObserver<Publication::AnyDataWriterHolderBasePtr>
        , public RxData::ObjectObserver<Topic::TopicHolderPtr>
        , public Templates::ContextData<EndpointDiscoveryControllerState>
{
public:
    EndpointDiscoveryController();
    virtual ~EndpointDiscoveryController();

    CLASS_TRAITS(EndpointDiscoveryController)

    // ---------------------------------------------------------------
    // Callbacks when subscribers are created
    // ---------------------------------------------------------------
    virtual bool OnObjectCreated(Subscription::SubscriberPtr data);
    virtual bool OnObjectDeleted(Subscription::SubscriberPtr data);
    virtual bool OnObjectModified(Subscription::SubscriberPtr data);

    // ---------------------------------------------------------------
    // Callbacks when readers are created
    // ---------------------------------------------------------------
    virtual bool OnObjectCreated(Subscription::AnyDataReaderHolderBasePtr data);
    virtual bool OnObjectDeleted(Subscription::AnyDataReaderHolderBasePtr data);
    virtual bool OnObjectModified(Subscription::AnyDataReaderHolderBasePtr data);

    // ---------------------------------------------------------------
    // Callbacks when publishers are created
    // ---------------------------------------------------------------
    virtual bool OnObjectCreated(Publication::PublisherPtr data);
    virtual bool OnObjectDeleted(Publication::PublisherPtr data);
    virtual bool OnObjectModified(Publication::PublisherPtr data);

    // ---------------------------------------------------------------
    // Callbacks when writers are created
    // ---------------------------------------------------------------
    virtual bool OnObjectCreated(Publication::AnyDataWriterHolderBasePtr data);
    virtual bool OnObjectDeleted(Publication::AnyDataWriterHolderBasePtr data);
    virtual bool OnObjectModified(Publication::AnyDataWriterHolderBasePtr data);

    // ---------------------------------------------------------------
    // Callbacks when topics are created
    // ---------------------------------------------------------------
    virtual bool OnObjectCreated(Topic::TopicHolderPtr data);
    virtual bool OnObjectDeleted(Topic::TopicHolderPtr data);
    virtual bool OnObjectModified(Topic::TopicHolderPtr data);

};

}}

#endif
