/*
 * DataReader.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Subscription_DataReader_h_IsIncluded
#define DCPS_Subscription_DataReader_h_IsIncluded

#include"DCPS/CommonDefines.h"

#include"DCPS/Sub/DataReaderListener.h"
#include"DCPS/Sub/Sample.h"
#include"DCPS/Sub/DataReaderStatus.h"

#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Topic/Topic.h"
#include"DCPS/Topic/TopicInstance.h"
#include"DCPS/Sub/ReadCondition.h"

#include"DCPS/IncludeExtLibs.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief The DataReaderConfig class
 */
class DLL_STATE DataReaderConfig
{
public:
    DataReaderConfig(Subscription::CacheReader cacheReader,
                     Status::DataState statusFilter,
                     DDS::DataReaderQoS dataReaderQos)
        : cacheReader_(cacheReader)
        , statusFilter_(statusFilter)
        , dataReaderQos_(dataReaderQos)
    {}
    virtual ~DataReaderConfig()
    {}

    const Status::DataState& GetDefaultStatusFilter() const
    {
        return statusFilter_;
    }

    void SetDefaultStatusFilter(const Status::DataState& status)
    {
        statusFilter_ = status;
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
    ReturnCode::Type SetQoS(const DDS::DataReaderQoS &qos)
    {
        dataReaderQos_ = qos;
        return ReturnCode::Ok;
    }

    const DDS::DataReaderQoS& GetQoS() const
    {
        return dataReaderQos_;
    }

    Subscription::CacheReader& GetCacheReader()
    {
        return cacheReader_;
    }

private:
    /**
     * @brief cacheReader_ provides the API to read instances from for example the RTPS implementation.
     */
    DCPS::Subscription::CacheReader cacheReader_;

    /**
     * @brief statusFilter_ filters out instances/samples
     */
    Status::DataState statusFilter_;

    /**
     * @brief dataReaderQos_ holds all the QoS policies that this DataReader is configured with.
     */
    DDS::DataReaderQoS dataReaderQos_;
};

/**
 * @brief The DataReaderState class
 */
template <typename DATA>
class DataReaderState
{
public:
    DataReaderState(Domain::DomainParticipantPtr participant,
                    SubscriberPtr subscriber,
                    typename Topic::Topic<DATA>::Ptr topic)
        : participant_(participant)
        , subscriber_(subscriber)
        , topic_(topic)
        , dataReaderStatus_()
    {}
    virtual ~DataReaderState()
    {}

    SubscriberPtr GetSubscriber() const
    {
        return subscriber_.lock();
    }

    typename DCPS::Topic::Topic<DATA>::Ptr GetTopic() const
    {
        return topic_;
    }

    Domain::DomainParticipantPtr GetParticipant() const
    {
        return participant_.lock();
    }

    DataReaderStatus& GetDataReaderStatus()
    {
        return dataReaderStatus_;
    }

    const DataReaderStatus& GetDataReaderStatus() const
    {
        return dataReaderStatus_;
    }

private:
    /**
     * @brief participant_
     */
    Domain::DomainParticipantWeakPtr participant_;

    /**
     * @brief subscriber_ is the creator/parent of this DataReader.
     */
    Subscription::SubscriberWeakPtr	subscriber_;

    /**
     * @brief topic_ is the Topic for this DataReader.
     */
    typename Topic::Topic<DATA>::Ptr topic_;

    /**
     * @brief dataReaderStatus_ holds all relevant statuses for the DataReader.
     */
    DataReaderStatus dataReaderStatus_;
};

/**
 * @brief
 *
 * A DataReader allows the application,
 * 1) to declare the data it wishes to receive (i.e., make a subscription), and
 * 2) to access the data received by the attached Subscriber.
 *
 * A DataReader refers to exactly one TopicDescription (either a Topic, a ContentFilteredTopic, or a MultiTopic) that
 * identifies the data to be read. The subscription has a unique resulting type. The data-reader may give access to several
 * instances of the resulting type, which can be distinguished from each other by their key (as described in Section 7.1.1.2.2,
 * Overall Conceptual Model, on page 9).
 *
 * DataReader is an abstract class. It must be specialized for each particular application data-type as shown in Figure 7.8 on
 * page 43. The additional methods that must be defined in the auto-generated class for a hypothetical application type
 * Foo are shown in the table below:
 */
template <typename DATA>
class DataReader
        : public DCPS::Infrastructure::Entity
        , public Templates::ContextObject<DataReaderConfig, DataReaderState<DATA>, Templates::NullCriticalState>
{
public:
    DataReader(Domain::DomainParticipantPtr participant,
               SubscriberPtr subscriber,
               typename Topic::Topic<DATA>::Ptr topic)
        : DCPS::Infrastructure::Entity(Status::StatusMask::All())
        , Templates::ContextObject<DataReaderConfig, DataReaderState<DATA>, Templates::NullCriticalState>
          (
              DataReaderConfig(Subscription::CacheReader(), Status::DataState::Any(), DDS::DataReaderQoS::Default()),
              DataReaderState<DATA>(participant, subscriber, topic)
          )
        , dataReaderListener_()
    { }

    DataReader(Domain::DomainParticipantPtr participant,
               SubscriberPtr subscriber,
               typename Topic::Topic<DATA>::Ptr topic,
               DDS::DataReaderQoS qos,
               typename DataReaderListener<DATA>::WeakPtr listener = DataReaderListener<DATA>::WeakPtr(),
               Status::StatusMask mask = Status::StatusMask::All())
        : DCPS::Infrastructure::Entity(mask)
        , Templates::ContextObject<DataReaderConfig, DataReaderState<DATA>, Templates::NullCriticalState>
          (
              DataReaderConfig(Subscription::CacheReader(), Status::DataState::Any(), qos),
              DataReaderState<DATA>(participant, subscriber, topic)
          )
        , dataReaderListener_(listener)
    { }

    virtual ~DataReader()
    { }

    CLASS_TRAITS(DataReader)

public:
    // --------------------------------------------------------------
    // Read/Take API
    // --------------------------------------------------------------

    /**
     * @brief
     *
     * This operation accesses a collection of Data values from the DataReader. The size of the returned collection will be
     * limited to the specified max_samples. The properties of the data_values collection and the setting of the
     * PRESENTATION QoS policy (see Section 7.1.3.6, PRESENTATION on page 110) may impose further limits on the
     * size of the returned list
     */
    bool Read(SampleSeq<DATA> &sampleSeq, const int32_t &maxSamples, const Status::DataState &dataState);

    /**
     * @brief Read
     * @param sampleSeq
     * @param maxSamples
     * @param readCondition
     * @return
     */
    bool Read(SampleSeq<DATA> &sampleSeq, const int32_t &maxSamples, const ReadCondition<DATA> &readCondition);

    /**
     * @brief ReadNext
     * @param sample
     * @param sampleInfo
     * @return
     */
    bool ReadNext(DATA &sample, SampleInfo &sampleInfo);

    /**
     * @brief
     *
     * This operation accesses a collection of data-samples from the DataReader and a corresponding collection of SampleInfo
     * structures. The operation will return either a list of samples or else a single sample. This is controlled by the
     * PRESENTATION QosPolicy using the same logic as for the read operation (see Section 7.1.2.5.3.8).
     *
     * The act of taking a sample removes it from the DataReader so it cannot be read or taken again. If the sample belongs
     * to the most recent generation of the instance, it will also set the view_state of the instance to NOT_NEW. It will not affect
     * the instance_state of the instance.
     */
    bool Take(SampleSeq<DATA> &sampleSeq, const int32_t &maxSamples, const Status::DataState &dataState);


    /*LoanedSamples<DATA> Read()
    {
          //raw_reader_->read(data, info, DDS::LENGTH_UNLIMITED,
          //      DDS::NOT_READ_SAMPLE_STATE, DDS::ANY_VIEW_STATE,
          //      DDS::ALIVE_INSTANCE_STATE);

        return LoanedSamples<DATA>();
    }

    LoanedSamples<DATA> Take()
    {
          //raw_reader_->take(data, info, DDS::LENGTH_UNLIMITED,
          //      DDS::NOT_READ_SAMPLE_STATE, DDS::ANY_VIEW_STATE,
          //      DDS::ALIVE_INSTANCE_STATE);

        return LoanedSamples<DATA>();
    }*/

public:
    // --------------------------------------------------------------
    //  Condition creations
    // --------------------------------------------------------------

    /**
     * @brief
     *
     * This operation creates a ReadCondition. The returned ReadCondition will be attached and belong to the DataReader.
     * In case of failure, the operation will return a nil value (as specified by the platform).
     */
    ReadCondition<DATA> CreateReadCondition(const Status::DataState &dataStates)
    {
        ReadCondition<DATA> readCondition(*this, dataStates);

        // TODO: Add readCondition to this class

        return readCondition;
    }

    /**
     * @brief
     *
     * This operation deletes a ReadCondition attached to the DataReader. Since QueryCondition specializes ReadCondition it
     * can also be used to delete a QueryCondition. If the ReadCondition is not attached to the DataReader, the operation will
     * return the error PRECONDITION_NOT_MET.
     * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
     */
    bool DeleteReadCondition(const ReadCondition<DATA> &readCondition)
    {
        // TODO: Remove readCondition from class
        return false;
    }

    /**
     * @brief
     *
     * This operation creates a QueryCondition. The returned QueryCondition will be attached and belong to the DataReader.
     * The syntax of the query_expression and query_parameters parameters is described in Annex A.
     * In case of failure, the operation will return a nil value (as specified by the platform).
     */
    //DCPS::QueryCondition CreateQueryCondition(const Status::DataState &dataStates, const DCPS::Core::Query &query) { }

public:
    // --------------------------------------------------------------
    // Status Filter
    // --------------------------------------------------------------

    /**
     * @brief GetDefaultStatusFilter
     * @return
     */
    const Status::DataState& GetDefaultStatusFilter() const
    {
        return this->config().GetDefaultStatusFilter();
    }

    /**
     * @brief SetDefaultStatusFilter
     * @param status
     */
    void SetDefaultStatusFilter(const Status::DataState& status)
    {
        this->config().SetDefaultStatusFilter(status);
    }

public:
    // --------------------------------------------------------------
    // Instance Management
    // --------------------------------------------------------------

    /**
     * @brief GetKeyValue
     * @param handle
     * @return
     */
    Topic::TopicInstance<DATA> GetKeyValue(const InstanceHandle& handle)
    {
        DATA keyHolder;

        //rawReader_->GetKeyValue(keyHolder, handle);

        return Topic::TopicInstance<DATA>(keyHolder, handle);
    }

    /**
     * @brief LookupInstance
     * @param key
     * @return
     */
    const InstanceHandle LookupInstance(const DATA& key) const
    {
        InstanceHandle handle; // = rawReader_->LookupInstance(key);

        // -- debug --
        if(handle.IsNil()) IWARNING() << "handle is Nil";
        // -- debug --

        return handle;
    }

    /**
     * @brief ReturnLoan
     * @param sampleSeq
     * @return
     */
    bool ReturnLoan(const SampleSeq<DATA> &sampleSeq)
    {
        IDEBUG() << "TODO: Implement";
        return false;
    }

    /**
     * @brief GetDataReaderStatus returns status management
     * @return
     */
    const DataReaderStatus& GetDataReaderStatus() const
    {
        return this->state().GetDataReaderStatus();
    }

public:
    // --------------------------------------------------------------
    //	Getters for private variables
    // --------------------------------------------------------------

    /**
     * @brief GetTopic
     * @return
     */
    typename Topic::Topic<DATA>::Ptr GetTopic() const
    {
        return this->state().GetTopic();;
    }

    /**
     * @brief Parent
     * @return
     */
    Subscription::SubscriberPtr	Parent() const
    {
        return this->state().GetSubscriber();
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
    ReturnCode::Type SetListener(const typename DataReaderListener<DATA>::Ptr &listener, const Status::StatusMask &statusMask)
    {
        this->dataReaderListener_ = listener;
        this->statusMask_ = statusMask;

        return ReturnCode::Ok;
    }

    /**
     * @brief GetListener allows access to the existing Listener attached to the Entity.
     * @return
     */
    typename DataReaderListener<DATA>::Ptr GetListener()
    {
        return dataReaderListener_.lock();
    }

    /**
     * @brief GetInstanceHandle
     * @return
     */
    virtual InstanceHandle GetInstanceHandle() const
    {
        // TODO: Implement
        return InstanceHandle::NIL();
    }

    /**
     * @brief Enable
     * @return
     */
    virtual ReturnCode::Type Enable()
    {
        return this->EnableParticipant(this->state().GetParticipant());
    }

private:
    /**
     * @brief dataReaderListener_ is the attached listener.
     */
    typename DataReaderListener<DATA>::WeakPtr dataReaderListener_;
};

/**
 * This operation accesses a collection of Data values from the DataReader. The size of the returned collection will be
 * limited to the specified max_samples. The properties of the data_values collection and the setting of the
 * PRESENTATION QoS policy (see Section 7.1.3.6, “PRESENTATION,” on page 110) may impose further limits on the
 * size of the returned ‘list.’
 *
 * 1. If PRESENTATION access_scope is INSTANCE, then the returned collection is a ‘list’ where samples belonging
 * to the same data-instance are consecutive.
 * 2. If PRESENTATION access_scope is TOPIC and ordered_access is set to FALSE, then the returned collection is a
 * ‘list’ where samples belonging to the same data-instance are consecutive.
 * 3. If PRESENTATION access_scope is TOPIC and ordered_access is set to TRUE, then the returned collection is a
 * ‘list’ were samples belonging to the same instance may or may not be consecutive. This is because to preserve
 * order it may be necessary to mix samples from different instances.
 * 4. If PRESENTATION access_scope is GROUP and ordered_access is set to FALSE, then the returned collection is
 * a ‘list’ where samples belonging to the same data instance are consecutive.
 * 5. If PRESENTATION access_scope is GROUP and ordered_access is set to TRUE, then the returned collection
 * contains at most one sample. The difference in this case is due to the fact that it is required that the application is
 * able to read samples belonging to different DataReader objects in a specific order.
 *
 * In any case, the relative order between the samples of one instance is consistent with the DESTINATION_ORDER
 * QosPolicy:
 * - If DESTINATION_ORDER is BY_RECEPTION_TIMESTAMP, samples belonging to the same instances will appear
 *    in the relative order in which they were received (FIFO, earlier samples ahead of the later samples).
 * - If DESTINATION_ORDER is BY_SOURCE_TIMESTAMP, samples belonging to the same instances will appear in
 *    the relative order implied by the source_timestamp (FIFO, smaller values of source_timestamp ahead of the larger
 *   values).
 *
 * In addition to the collection of samples, the read operation also uses a collection of SampleInfo structures (sample_infos),
 * see Section 7.1.2.5.5, “SampleInfo Class,” on page 91.
 * The initial (input) properties of the data_values and sample_infos collections will determine the precise behavior of read
 * operation. For the purposes of this description the collections are modeled as having three properties: the current-length
 * (len), the maximum length (max_len), and whether the collection container owns the memory of the elements within
 * (owns). PSM mappings that do not provide these facilities may need to change the signature of the read operation slightly
 * to compensate for it.
 *
 * The initial (input) values of the len, max_len, and owns properties for the data_values and sample_infos collections
 * govern the behavior of the read operation as specified by the following rules:
 *
 * 1. The values of len, max_len, and owns for the two collections must be identical. Otherwise read will and return
 * PRECONDITION_NOT_MET.
 * 2. On successful output, the values of len, max_len, and owns will be the same for both collections.
 * 3. If the input max_len==0, then the data_values and sample_infos collections will be filled with elements that are
 * ‘loaned’ by the DataReader. On output, owns will be FALSE, len will be set to the number of values returned, and
 * max_len will be set to a value verifying max_len >= len. The use of this variant allows for zero-copy22 access to the
 * data and the application will need to “return the loan” to the DataWriter using the return_loan operation (see Section
 * 7.1.2.5.3.20).
 * 4. If the input max_len>0 and the input owns==FALSE, then the read operation will fail and return
 * PRECONDITION_NOT_MET. This avoids the potential hard-to-detect memory leaks caused by an application
 * forgetting to “return the loan.”
 * 5. If input max_len>0 and the input owns==TRUE, then the read operation will copy the Data values and
 * SampleInfo values into the elements already inside the collections. On output, owns will be TRUE, len will be set
 * to the number of values copied, and max_len will remain unchanged. The use of this variant forces a copy but the
 * application can control where the copy is placed and the application will not need to “return the loan.” The number
 * of samples copied depends on the relative values of max_len and max_samples:
 * - If max_samples = LENGTH_UNLIMITED, then at most max_len values will be copied. The use of this variant
 *    lets the application limit the number of samples returned to what the sequence can accommodate.
 * - If max_samples <= max_len, then at most max_samples values will be copied. The use of this variant lets the
 *    application limit the number of samples returned to fewer that what the sequence can accommodate.
 * - If max_samples > max_len, then the read operation will fail and return PRECONDITION_NOT_MET. This
 *     avoids the potential confusion where the application expects to be able to access up to max_samples, but that
 *     number can never be returned, even if they are available in the DataReader, because the output sequence cannot
 *     accommodate them.
 *
 * As described above, upon return the data_values and sample_infos collections may contain elements “loaned” from the
 * DataReader. If this is the case, the application will need to use the return_loan operation (see Section 7.1.2.5.3.20) to
 * return the “loan” once it is no longer using the Data in the collection. Upon return from return_loan, the collection will
 * have max_len=0 and owns=FALSE.
 *
 * The application can determine whether it is necessary to “return the loan” or not based on how the state of the collections
 * when the read operation was called, or by accessing the ‘owns’ property. However, in many cases it may be simpler to
 * always call return_loan, as this operation is harmless (i.e., leaves all elements unchanged) if the collection does not have
 * a loan.
 *
 * To avoid potential memory leaks, the implementation of the Data and SampleInfo collections should disallow changing
 * the length of a collection for which owns==FALSE. Furthermore, deleting a collection for which owns==FALSE should
 * be considered an error.
 * On output, the collection of Data values and the collection of SampleInfo structures are of the same length and are in a
 * one-to-one correspondence. Each SampleInfo provides information, such as the source_timestamp, the sample_state,
 * view_state, and instance_state, etc. about the corresponding sample.
 *
 * Some elements in the returned collection may not have valid data. If the instance_state in the SampleInfo is
 * NOT_ALIVE_DISPOSED or NOT_ALIVE_NO_WRITERS, then the last sample for that instance in the collection, that
 * is, the one whose SampleInfo has sample_rank==0 does not contain valid data. Samples that contain no data do not count
 * towards the limits imposed by the RESOURCE_LIMITS QoS policy.
 *
 * The act of reading a sample sets its sample_state to READ. If the sample belongs to the most recent generation of the
 * instance, it will also set the view_state of the instance to NOT_NEW. It will not affect the instance_state of the instance.
 * This operation must be provided on the specialized class that is generated for the particular application data-type that is
 * being read.
 *
 * If the DataReader has no samples that meet the constraints, the return value will be NO_DATA.
 */
template <typename DATA>
bool DataReader<DATA>::Read(SampleSeq<DATA> &sampleSeq,
                            const int32_t &maxSamples,
                            const Status::DataState &dataState)
{
    return true;
}

template <typename DATA>
bool DataReader<DATA>::Read(SampleSeq<DATA> &sampleSeq,
                            const int32_t &maxSamples,
                            const ReadCondition<DATA> &readCondition)
{
    return false;
}

template <typename DATA>
bool DataReader<DATA>::ReadNext(DATA &sample, SampleInfo &sampleInfo)
{
    return false;
}

template <typename DATA>
bool DataReader<DATA>::Take(SampleSeq<DATA> &sampleSeq,
                            const int32_t &maxSamples,
                            const Status::DataState &dataState)
{
    return true;
}


}} // namespace DCPS::Subscription

#endif // DCPS_Subscription_DataReader_h_IsIncluded
