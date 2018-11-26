/*
 * Subscriber.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Subscription_Subscriber_h_IsIncluded
#define DCPS_Subscription_Subscriber_h_IsIncluded

#include"DCPS/CommonDefines.h"

#include"DCPS/Factory/DCPSConfig.h"
#include"DCPS/Sub/DataReaderListener.h"
#include"DCPS/Sub/SubscriberListener.h"
#include"DCPS/Sub/AnyDataReaderHolder.h"

#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Topic/Topic.h"
#include"DCPS/Status/IncludeLibs.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief The SubscriberConfig class
 */
class DLL_STATE SubscriberConfig
{
public:
    SubscriberConfig()
        : subscriberQos_()
        , defaultDataReaderQoS_()
    {}

    SubscriberConfig(DDS::SubscriberQoS subscriberQos, DDS::DataReaderQoS datareaderQos)
        : subscriberQos_(subscriberQos)
        , defaultDataReaderQoS_()
    {}
    virtual ~SubscriberConfig()
    {}

public:
    /**
     * @brief
     *
     * This operation retrieves the default value of the DataReader QoS, that is, the QoS policies which will be used for newly
     * created DataReader entities in the case where the QoS policies are defaulted in the create_datareader operation.
     *
     * The values retrieved get_default_datareader_qos will match the set of values specified on the last successful call to
     * get_default_datareader_qos, or else, if the call was never made, the default values listed in the QoS table in
     * Section 7.1.3, “Supported QoS,” on page 96.
     */
    DDS::SubscriberQoS GetQoS() const
    {
        return subscriberQos_;
    }

    /**
     * @brief
     *
     * TODO: Verify SubscriberQos is valid by adding an isValid() function for all QoS implementations
     *
     * This operation sets a default value of the DataReader QoS policies which will be used for newly created DataReader
     * entities in the case where the QoS policies are defaulted in the create_datareader operation.
     *
     * This operation will check that the resulting policies are self consistent; if they are not, the operation will have no effect
     * and return INCONSISTENT_POLICY.
     *
     * The special value DATAREADER_QOS_DEFAULT may be passed to this operation to indicate that the default QoS
     * should be reset back to the initial values the factory would use, that is the values that would be used if the
     * set_default_datareader_qos operation had never been called
     *
     * Possible error codes returned in addition to the standard ones: INCONSISTENT_POLICY, IMMUTABLE_POLICY.
     */
    ReturnCode::Type SetQoS(const DDS::SubscriberQoS &subscriberQos)
    {
        subscriberQos_ = subscriberQos;
        return ReturnCode::Ok;
    }

    DDS::DataReaderQoS GetDefaultDataReaderQoS() const
    {
        return defaultDataReaderQoS_;
    }

    ReturnCode::Type SetDefaultDataReaderQoS(const DDS::DataReaderQoS &qos)
    {
        defaultDataReaderQoS_ = qos;
        return ReturnCode::Ok;
    }

private:
    DDS::SubscriberQoS subscriberQos_;
    DDS::DataReaderQoS defaultDataReaderQoS_;
};

/**
 * @brief The SubscriberState class
 */
class DLL_STATE SubscriberState
{
public:
    SubscriberState();
    SubscriberState(Domain::DomainParticipantPtr participant, RxData::CacheDescription description);
    virtual ~SubscriberState();

    const Status::SampleLostStatus& GetSampleLostStatus() const;
    Status::SampleLostStatus& GetSampleLostStatus();

    Domain::DomainParticipantPtr GetParticipant() const;

    DCPSConfig::DataReaderAccess readers();

private:
    Domain::DomainParticipantWeakPtr    participant_;
    Status::SampleLostStatus            sampleLostStatus_;
    RxData::Cache::Ptr   cache_;
};

/**
 * @brief
 *
 * A Subscriber is the object responsible for the actual reception of the data resulting from its subscriptions.
 *
 * A Subscriber acts on the behalf of one or several DataReader objects that are related to it. When it receives data (from
 * the other parts of the system), it builds the list of concerned DataReader objects, and then indicates to the application that
 * data is available, through its listener or by enabling related conditions. The application can access the list of concerned
 * DataReader objects through the operation get_datareaders and then access the data available through operations on the
 * DataReader.
 *
 * All operations except for the base-class operations set_qos, get_qos, set_listener, get_listener, enable,
 * get_statuscondition, and create_datareader may return the value NOT_ENABLED.
 */
class DLL_STATE Subscriber : public Infrastructure::Entity
                           , public Templates::ContextObject<SubscriberConfig, SubscriberState, Templates::NullCriticalState>
                           , public ENABLE_SHARED_FROM_THIS(Subscriber)
{
public:
    Subscriber(DDS::SubscriberQoS qos,
               Domain::DomainParticipantPtr participant,
               Subscription::SubscriberListener::Ptr listener,
               Status::StatusMask statuses);

    virtual ~Subscriber();

    CLASS_TRAITS(Subscriber)

    Subscriber::Ptr GetPtr()
    {
        return shared_from_this();
    }

    // --------------------------------------------------------------
    //  DataReader Management
    // --------------------------------------------------------------

    template <typename DATA>
    typename DataReader<DATA>::Ptr CreateDataReader(
                const typename DCPS::Topic::Topic<DATA>::Ptr &topic,
                const DDS::DataReaderQoS &qos,
                const typename DataReaderListener<DATA>::Ptr &dataReaderListener,
                const Status::StatusMask &statusMask);

    template <typename DATA>
    ReturnCode::Type DeleteDataReader(typename DataReader<DATA>::Ptr &dataReader);

    template <typename DATA>
    typename DataReader<DATA>::Ptr LookupDataReader(const DDS::Elements::TopicName &topicName);

    // --------------------------------------------------------------
    //  Access Management
    // --------------------------------------------------------------

    ReturnCode::Type BeginAccess();
    ReturnCode::Type EndAccess();

    template <typename DATA>
    typename std::vector<AnyDataReaderHolderBase::Ptr> GetDataReaders(const Status::DataState &dataState);

public:
    ReturnCode::Type                NotifyDataReaders();
    Domain::DomainParticipantPtr	GetParticipant() const;
    ReturnCode::Type                DeleteContainedEntities();

    ReturnCode::Type        SetListener(const SubscriberListener::Ptr &listener, const Status::StatusMask &statusMask);
    SubscriberListener::Ptr	GetListener();

public:
    virtual InstanceHandle 		GetInstanceHandle() const;
    virtual ReturnCode::Type 	Enable();

private:
    SubscriberListener::WeakPtr subscriberListener_;
};

/**
 * @brief Subscriber::CreateDataReader
 *
 * This operation creates a DataReader. The returned DataReader will be attached and belong to the Subscriber.
 * The DataReader returned by the create_datareader operation will in fact be a derived class, specific to the data-type
 * associated with the Topic. As described in Section 7.1.2.3.7, for each application-defined type "Foo" there is an implied
 * auto-generated class FooDataReader that extends DataReader and contains the operations to read data of type "Foo."
 * In case of failure, the operation will return a ‘nil’ value (as specified by the platform).
 *
 * Note that a common application pattern to construct the QoS for the DataReader is to:
 *  - Retrieve the QoS policies on the associated Topic by means of the get_qos operation on the Topic.
 *  - Retrieve the default DataReader qos by means of the get_default_datareader_qos operation on the Subscriber.
 *  - Combine those two QoS policies and selectively modify policies as desired.
 *  - Use the resulting QoS policies to construct the DataReader.
 *
 * The special value DATAREADER_QOS_DEFAULT can be used to indicate that the DataReader should be created with
 * the default DataReader QoS set in the factory. The use of this value is equivalent to the application obtaining the default
 * DataReader QoS by means of the operation get_default_datareader_qos (Section 7.1.2.4.1.16) and using the resulting
 * QoS to create the DataReader.
 *
 * Provided that the TopicDescription passed to this method is a Topic or a ContentFilteredTopic, the special value
 * DATAREADER_QOS_USE_TOPIC_QOS can be used to indicate that the DataReader should be created with a
 * combination of the default DataReader QoS and the Topic QoS. (In the case of a ContentFilteredTopic, the Topic in
 * question is the ContentFilteredTopic’s "related Topic.") The use of this value is equivalent to the application obtaining the
 * default DataReader QoS and the Topic QoS (by means of the operation Topic::get_qos) and then combining these two
 * QoS using the operation copy_from_topic_qos whereby any policy that is set on the Topic QoS "overrides" the
 * corresponding policy on the default QoS. The resulting QoS is then applied to the creation of the DataReader. It is an
 * error to use DATAREADER_QOS_USE_TOPIC_QOS when creating a DataReader with a MultiTopic; this method will
 * return a ‘nil’ value in that case.
 *
 * The TopicDescription passed to this operation must have been created from the same DomainParticipant that was used
 * to create this Subscriber. If the TopicDescription was created from a different DomainParticipant, the operation will fail
 * and return a nil result.
 *
 * @param topic
 * @param qos
 * @param dataReaderListener
 * @param statusMask
 * @return
 */
template <typename DATA>
typename DataReader<DATA>::Ptr Subscriber::CreateDataReader(
        const typename DCPS::Topic::Topic<DATA>::Ptr &topic,
        const DDS::DataReaderQoS &qos,
        const typename DataReaderListener<DATA>::Ptr &dataReaderListener,
        const Status::StatusMask &statusMask)
{
    typename DataReader<DATA>::Ptr dataReader(
                new DataReader<DATA>(
                    this->state().GetParticipant(),
                    this->GetPtr(),
                    topic,
                    qos,
                    dataReaderListener,
                    statusMask)
                );

    typename AnyDataReaderHolder<DATA>::Ptr anyDataReader( new AnyDataReaderHolder<DATA>(dataReader) );

    this->state().readers().Write(anyDataReader, topic->GetTopicName());

    return dataReader;
}

/**
  * @brief
  *
  * This operation deletes a DataReader that belongs to the Subscriber. If the DataReader does not belong to the Subscriber,
  * the operation returns the error PRECONDITION_NOT_MET.
  *
  * The deletion of a DataReader is not allowed if there are any existing ReadCondition or QueryCondition objects that are
  * attached to the DataReader. If the delete_datareader operation is called on a DataReader with any of these existing
  * objects attached to it, it will return PRECONDITION_NOT_MET.
  *
  * The deletion of a DataReader is not allowed if it has any outstanding loans as a result of a call to read, take, or one of
  * the variants thereof. If the delete_datareader operation is called on a DataReader with one or more outstanding loans, it
  * will return PRECONDITION_NOT_MET.
  *
  * The delete_datareader operation must be called on the same Subscriber object used to create the DataReader. If
  * delete_datareader is called on a different Subscriber, the operation will have no effect and it will return
  * PRECONDITION_NOT_MET.
  *
  * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
  */
template <typename DATA>
ReturnCode::Type Subscriber::DeleteDataReader(typename DataReader<DATA>::Ptr &dataReader)
{
    if(this->state().readers().containsKey(dataReader->GetTopic()->GetTopicName()))
    {
        this->state().readers().Dispose(dataReader->GetTopic()->GetTopicName());
        return ReturnCode::Ok;
    }

    return ReturnCode::ALREADY_DELETED;
}

/**
  * @brief
  *
  * This operation retrieves a previously-created DataReader belonging to the Subscriber that is attached to a Topic with a
  * matching topic_name. If no such DataReader exists, the operation will return ’nil.’
  *
  * If multiple DataReaders attached to the Subscriber satisfy this condition, then the operation will return one of them. It is
  * not specified which one.
  *
  * The use of this operation on the built-in Subscriber allows access to the built-in DataReader entities for the built-in
  * topics.
  */
template <typename DATA>
typename DataReader<DATA>::Ptr Subscriber::LookupDataReader(const DDS::Elements::TopicName &topicName)
{
    if(!this->state().readers().containsKey(topicName))
    {
        // -- debug --
        IWARNING() << "Could not find " << topicName;
        // -- debug --

        return DataReader<DATA>::Ptr();
    }

    typename AnyDataReaderHolder<DATA>::Ptr holder = this->state().readers().findObject(topicName);

    if(holder)
    {
        // -- debug --
        ASSERT(holder->get());
        // -- debug --

        return holder->get();
    }

    // -- debug --
    ICRITICAL() << "TopicName " << topicName << " could not be cast to data reader with type " << TYPE_NAME(DATA) ;
    // -- debug --

    return DataReader<DATA>::Ptr();
}

/**
  * @brief
  *
  * This operation allows the application to access the DataReader objects that contain samples with the specified
  * sample_states, view_states, and instance_states.
  *
  * If the PRESENTATION QosPolicy of the Subscriber to which the DataReader belongs has the access_scope set to
  * ‘GROUP.’ This operation should only be invoked inside a begin_access/end_access block. Otherwise it will return the
  * error PRECONDITION_NOT_MET.
  *
  * Depending on the setting of the PRESENTATION QoS policy (see Section 7.1.3.6, “PRESENTATION,” on page 110), the
  * returned collection of DataReader objects may be a ‘set’ containing each DataReader at most once in no specified order,
  * or a ‘list’ containing each DataReader one or more times in a specific order.
  *
  * 1. If PRESENTATION access_scope is INSTANCE or TOPIC, the returned collection is a ‘set.’
  * 2. If PRESENTATION access_scope is GROUP and ordered_access is set to TRUE, then the returned collection is a ‘list.’
  *
  * This difference is due to the fact that, in the second situation it is required to access samples belonging to different
  * DataReader objects in a particular order. In this case, the application should process each DataReader in the same order
  * it appears in the ‘list’ and read or take exactly one sample from each DataReader. The patterns that an application should
  * use to access data is fully described in Section 7.1.2.5.1, “Access to the data,” on page 62.
  */
template <typename DATA>
typename std::vector<AnyDataReaderHolderBase::Ptr> Subscriber::GetDataReaders(const Status::DataState &dataState)
{
    return std::vector<AnyDataReaderHolderBase::Ptr>();
}

}}

#endif
