#ifndef DCPS_Factory_DCPSFactory_h_IsIncluded
#define DCPS_Factory_DCPSFactory_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Factory/DCPSInit.h"
#include"DCPS/Factory/DCPSInitStrategies.h"

#include"DCPS/Sub/Subscriber.h"
#include"DCPS/Sub/DataReaderListener.h"
#include"DCPS/Domain/DomainParticipant.h"
#include"DCPS/Topic/Topic.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Factory
{
// ------------------------------------------------------------------------------
// Factory configurations and the DCPSFactory
// ------------------------------------------------------------------------------

/**
 * @brief The DCPSFactoryConfig class
 */
class DLL_STATE DCPSFactoryConfig
{
public:
    DCPSFactoryConfig() {}
    virtual ~DCPSFactoryConfig() {}

    static DCPSFactoryConfig Default()
    {
        return DCPSFactoryConfig();
    }

private:
    // TODO: Factory configurations
};


/**
 * @brief The DCPSFactory class
 *
 * TODO: Create all DCPS entities from this class. DomainParticipantFactory can call this class.
 */
class DLL_STATE DCPSFactory : public BaseLib::Templates::ContextData<DCPSFactoryConfig>
{
private:
    DCPSFactory() : BaseLib::Templates::ContextData<DCPSFactoryConfig>()
    {}
    virtual ~DCPSFactory()
    {}

    friend class Singleton<DCPSFactory>;

public:
    /**
     * @brief Instance
     * @return
     */
    static DCPSFactory& Instance();

    /**
     * @brief CreateBuiltinSubscriber
     * @param participant
     * @return
     */
    static Subscription::SubscriberPtr CreateBuiltinSubscriber(Domain::DomainParticipantPtr participant);

    // ------------------------------------------------
    // Create initializers for DCPS entities
    // ------------------------------------------------

    static DomainParticipantInit::Ptr   CreateDomainParticipantInit();
    static PublisherInit::Ptr           CreatePublisherInit();
    static SubscriberInit::Ptr          CreateSubscriberInit();

    template <typename T>
    static typename DataWriterInit<T>::Ptr       CreateDataWriterInit();

    template <typename T>
    static typename DataReaderInit<T>::Ptr       CreateDataReaderInit();

private:
    // ------------------------------------------------
    // private functions
    // ------------------------------------------------

    template <typename T>
    void InitializeBuiltinTopic(Domain::DomainParticipantPtr participant,
                                Subscription::SubscriberPtr subscriber,
                                const DDS::TopicQoS &topicQos);

private:
    static Singleton<DCPSFactory> factory_;
};

/**
 * @brief DCPSFactory::InitializeBuiltinTopic
 * @param participant
 * @param subscriber
 * @param topicQos
 */
template <typename T>
void DCPSFactory::InitializeBuiltinTopic(
        Domain::DomainParticipantPtr participant,
        Subscription::SubscriberPtr subscriber,
        const DDS::TopicQoS &topicQos)
{
    typename Topic::Topic<T>::Ptr topic = participant->CreateTopic<T>(topicQos);

    subscriber->CreateDataReader<T>(
                topic,
                DDS::DataReaderQoS(
                    topicQos,
                    DDS::Policy::UserData::Empty(),
                    DDS::Policy::TimeBasedFilter(Duration::FromMilliseconds(0)),
                    DDS::Policy::ReaderDataLifecycle::NoAutoPurgeDisposedSamples()),
                typename Subscription::DataReaderListener<T>::Ptr(),
                Status::StatusMask::All());
}

// ------------------------------------------------------------------------------
// Builders for each entity type
// ------------------------------------------------------------------------------

/**
 * @brief The DomainParticipantInitBuilder class
 */
class DomainParticipantInitBuilder
        : public CreateMethod0<DomainParticipantInit::Ptr>
        , public BaseLib::Templates::ContextData<DCPSFactoryConfig>

{
public:
    DomainParticipantInitBuilder(DCPSFactoryConfig config = DCPSFactoryConfig::Default())
        : BaseLib::Templates::ContextData<DCPSFactoryConfig>()
    {}
    virtual ~DomainParticipantInitBuilder() {}

    virtual DomainParticipantInit::Ptr Create();

private:
    // TODO: Configurations here
};


/**
 * @brief The SubscriberInitBuilder class
 */
class SubscriberInitBuilder
        : public CreateMethod0<SubscriberInit::Ptr>
        , public BaseLib::Templates::ContextData<DCPSFactoryConfig>
{
public:
    SubscriberInitBuilder(DCPSFactoryConfig config = DCPSFactoryConfig::Default())
        : BaseLib::Templates::ContextData<DCPSFactoryConfig>()
    {}
    virtual ~SubscriberInitBuilder() {}

    virtual SubscriberInit::Ptr Create();

private:
    // TODO: Configurations here
};

/**
 * @brief The DataReaderInitBuilder class
 */
template <typename T>
class DataReaderInitBuilder
        : public CreateMethod0<typename DataReaderInit<T>::Ptr>
        , public BaseLib::Templates::ContextData<DCPSFactoryConfig>
{
public:
    DataReaderInitBuilder(DCPSFactoryConfig config = DCPSFactoryConfig::Default())
        : BaseLib::Templates::ContextData<DCPSFactoryConfig>()
    {}
    virtual ~DataReaderInitBuilder() {}

    virtual typename DataReaderInit<T>::Ptr Create()
    {
        return RTPSDataReaderInit<T>::Ptr( new RTPSDataReaderInit<T>());
    }

private:
    // TODO: Configurations here
};

/**
 * @brief The PublisherInitBuilder class
 */
class PublisherInitBuilder
        : public CreateMethod0<PublisherInit::Ptr>
        , public BaseLib::Templates::ContextData<DCPSFactoryConfig>
{
public:
    PublisherInitBuilder(DCPSFactoryConfig config = DCPSFactoryConfig::Default())
        : BaseLib::Templates::ContextData<DCPSFactoryConfig>()
    {}
    virtual ~PublisherInitBuilder() {}

    virtual PublisherInit::Ptr Create();

private:
    // TODO: Configurations here
};

/**
 * @brief The DataWriterInitBuilder class
 */
template <typename T>
class DataWriterInitBuilder
        : public CreateMethod0<typename DataWriterInit<T>::Ptr>
        , public BaseLib::Templates::ContextData<DCPSFactoryConfig>
{
public:
    DataWriterInitBuilder(DCPSFactoryConfig config = DCPSFactoryConfig::Default())
        : BaseLib::Templates::ContextData<DCPSFactoryConfig>()
    {}
    virtual ~DataWriterInitBuilder() {}

    virtual typename DataWriterInit<T>::Ptr Create()
    {
        return RTPSDataWriterInit<T>::Ptr( new RTPSDataWriterInit<T>());
    }

private:
    // TODO: Configurations here
};



/**
 * @brief DCPSFactory::CreateDataWriterInit
 * @return
 */
template <typename T>
typename DataWriterInit<T>::Ptr DCPSFactory::CreateDataWriterInit()
{
    DataWriterInitBuilder<T> builder(DCPSFactory::Instance().context());
    return builder.Create();
}

/**
 * @brief DCPSFactory::CreateDataReaderInit
 * @return
 */
template <typename T>
typename DataReaderInit<T>::Ptr DCPSFactory::CreateDataReaderInit()
{
    DataReaderInitBuilder<T> builder(DCPSFactory::Instance().context());
    return builder.Create();
}


}}

#endif
