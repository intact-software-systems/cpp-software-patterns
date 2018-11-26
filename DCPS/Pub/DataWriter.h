/*
 * DataWriter.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Publication_DataWriter_h_IsIncluded
#define DCPS_Publication_DataWriter_h_IsIncluded

#include"DCPS/CommonDefines.h"

#include"DCPS/Pub/DataWriterListener.h"
#include"DCPS/Pub/DataWriterStatus.h"

#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Topic/Topic.h"
#include"DCPS/Machine/CacheWriter.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Publication
{

/**
 * @brief The DataWriterState class contains objects that represents the state of the DataWriter.
 */
template <typename DATA>
class DataWriterState
{
public:
    DataWriterState(Domain::DomainParticipantPtr participant,
                    PublisherPtr publisher,
                    typename Topic::Topic<DATA>::Ptr topic)
        : participant_(participant)
        , publisher_(publisher)
        , topic_(topic)
    {}
    virtual ~DataWriterState()
    {}

    PublisherPtr Parent() const
    {
        return publisher_.lock();
    }

    typename DCPS::Topic::Topic<DATA>::Ptr GetTopic() const
    {
        return topic_;
    }

    /**
     * @brief GetParticipant returns the parent of the publisher
     */
    Domain::DomainParticipantPtr GetParticipant() const
    {
        return participant_.lock();
    }

    DataWriterStatus& GetDataWriterStatus()
    {
        return dataWriterStatus_;
    }

    const DataWriterStatus& GetDataWriterStatus() const
    {
        return dataWriterStatus_;
    }

private:
    Domain::DomainParticipantWeakPtr participant_;

private:
    Publication::PublisherWeakPtr		publisher_;
    typename Topic::Topic<DATA>::Ptr 	topic_;

private:
    DataWriterStatus dataWriterStatus_;
};

/**
 * @brief The DataWriterConfig class contains objects that configures the behavior of the DataWriter.
 */
class DLL_STATE DataWriterConfig
{
public:
    DataWriterConfig(DCPSCacheWriter cacheWriter,
                     Status::DataState statusFilter,
                     DDS::DataWriterQoS dataWriterQos)
        : cacheWriter_(cacheWriter)
        , statusFilter_(statusFilter)
        , dataWriterQos_(dataWriterQos)
    {}

    ~DataWriterConfig()
    {}

    DCPSCacheWriter& GetCacheWriter()
    {
        return cacheWriter_;
    }

    void SetCacheWriter(const DCPSCacheWriter &writer)
    {
        cacheWriter_ = writer;
    }

    Status::DataState& GetStatusFilter()
    {
        return statusFilter_;
    }

    /**
     * @brief SetQoS installs the QoS for this DataWriter.
     *
     * TODO: Verify DataWriterQos is valid by adding an isValid() function for all QoS implementations
     *
     * Possible error codes returned in addition to the standard ones: INCONSISTENT_POLICY, IMMUTABLE_POLICY.
     *
     * @param dataWriterQoS
     * @return
     */
    ReturnCode::Type SetQoS(const DDS::DataWriterQoS &dataWriterQoS)
    {
       dataWriterQos_ = dataWriterQoS;
       return ReturnCode::Ok;
    }

    const DDS::DataWriterQoS& GetQoS() const
    {
        return dataWriterQos_;
    }

private:
    DCPSCacheWriter cacheWriter_;
    Status::DataState statusFilter_;
    DDS::DataWriterQoS dataWriterQos_;
};

/**
 * @brief A DataWriter is attached to exactly one Publisher that acts as a factory for it.
 * A DataWriter is bound to exactly one Topic and therefore to exactly one data type.
 * The Topic must exist prior to the DataWriter’s creation.
 *
 * DataWriter is an abstract class. It must be specialized for each particular application
 * data-type as shown in Figure 7.8. Template argument DATA is the data to share.
 *
 * TODO:
 * - Do not use RTPS::Timestamp
 * - RtpsData should be a template argument to DataWriter class!!
 */
template <typename DATA>
class DataWriter : public DCPS::Infrastructure::Entity
                 , public Templates::ContextObject<DataWriterConfig, DataWriterState<DATA>, Templates::NullCriticalState>
{
public:
    /**
     * @brief Create a DataWriter.
     *
     * @param pub the publisher
     * @param topic the Topic associated with this DataWriter
     * @param qos the DataWriter qos.
     * @param listener the DataWriter listener.
     * @param mask the listener event mask.
     */
    DataWriter(Domain::DomainParticipantPtr participant,
               PublisherPtr publisher,
               typename DCPS::Topic::Topic<DATA>::Ptr topic,
               DDS::DataWriterQoS qos,
               typename DataWriterListener<DATA>::Ptr listener,
               Status::StatusMask mask)
        : DCPS::Infrastructure::Entity(mask)
        , Templates::ContextObject<DataWriterConfig, DataWriterState<DATA>, Templates::NullCriticalState>
          (
              DataWriterConfig(DCPSCacheWriter(), Status::DataState::Any(), qos),
              DataWriterState<DATA>(participant, publisher, topic)
          )
        , dataWriterListener_(listener)
    {}

    virtual ~DataWriter()
    {}

    CLASS_TRAITS(DataWriter)

public:
    // --------------------------------------------------------------
    // Instance Management
    // --------------------------------------------------------------
    InstanceHandle RegisterInstance(const DATA &data);
    InstanceHandle RegisterInstance(const DATA &data, const RTPS::Timestamp &timestamp);

    ReturnCode::Type UnregisterInstance(const DATA &data, const InstanceHandle &handle);
    ReturnCode::Type UnregisterInstance(const DATA &data, const InstanceHandle &handle, const RTPS::Timestamp &timestamp);

    ReturnCode::Type Dispose(const DATA &data, const InstanceHandle &handle);
    ReturnCode::Type Dispose(const DATA &data, const InstanceHandle &handle, const RTPS::Timestamp &timestamp);

    ReturnCode::Type Write(const DATA &data, const InstanceHandle &handle);
    ReturnCode::Type Write(const DATA &data, const InstanceHandle &handle, const RTPS::Timestamp &timestamp);

private:
    InstanceHandle registerInstance(const DATA &data, const RTPS::Timestamp &timestamp = RTPS::Timestamp::INVALID());

public:

    /**
     * This operation can be used to retrieve the instance key that corresponds to an instance_handle. The operation will only
     * fill the fields that form the key inside the key_holder instance.
     *
     * This operation may return BAD_PARAMETER if the InstanceHandle_t a_handle does not correspond to an existing
     * data-object known to the DataWriter. If the implementation is not able to check invalid handles, then the result in this
     * situation is unspecified.
     */
    DATA& GetKeyValue(DATA &data, const InstanceHandle &handle)
    {
        return DATA();
    }

    /**
     * @brief This operation retrieves the list of subscriptions currently “associated” with the DataWriter; that is, subscriptions that
     * have a matching Topic and compatible QoS that the application has not indicated should be “ignored” by means of the
     * DomainParticipant ignore_subscription operation.
     *
     * The handles returned in the ‘subscription_handles’ list are the ones that are used by the DDS implementation to locally
     * identify the corresponding matched DataReader entities. These handles match the ones that appear in the
     * ‘instance_handle’ field of the SampleInfo when reading the “DCPSSubscriptions” builtin topic.
     *
     * The operation may fail if the infrastructure does not locally maintain the connectivity information.
     *
     * TODO: SubscriptionBuiltinTopicData
     */
     //ReturnCode GetMatchedSubscriptions(SubscriptionBuiltinTopicData &subData, InstanceHandle &handle);

    /**
     * @brief This operation retrieves information on a subscription that is currently “associated” with the DataWriter; that is, a
     * subscription with a matching Topic and compatible QoS that the application has not indicated should be “ignored” by
     * means of the DomainParticipant ignore_subscription operation.
     *
     * The subscription_handle must correspond to a subscription currently associated with the DataWriter, otherwise the
     * operation will fail and return BAD_PARAMETER. The operation get_matched_subscriptions can be used to find the
     * subscriptions that are currently matched with the DataWriter.
     *
     * The operation may also fail if the infrastructure does not hold the information necessary to fill in the subscription_data.
     * In this case the operation will return UNSUPPORTED.
     */
    InstanceHandleSet GetMatchedSubscriptionData()
    {
        return InstanceHandleSet();
    }

    /**
     * @brief This operation is intended to be used only if the DataWriter has RELIABILITY QoS kind set to RELIABLE. Otherwise
     * the operation will return immediately with RETCODE_OK.
     *
     * The operation wait_for_acknowledgments blocks the calling thread until either all data written by the DataWriter is
     * acknowledged by all matched DataReader entities that have RELIABILITY QoS kind RELIABLE, or else the duration
     * specified by the max_wait parameter elapses, whichever happens first. A return value of OK indicates that all the samples
     * written have been acknowledged by all reliable matched data readers; a return value of TIMEOUT indicates that
     * max_wait elapsed before all the data was acknowledged.
     */
    ReturnCode::Type WaitForAcknowledgments(const BaseLib::Duration &duration) // TODO: Default is = BaseLib::Duration::Maximum());
    {
        return ReturnCode::Ok;
    }

    /**
     * @brief This operation takes as a parameter an instance and returns a handle that can be used in subsequent operations that accept
     * an instance handle as an argument. The instance parameter is only used for the purpose of examining the fields that define
     * the key.
     *
     * This operation does not register the instance in question. If the instance has not been previously registered, or if for any
     * other reason the Service is unable to provide an instance handle, the Service will return the special value HANDLE_NIL.
     */
    InstanceHandle LookupInstance(const DATA &data)
    {
        return InstanceHandle::NIL();
    }

    /**
     * @brief This operation manually asserts the liveliness of the DataWriter. This is used in combination with the LIVELINESS QoS
     * policy (see Section 7.1.3, “Supported QoS,” on page 96) to indicate to the Service that the entity remains active.
     * This operation need only be used if the LIVELINESS setting is either MANUAL_BY_PARTICIPANT or
     * MANUAL_BY_TOPIC. Otherwise, it has no effect.
     *
     * Note – Writing data via the write operation on a DataWriter asserts liveliness on the DataWriter itself and its
     * DomainParticipant. Consequently the use of assert_liveliness is only needed if the application is not writing data regularly.
     * Complete details are provided in Section 7.1.3.11, “LIVELINESS,” on page 113.
     */
    ReturnCode::Type AssertLiveliness()
    {
        // TODO: Implement
        return ReturnCode::Ok;
    }

    /**
     * @brief SetListener installs a Listener on the Entity. The listener will only be invoked on the changes of communication status indicated by
     * the specified mask. It is permitted to use nil as the value of the listener. The nil listener behaves as a Listener whose operations perform
     * no action.
     *
     * Only one listener can be attached to each Entity. If a listener was already set, the operation set_listener will replace it with the new one.
     * Consequently if the value nil is passed for the listener parameter to the set_listener operation, any existing listener will be removed.
     *
     * @param listener
     * @param statusMask
     * @return
     */
    ReturnCode::Type SetListener(const typename DataWriterListener<DATA>::Ptr &listener, const Status::StatusMask &statusMask)
    {
        this->dataWriterListener_ = listener;
        this->statusMask_ = statusMask;

        return ReturnCode::Ok;
    }

    /**
     * @brief GetListener allows access to the existing Listener attached to the Entity.
     * @return
     */
    typename DataWriterListener<DATA>::Ptr GetListener()
    {
        return dataWriterListener_.lock();
    }

    /**
     * @brief GetInstanceHandle
     * @return
     */
    virtual InstanceHandle GetInstanceHandle() const
    {
        return InstanceHandle::NIL();
    }

    /**
     * @brief Enable
     * @return
     */
    virtual ReturnCode::Type Enable();

private:
    /**
     * @brief dataWriterListener_
     */
    typename DataWriterListener<DATA>::WeakPtr dataWriterListener_;
};

/**
 * @brief
 *
 * This operation informs the Service that the application will be modifying a particular instance. It gives an opportunity to
 * the Service to pre-configure itself to improve performance.
 *
 * It takes as a parameter an instance (to get the key value) and returns a handle that can be used in successive write or
 * dispose operations.
 *
 * This operation should be invoked prior to calling any operation that modifies the instance, such as write,
 * write_w_timestamp, dispose, and dispose_w_timestamp.
 *
 * The special value HANDLE_NIL may be returned by the Service if it does not want to allocate any handle for that
 * instance.
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * This operation may return OUT_OF_RESOURCES under the same circumstances described for the write operation
 * (Section 7.1.2.4.2.11).
 *
 * The operation register_instance is idempotent. If it is called for an already registered instance, it just returns the already
 * allocated handle. This may be used to lookup and retrieve the handle allocated to a given instance. The explicit use of this
 * operation is optional as the application may call directly the write operation and specify a HANDLE_NIL to indicate that
 * the ‘key’ should be examined to identify the instance.
 */
template <typename DATA>
InstanceHandle DataWriter<DATA>::RegisterInstance(const DATA &data)
{
    return registerInstance(data);
}

/**
 * @brief
 *
 * This operation performs the same function as register_instance and can be used instead of register_instance in the cases
 * where the application desires to specify the value for the source_timestamp. The source_timestamp potentially affects the
 * relative order in which readers observe events from multiple writers. For details see Section 7.1.3.17,
 * “DESTINATION_ORDER,” on page 116 for the QoS policy DESTINATION_ORDER.
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * This operation may return OUT_OF_RESOURCES under the same circumstances described for the write operation
 * (Section 7.1.2.4.2.11).
 */
template <typename DATA>
InstanceHandle DataWriter<DATA>::RegisterInstance(const DATA &data, const RTPS::Timestamp &timestamp)
{
    return registerInstance(data, timestamp);
}

/**
 * @brief DataWriter<DATA>::registerInstance
 * @param data
 * @param timestamp
 * @return
 */
template <typename DATA>
InstanceHandle DataWriter<DATA>::registerInstance(const DATA &data, const RTPS::Timestamp &timestamp)
{
    // --------------------------------------------
    // TODO: Generate the instance handle here
    // NO! Let the CacheWriter return the handle? NO it is sequence-number
    // --------------------------------------------
    InstanceHandle handle = InstanceHandle::NIL();

    // ------------------------------------------------------------------------------------
    // TODO: Check and act on the writer QoS...
    // ------------------------------------------------------------------------------------
    // QosChecker.check(qos);

    // --------------------------------------------
    // Write to the out-cache, i.e., RTPS
    // --------------------------------------------
    bool isWritten = this->config().GetCacheWriter(). template Write<DATA>(data, handle, timestamp);

    if(isWritten == false)
    {
        // -- debug --
        IWARNING() << "Failed to write to cache!";
        // -- debug --

        return InstanceHandle::NIL();
    }

    return handle;
}

/**
 * @brief
 *
 * This operation reverses the action of register_instance. It should only be called on an instance that is currently registered.
 *
 * The operation unregister_instance should be called just once per instance, regardless of how many times
 * register_instance was called for that instance.
 *
 * This operation informs the Service that the DataWriter is not intending to modify any more of that data instance. This
 * operation also indicates that the Service can locally remove all information regarding that instance. The application
 * should not attempt to use the handle previously allocated to that instance after calling unregister_instance.
 *
 * The special value HANDLE_NIL can be used for the parameter handle. This indicates that the identity of the instance
 * should be automatically deduced from the instance (by means of the key).
 *
 * If handle is any value other than HANDLE_NIL, then it must correspond to the value returned by register_instance when
 * the instance (identified by its key) was registered. Otherwise the behavior is as follows:
 * - If the handle corresponds to an existing instance but does not correspond to the same instance referred by the
 *    ‘instance’ parameter, the behavior is in general unspecified, but if detectable by the Service implementation, the
 * 	  operation shall fail and return the error-code ‘PRECONDITION_NOT_MET.’
 * - If the handle does not correspond to an existing instance the behavior is in general unspecified, but if detectable by the
 *    Service implementation, the operation shall fail and return the error-code ‘BAD_PARAMETER.’
 *
 * If after that, the application wants to modify (write or dispose) the instance, it has to register it again, or else use the
 * special handle value HANDLE_NIL.
 *
 * This operation does not indicate that the instance is deleted (that is the purpose of dispose). The operation
 * unregister_instance just indicates that the DataWriter no longer has ‘anything to say’ about the instance. DataReader
 * entities that are reading the instance will eventually receive a sample with a NOT_ALIVE_NO_WRITERS instance state
 * if no other DataWriter entities are writing the instance.
 *
 * This operation can affect the ownership of the data instance (as described in Section 7.1.3.9 and Section 7.1.3.23.1). If the
 * DataWriter was the exclusive owner of the instance, then calling unregister_instance will relinquish that ownership.
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * Possible error codes returned in addition to the standard ones: TIMEOUT, PRECONDITION_NOT_MET.
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::UnregisterInstance(const DATA &data, const InstanceHandle &handle)
{
    if(handle == InstanceHandle::NIL())
    {
        // instance handle should be deduced by DATA->GetKey();
    }

    bool isUnregistered = this->config().GetCacheWriter().Unregister(handle);

    if(isUnregistered == false)
    {
        // -- debug --
        IWARNING() << "Failed to unregister handle # " << handle.GetHandle();
        // -- debug --

        return ReturnCode::ALREADY_DELETED;
    }


    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation performs the same function as unregister_instance and can be used instead of unregister_instance in the
 * cases where the application desires to specify the value for the source_timestamp. The source_timestamp potentially
 * affects the relative order in which readers observe events from multiple writers. For details see Section 7.1.3.17,
 * “DESTINATION_ORDER,” on page 116 for the QoS policy DESTINATION_ORDER.
 *
 * The constraints on the values of the handle parameter and the corresponding error behavior are the same specified for the
 * unregister_instance operation (Section 7.1.2.4.2.7).
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::UnregisterInstance(const DATA &data, const InstanceHandle &handle, const RTPS::Timestamp &timestamp)
{
    IDEBUG() << "Implement";

    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation requests the middleware to delete the data (the actual deletion is postponed until there is no more use for
 * that data in the whole system). In general, applications are made aware of the deletion by means of operations on the
 * DataReader objects that already knew that instance13 (see Section 7.1.2.5, “Subscription Module,” on page 61 for more
 * details).
 *
 * This operation does not modify the value of the instance. The instance parameter is passed just for the purposes of
 * identifying the instance.
 *
 * When this operation is used, the Service will automatically supply the value of the source_timestamp that is made
 * available to DataReader objects by means of the source_timestamp attribute inside the SampleInfo.
 *
 * The constraints on the values of the handle parameter and the corresponding error behavior are the same specified for the
 * unregister_instance operation (Section 7.1.2.4.2.7).
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * This operation may return OUT_OF_RESOURCES under the same circumstances described for the write operation
 * (Section 7.1.2.4.2.11).
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::Dispose(const DATA &data, const InstanceHandle &handle)
{
    bool isDisposed = this->config().GetCacheWriter().Dispose(handle);

    if(isDisposed == false)
    {
        // -- debug --
        IWARNING() << "Failed to dispose handle # " << handle.GetHandle();
        // -- debug --

        return ReturnCode::ALREADY_DELETED;
    }

    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation performs the same functions as dispose except that the application provides the value for the
 * source_timestamp that is made available to DataReader objects by means of the source_timestamp attribute inside the
 * SampleInfo (see Section 7.1.2.5, “Subscription Module,” on page 61).
 *
 * The constraints on the values of the handle parameter and the corresponding error behavior are the same specified for the
 * dispose operation (Section 7.1.2.4.2.13).
 *
 * This operation may return PRECONDITION_NOT_MET under the same circumstances described for the dispose
 * operation (Section 7.1.2.4.2.13).
 *
 * This operation may return BAD_PARAMETER under the same circumstances described for the dispose operation
 * (Section 7.1.2.4.2.13).
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * This operation may return OUT_OF_RESOURCES under the same circumstances described for the write operation
 * (Section 7.1.2.4.2.11).
 *
 * Possible error codes returned in addition to the standard ones: TIMEOUT, PRECONDITION_NOT_MET.
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::Dispose(const DATA &data, const InstanceHandle &handle, const RTPS::Timestamp &timestamp)
{
    IDEBUG() << "Implement";

    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation modifies the value of a data instance. When this operation is used, the Service will automatically supply
 * the value of the source_timestamp that is made available to DataReader objects by means of the source_timestamp
 * attribute inside the SampleInfo. See Section 7.1.2.5, “Subscription Module,” on page 61 for more details on data
 * timestamps at reader side and Section 7.1.3.17, “DESTINATION_ORDER,” on page 116 for the QoS policy
 * DESTINATION_ORDER.
 *
 * This operation must be provided on the specialized class that is generated for the particular application data-type that is
 * being written. That way the data argument holding the data has the proper application-defined type (e.g., ‘Foo’).
 * As a side effect, this operation asserts liveliness on the DataWriter itself, the Publisher and the DomainParticipant.
 *
 * The special value HANDLE_NIL can be used for the parameter handle. This indicates that the identity of the instance
 * should be automatically deduced from the instance_data (by means of the key).
 *
 * If handle is any value other than HANDLE_NIL, then it must correspond to the value returned by register_instance when
 * the instance (identified by its key) was registered. Otherwise the behavior is as follows:
 *
 * - If the handle corresponds to an existing instance but does not correspond to the same instance referred by the 'data'
 * parameter, the behavior is in general unspecified, but if detectable by the Service implementation, the operation shall
 * fail and return the error-code ‘PRECONDITION_NOT_MET.’
 *
 * - If the handle does not correspond to an existing instance, the behavior is in general unspecified; but if detectable by
 * the Service implementation, the operation shall fail and return the error-code ‘BAD_PARAMETER.’
 *
 * If the RELIABILITY kind is set to RELIABLE, the write operation may block if the modification would cause data to be
 * lost or else cause one of the limits specified in the RESOURCE_LIMITS to be exceeded. Under these circumstances, the
 * RELIABILITY max_blocking_time configures the maximum time the write operation may block waiting for space to
 * become available. If max_blocking_time elapses before the DataWriter is able to store the modification without exceeding
 * the limits, the write operation will fail and return TIMEOUT.
 *
 * Specifically, the DataWriter write operation may block in the following situations (note that the list may not be
 * exhaustive), even if its HISTORY kind is KEEP_LAST.
 *
 * - If (RESOURCE_LIMITS max_samples < RESOURCE_LIMITS max_instances * HISTORY depth), then in the
 *    situation where the max_samples resource limit is exhausted the Service is allowed to discard samples of some other
 *   instance as long as at least one sample remains for such an instance. If it is still not possible to make space available to
 *  store the modification, the writer is allowed to block.
 * - If (RESOURCE_LIMITS max_samples < RESOURCE_LIMITS max_instances), then the DataWriter may block
 *    regardless of the HISTORY depth.
 *
 * Instead of blocking, the write operation is allowed to return immediately with the error code OUT_OF_RESOURCES
 * provided the following two conditions are met:
 *
 * 1. The reason for blocking would be that the RESOURCE_LIMITS are exceeded.
 * 2. The service determines that waiting the ‘max_waiting_time’ has no chance of freeing the necessary resources. For
 * example, if the only way to gain the necessary resources would be for the user to unregister an instance.
 *
 * In case the provided handle is valid, i.e., corresponds to an existing instance, but does not correspond to same instance
 * referred by the ‘data' parameter,’ the behavior is in general unspecified, but if detectable by the Service implementation,
 * the return error-code will be ‘PRECONDITION_NOT_MET.’ In case the handle is invalid, the behavior is in general
 * unspecified, but if detectable the returned error-code will be ‘BAD_PARAMETER.’
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::Write(const DATA &data, const InstanceHandle &handle)
{
    if(handle == InstanceHandle::NIL())
    {
        // instance handle should be deduced by DATA->GetKey();
    }

    // TODO:
    // - Implement suspend_publications and resume_publications inside the cacheWriter?
    // - The Publisher shares its CacheWriter with the DataWriter
    // - Let the Publisher control resume/suspend through a boolean in the CacheWriter API
    // - Inside CacheWriter there can be a wait function with a maximum timout

    bool isWritten = this->config().GetCacheWriter().template Write<DATA>(data, handle);

    if(isWritten == false)
    {
        // -- debug --
        IWARNING() << "Failed to write handle # " << handle.GetHandle();
        // -- debug --

        return ReturnCode::OUT_OF_RESOURCES;
    }

    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation performs the same function as write except that it also provides the value for the source_timestamp that is
 * made available to DataReader objects by means of the source_timestamp attribute inside the SampleInfo. See
 * Section 7.1.2.5, “Subscription Module,” on page 61 for more details on data timestamps at reader side and
 * Section 7.1.3.17, “DESTINATION_ORDER,” on page 116 for the QoS policy DESTINATION_ORDER.
 *
 * The constraints on the values of the handle parameter and the corresponding error behavior are the same specified for the
 * write operation (Section 7.1.2.4.2.11).
 *
 * This operation may block and return TIMEOUT under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * This operation may return OUT_OF_RESOURCES under the same circumstances described for the write operation
 * (Section 7.1.2.4.2.11).
 *
 * This operation may return PRECONDITION_NOT_MET under the same circumstances described for the write operation
 * (Section 7.1.2.4.2.11).
 *
 * This operation may return BAD_PARAMETER under the same circumstances described for the write operation (Section
 * 7.1.2.4.2.11).
 *
 * Similar to write, this operation must also be provided on the specialized class that is generated for the particular
 * application data-type that is being written.
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::Write(const DATA &data, const InstanceHandle &handle, const RTPS::Timestamp &timestamp)
{
    if(handle == InstanceHandle::NIL())
    {
        // instance handle should be deduced by DATA->GetKey();
    }

    bool isWritten = this->config().GetCacheWriter().template Write<DATA>(data, handle, timestamp);

    if(isWritten == false)
    {
        // -- debug --
        IWARNING() << "Failed to write handle # " << handle.GetHandle();
        // -- debug --

        return ReturnCode::OUT_OF_RESOURCES;
    }

    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * According spec:
 * - Calling enable on an already enabled Entity returns OK and has no effect.
 * - Calling enable on an Entity whose factory is not enabled will fail and
 * return PRECONDITION_NOT_MET.
 */
template <typename DATA>
ReturnCode::Type DataWriter<DATA>::Enable()
{
    return this->EnableParticipant(this->state().GetParticipant());
}

}}

#endif
