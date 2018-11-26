/*
 * Publisher.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Publication_Publisher_h_IsIncluded
#define DCPS_Publication_Publisher_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Factory/DCPSConfig.h"

#include"DCPS/Pub/DataWriter.h"
#include"DCPS/Pub/PublisherListener.h"
#include"DCPS/Pub/DataWriterListener.h"
#include"DCPS/Pub/AnyDataWriterHolder.h"

#include"DCPS/Topic/Topic.h"
#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Publication
{

/**
 * @brief The PublisherConfig class
 */
class DLL_STATE PublisherConfig
{
public:
    PublisherConfig(DDS::PublisherQoS qos = DDS::PublisherQoS::Default());
    ~PublisherConfig();

public:
    /**
     * @brief
     *
     * This operation sets a default value of the DataWriter QoS policies which will be used for newly created DataWriter
     * entities in the case where the QoS policies are defaulted in the create_datawriter operation.
     * This operation will check that the resulting policies are self consistent; if they are not, the operation will have no effect
     * and return INCONSISTENT_POLICY.
     * The special value DATAWRITER_QOS_DEFAULT may be passed to this operation to indicate that the default QoS
     * should be reset back to the initial values the factory would use, that is the values that would be used if the
     * set_default_datawriter_qos operation had never been called.
     */
    ReturnCode::Type SetDefaultDataWriterQoS(const DDS::DataWriterQoS &dataWriterQos);

    /**
     * @brief Publisher::SetQoS
     *
     * TODO: Verify PublisherQos is valid by adding an isValid() function for all QoS implementations
     *
     * Possible error codes returned in addition to the standard ones: INCONSISTENT_POLICY, IMMUTABLE_POLICY.
     *
     * @param publisherQos
     * @return
     */
    ReturnCode::Type SetQoS(const DDS::PublisherQoS &publisherQoS);

    /**
     * @brief
     *
     * This operation retrieves the default value of the DataWriter QoS, that is, the QoS policies which will be used for newly
     * created DataWriter entities in the case where the QoS policies are defaulted in the create_datawriter operation.
     * The values retrieved by get_default_datawriter_qos will match the set of values specified on the last successful call to
     * set_default_datawriter_qos, or else, if the call was never made, the default values listed in the QoS table in Section 7.1.3,
     * "Supported QoS," on page 96.
     */
    ReturnCode::Type GetDefaultDataWriterQoS(DDS::DataWriterQoS &dataWriterQoS) const;

    DDS::PublisherQoS GetQoS() const;

private:
    DDS::PublisherQoS  publisherQos_;
    DDS::DataWriterQoS defaultDataWriterQos_;
};

/**
 * @brief The PublisherState class
 */
class DLL_STATE PublisherState
{
public:
    PublisherState();

    PublisherState(Domain::DomainParticipantPtr participant, RxData::CacheDescription description);
    ~PublisherState();

    /**
     * @brief writers give read and write access to the data-writer cache
     * @return
     */
    DCPSConfig::DataWriterAccess writers();

    /**
     * @brief GetParticipant returns the parent of the publisher
     *
     * This operation returns the DomainParticipant to which the Publisher belongs.
     * @return
     */
    Domain::DomainParticipantPtr GetParticipant() const;

private:
    /**
     * @brief participant_ is the parent (creator) of the Publisher
     */
    Domain::DomainParticipantWeakPtr participant_;

    /**
     * @brief cache_
     */
    RxData::Cache::Ptr cache_;
};

/**
 * @brief The Publisher class is responsible for the actual dissemination of publications.
 */
class DLL_STATE Publisher : public DCPS::Infrastructure::Entity
                          , public Templates::ContextObject<PublisherConfig, PublisherState, Templates::NullCriticalState>
                          , public ENABLE_SHARED_FROM_THIS(Publisher)
{
public:
    Publisher(DDS::PublisherQoS qos,
              Domain::DomainParticipantPtr participant,
              Publication::PublisherListener::Ptr listener,
              Status::StatusMask statuses);

    virtual ~Publisher();

    CLASS_TRAITS(Publisher)

    /**
     * @brief GetPtr
     * @return
     */
    Publisher::Ptr GetPtr()
    {
        return shared_from_this();
    }

public:
    template <typename DATA>
    typename DataWriter<DATA>::Ptr CreateDataWriter(typename DCPS::Topic::Topic<DATA>::Ptr topic,
                                                    DDS::TopicQoS qos,
                                                    typename DataWriterListener<DATA>::Ptr dataWriterListener,
                                                    Status::StatusMask statusMask);

    template <typename DATA>
    ReturnCode::Type DeleteDataWriter(typename DataWriter<DATA>::Ptr &dataWriter);

    template <typename DATA>
    typename DataWriter<DATA>::Ptr LookupDataWriter(const DDS::Elements::TopicName &topicName);

public:
    ReturnCode::Type SuspendPublications();
    ReturnCode::Type ResumePublications();

    ReturnCode::Type BeginCoherentChanges();
    ReturnCode::Type EndCoherentChanges();

    ReturnCode::Type WaitForAcknowledgments(Duration &maxWait);

    ReturnCode::Type DeleteContainedEntities();

public:
    ReturnCode::Type CopyFromTopicQos(DDS::DataWriterQoS &dataWriterQoS, const DDS::TopicQoS &topicQoS);

    Domain::DomainParticipantPtr GetParticipant() const;

public:
    ReturnCode::Type        SetListener(const PublisherListener::Ptr &listener, const Status::StatusMask &statusMask);
    PublisherListener::Ptr	GetListener();

public:
    virtual InstanceHandle      GetInstanceHandle() const;
    virtual ReturnCode::Type 	Enable();

private:
    /**
     * @brief publisherListener_
     */
    PublisherListener::WeakPtr  publisherListener_;
};

/**
 * @brief
 *
 * This operation creates a DataWriter. The returned DataWriter will be attached and belongs to the Publisher.
 *
 * The DataWriter returned by the create_datawriter operation will in fact be a derived class, specific to the data-type
 * associated with the Topic. As described in Section 7.1.2.3.7, "Derived Classes for Each Application Class," on page 42,
 * for each application-defined type "Foo" there is an implied, auto-generated class FooDataWriter that extends DataWriter
 * and contains the operations to write data of type "Foo."
 *
 * In case of failure, the operation will return a 'nil' value (as specified by the platform).
 *
 * Note that a common application pattern to construct the QoS for the DataWriter is to:
 * - Retrieve the QoS policies on the associated Topic by means of the get_qos operation on the Topic.
 * - Retrieve the default DataWriter qos by means of the get_default_datawriter_qos operation on the Publisher.
 * - Combine those two QoS policies and selectively modify policies as desired.
 * - Use the resulting QoS policies to construct the DataWriter.
 *
 * The special value DATAWRITER_QOS_DEFAULT can be used to indicate that the DataWriter should be created with
 * the default DataWriter QoS set in the factory. The use of this value is equivalent to the application obtaining the default
 *
 * DataWriter QoS by means of the operation get_default_datawriter_qos (7.1.2.4.1.16) and using the resulting QoS to
 * create the DataWriter.
 *
 * The special value DATAWRITER_QOS_USE_TOPIC_QOS can be used to indicate that the DataWriter should be created
 * with a combination of the default DataWriter QoS and the Topic QoS. The use of this value is equivalent to the
 * application obtaining the default DataWriter QoS and the Topic QoS (by means of the operation Topic::get_qos) and then
 * combining these two QoS using the operation copy_from_topic_qos whereby any policy that is set on the Topic QoS
 * "overrides" the corresponding policy on the default QoS. The resulting QoS is then applied to the creation of the
 * DataWriter.
 *
 * The Topic passed to this operation must have been created from the same DomainParticipant that was used to create this
 * Publisher. If the Topic was created from a different DomainParticipant, the operation will fail and return a nil result.
 */
template <typename DATA>
typename DataWriter<DATA>::Ptr Publisher::CreateDataWriter(
        typename DCPS::Topic::Topic<DATA>::Ptr topic,
        DDS::TopicQoS qos,
        typename DataWriterListener<DATA>::Ptr dataWriterListener,
        Status::StatusMask statusMask)
{
    typename DataWriter<DATA>::Ptr dataWriter(
                new DataWriter<DATA>(
                    this->state().GetParticipant(),
                    this->GetPtr(),
                    topic,
                    qos,
                    dataWriterListener,
                    statusMask)
                );

    typename AnyDataWriterHolder<DATA>::Ptr anyDataWriter( new AnyDataWriterHolder<DATA>(dataWriter) );

    this->state().writers().Write(anyDataWriter, topic->GetTopicName());

    return dataWriter;
}

/**
 * @brief
 *
 * This operation deletes a DataWriter that belongs to the Publisher.
 *
 * The delete_datawriter operation must be called on the same Publisher object used to create the DataWriter. If
 * delete_datawriter is called on a different Publisher, the operation will have no effect and it will return
 * PRECONDITION_NOT_MET.
 *
 * The deletion of the DataWriter will automatically unregister all instances. Depending on the settings of the
 * WRITER_DATA_LIFECYCLE QosPolicy, the deletion of the DataWriter may also dispose all instances. Refer to
 *
 * Section 7.1.3.21, "WRITER_DATA_LIFECYCLE," on page 117 for details.
 * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
 */
template <typename DATA>
ReturnCode::Type Publisher::DeleteDataWriter(typename DataWriter<DATA>::Ptr &dataWriter)
{
    if(this->state().writers().containsKey(dataWriter->GetTopic()->GetTopicName()))
    {
        this->state().writers().Dispose(dataWriter->GetTopic()->GetTopicName());
        return ReturnCode::Ok;
    }

    return ReturnCode::ALREADY_DELETED;
}

/**
 * @brief
 *
 * This operation retrieves a previously created DataWriter belonging to the Publisher that is attached to a Topic with a
 * matching topic_name. If no such DataWriter exists, the operation will return 'nil.'
 * If multiple DataWriters attached to the Publisher satisfy this condition, then the operation will return one of them. It is
 * not specified which one.
 */
template <typename DATA>
typename DataWriter<DATA>::Ptr Publisher::LookupDataWriter(const DDS::Elements::TopicName &topicName)
{
    if(!this->state().writers().containsKey(topicName))
    {
        // -- debug --
        IWARNING() << "Could not find " << topicName;
        // -- debug --

        return DataWriter<DATA>::Ptr();
    }

    typename AnyDataWriterHolder<DATA>::Ptr holder = this->state().writers().findObject(topicName);

    if(holder)
    {
        // -- debug --
        ASSERT(holder->get());
        // -- debug --

        return holder->get();
    }

    // -- debug --
    ICRITICAL() << "TopicName " << topicName << " could not be cast to data writer with type " << TYPE_NAME(DATA) ;
    // -- debug --

    return DataWriter<DATA>::Ptr();
}

}} // namespace DCPS::Publication

#endif // DCPS_Publication_Publisher_h_IsIncluded
