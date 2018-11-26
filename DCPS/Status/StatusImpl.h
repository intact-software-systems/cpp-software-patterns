#ifndef DCPS_Status_StatusImpl_h_Included
#define DCPS_Status_StatusImpl_h_Included

#include"DCPS/CommonDefines.h"
#include"DCPS/Status/State.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Status { namespace Impl
{

/**
 * @brief Count number of inconsistent topics
 *
 * @arg totalCount_
 * 		Total cumulative count of the Topics discovered whose name matches
 * 		the Topic to which this status is attached and whose type is
 * 		inconsistent with the Topic.
 *
 * @arg totalCountChange_
 * 		The incremental number of inconsistent topics discovered since the
 * 		last time the listener was called or the status was read.
 */
class DLL_STATE InconsistentTopicStatusImpl
{
public:
    InconsistentTopicStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
    { }

    InconsistentTopicStatusImpl(int32_t totalCount, int32_t totalCountChange)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
    { }

    CLASS_TRAITS(InconsistentTopicStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const InconsistentTopicStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ && totalCountChange_ == other.totalCountChange_);
    }

    bool operator!=(const InconsistentTopicStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const InconsistentTopicStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const InconsistentTopicStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const InconsistentTopicStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const InconsistentTopicStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;
};

/**
 * @brief Count number of samples lost
 *
 * @arg totalCount_
 * 		Total cumulative count of all samples lost across of instances
 * 		of data published under the Topic.
 *
 * @arg totalCountChange_
 * 		The incremental number of samples lost since the last time the
 * 		listener was called or the status was read.
 */
class DLL_STATE SampleLostStatusImpl
{
public:
    SampleLostStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
    { }

    SampleLostStatusImpl(int32_t totalCount, int32_t totalCountChange)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
    { }

    CLASS_TRAITS(SampleLostStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const SampleLostStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ && totalCountChange_ == other.totalCountChange_);
    }

    bool operator!=(const SampleLostStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const SampleLostStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const SampleLostStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const SampleLostStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const SampleLostStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;
};

/**
 * @brief Count number of samples rejected
 *
 * @arg totalCount_
 * 		Total cumulative count of samples rejected by the DataReader.
 *
 * @arg totalCountChange_
 * 		The incremental number of samples rejected since the last time the listener
 * 		was called or the status was read.
 *
 * @arg lastReason_
 *		Reason for rejecting the last sample rejected. If no samples have been
 *		rejected, the reason is the special value NOT_REJECTED.
 *
 * @arg lastInstanceHandle_
 *		Handle to the instance being updated by the last sample that was rejected.
 *
 */
class DLL_STATE SampleRejectedStatusImpl
{
public:
    SampleRejectedStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , lastReason_(Status::SampleRejectedState::NotRejected())
        , lastInstanceHandle_()
    { }

    SampleRejectedStatusImpl(int32_t totalCount
                            , int32_t totalCountChange
                            , Status::SampleRejectedState lastReason
                            , InstanceHandle lastInstancHandle)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , lastReason_(lastReason)
        , lastInstanceHandle_(lastInstancHandle)
    { }

    CLASS_TRAITS(SampleRejectedStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

    Status::SampleRejectedState 		GetLastReason() 			const { return lastReason_; }
    InstanceHandle 	GetLastInstanceHandle() 	const { return lastInstanceHandle_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

    void SetLastReason(const Status::SampleRejectedState &lastReason) 				{ lastReason_ = lastReason; }
    void SetLastInstanceHandle(const InstanceHandle &lastHandle) 	{ lastInstanceHandle_ = lastHandle; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const SampleRejectedStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                lastReason_ == other.lastReason_ &&
                lastInstanceHandle_ == other.lastInstanceHandle_);
    }

    bool operator!=(const SampleRejectedStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const SampleRejectedStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const SampleRejectedStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const SampleRejectedStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const SampleRejectedStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;

    Status::SampleRejectedState 		lastReason_;
    InstanceHandle 	lastInstanceHandle_;
};


/**
 * @brief Count number of time a DataWriter failed to report alive
 *
 * @arg totalCount_
 * 		Total cumulative number of times that a previously-alive DataWriter
 * 		became not alive due to a failure to actively signal its liveliness within
 * 		its offered liveliness period. This count does not change when an already
 * 		not alive DataWriter simply remains not alive for another liveliness period.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called or
 * 		the status was read.
 */
class DLL_STATE LivelinessLostStatusImpl
{
public:
    LivelinessLostStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
    { }

    LivelinessLostStatusImpl(int32_t totalCount, int32_t totalCountChange)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
    { }

    CLASS_TRAITS(LivelinessLostStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const LivelinessLostStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ && totalCountChange_ == other.totalCountChange_);
    }

    bool operator!=(const LivelinessLostStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const LivelinessLostStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const LivelinessLostStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const LivelinessLostStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const LivelinessLostStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;
};

/**
 * @brief Status on the number of times the liveliness of a DataWriter changed
 *
 * @arg aliveCount_
 *
 * 		The total number of currently active DataWriters that write the Topic
 * 		read by the DataReader. This count increases when a newly matched
 * 		DataWriter asserts its liveliness for the first time or when a DataWriter
 * 		previously considered to be not alive reasserts its liveliness. The count
 * 		decreases when a DataWriter considered alive fails to assert its liveliness
 * 		and becomes not alive, whether because it was deleted normally or for some other reason.
 *
 * @arg notAliveCount_
 *
 * 		The total count of currently DataWriters that write the Topic read by
 * 		the DataReader that are no longer asserting their liveliness. This count
 * 		increases when a DataWriter considered alive fails to assert its
 * 		liveliness and becomes not alive for some reason other than the normal
 * 		deletion of that DataWriter. It decreases when a previously not alive
 * 		DataWriter either reasserts its liveliness or is deleted normally.
 *
 * @arg aliveCountChange_
 *
 * 		The change in the alive_count since the last time the listener was
 * 		called or the status was read.
 *
 * @arg notAliveCountChange_
 *
 *		The change in the not_alive_count since the last time the listener was
 *		called or the status was read.
 *
 * @arg lastPublicationHandle_
 *
 *		Handle to the last DataWriter whose change in liveliness caused this
 *		status to change.
 */
class DLL_STATE LivelinessChangedStatusImpl
{
public:
    LivelinessChangedStatusImpl()
        : aliveCount_(0)
        , notAliveCount_(0)
        , aliveCountChange_(0)
        , notAliveCountChange_(0)
        , lastPublicationHandle_()
    { }

    LivelinessChangedStatusImpl(int32_t aliveCount
                                , int32_t notAliveCount
                                , int32_t aliveCountChange
                                , int32_t notAliveCountChange
                                , InstanceHandle lastPublicationHandle)
        : aliveCount_(aliveCount)
        , notAliveCount_(notAliveCount)
        , aliveCountChange_(aliveCountChange)
        , notAliveCountChange_(notAliveCountChange)
        , lastPublicationHandle_(lastPublicationHandle)
    { }

    CLASS_TRAITS(LivelinessChangedStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetAliveCount() 			const { return aliveCount_; }
    int32_t GetNotAliveCount() 			const { return notAliveCount_; }
    int32_t GetAliveCountChange() 		const { return aliveCountChange_; }
    int32_t GetNotAliveCountChhange() 	const { return notAliveCountChange_; }

    const InstanceHandle GetLastPublicationHandle() const
    {
        return lastPublicationHandle_;
    }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetAliveCount(int32_t aliveCount) 						{ aliveCount_ = aliveCount; }
    void SetNotAliveCount(int32_t notAliveCount) 				{ notAliveCount_ = notAliveCount; }
    void SetAliveCountChange(int32_t aliveCountChange) 			{ aliveCountChange_ = aliveCountChange; }
    void SetNotAliveCountChhange(int32_t notAliveCountChange) 	{ notAliveCountChange_ = notAliveCountChange; }

    void SetLastPublicationHandle(const InstanceHandle &lastPublicationHandle)
    {
        lastPublicationHandle_ = lastPublicationHandle;
    }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncAliveCount()            { aliveCount_++; }
    void IncAliveCountChange()      { aliveCountChange_++; }
    void IncNotAliveCount()         { notAliveCount_++; }
    void IncNotAliveCountChange()   { notAliveCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const LivelinessChangedStatusImpl &other) const
    {
        return (aliveCount_ == other.aliveCount_ &&
                notAliveCount_ == other.notAliveCount_ &&
                aliveCountChange_ == other.aliveCountChange_ &&
                notAliveCountChange_ == other.notAliveCountChange_);
    }

    bool operator!=(const LivelinessChangedStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const LivelinessChangedStatusImpl &other) const
    {
        return (aliveCount_ < other.aliveCount_ &&
                notAliveCount_ < other.notAliveCount_);
    }

    bool operator<=(const LivelinessChangedStatusImpl &other) const
    {
        return (aliveCount_ <= other.aliveCount_ &&
                notAliveCount_ <= other.notAliveCount_);
    }

    bool operator>(const LivelinessChangedStatusImpl &other) const
    {
        return (aliveCount_ > other.aliveCount_ &&
                notAliveCount_ > other.notAliveCount_);
    }

    bool operator>=(const LivelinessChangedStatusImpl &other) const
    {
        return (aliveCount_ >= other.aliveCount_ &&
                notAliveCount_ >= other.notAliveCount_);
    }

protected:
    int32_t aliveCount_;
    int32_t notAliveCount_;
    int32_t aliveCountChange_;
    int32_t notAliveCountChange_;

    InstanceHandle lastPublicationHandle_;
};

/**
 * @brief Count number of deadline misses.
 *
 * @arg totalCount_
 * 		Total cumulative number of offered deadline periods elapsed during
 * 		which a DataWriter failed to provide data. Missed deadlines accumulate;
 * 		that is, each deadline period the total_count will be incremented by one.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called or
 * 		the status was read.
 *
 * @arg lastInstanceHandle_
 *		Handle to the last instance in the DataWriter for which an offered
 *		deadline was missed.
 */
class DLL_STATE OfferedDeadlineMissedStatusImpl
{
public:
    OfferedDeadlineMissedStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , lastInstanceHandle_()
    { }

    OfferedDeadlineMissedStatusImpl(int32_t totalCount
                                    , int32_t totalCountChange
                                    , InstanceHandle lastInstancHandle)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , lastInstanceHandle_(lastInstancHandle)
    { }

    CLASS_TRAITS(OfferedDeadlineMissedStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

    InstanceHandle GetLastInstanceHandle() 	const { return lastInstanceHandle_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

    void SetLastInstanceHandle(const InstanceHandle &lastHandle) 	{ lastInstanceHandle_ = lastHandle; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const OfferedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                lastInstanceHandle_ == other.lastInstanceHandle_);
    }

    bool operator!=(const OfferedDeadlineMissedStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const OfferedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const OfferedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const OfferedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const OfferedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;

    InstanceHandle lastInstanceHandle_;
};

/**
 * @brief Count number of deadline misses on DataReader.
 *
 * @arg totalCount_
 * 		Total cumulative number of missed deadlines detected for any instance
 * 		read by the DataReader. Missed deadlines accumulate; that is, each
 * 		deadline period the total_count will be incremented by one for each
 * 		instance for which data was not received.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called or
 * 		the status was read.
 *
 * @arg lastInstanceHandle_
 *		Handle to the last instance in the DataReader for which a deadline was
 *		detected.
 */
class DLL_STATE RequestedDeadlineMissedStatusImpl
{
public:
    RequestedDeadlineMissedStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , lastInstanceHandle_()
    { }

    RequestedDeadlineMissedStatusImpl(int32_t totalCount
                                    , int32_t totalCountChange
                                    , InstanceHandle lastInstancHandle)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , lastInstanceHandle_(lastInstancHandle)
    { }

    CLASS_TRAITS(RequestedDeadlineMissedStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

    InstanceHandle GetLastInstanceHandle() 	const { return lastInstanceHandle_; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

    void SetLastInstanceHandle(const InstanceHandle &lastHandle) 	{ lastInstanceHandle_ = lastHandle; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const RequestedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                lastInstanceHandle_ == other.lastInstanceHandle_);
    }

    bool operator!=(const RequestedDeadlineMissedStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const RequestedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const RequestedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const RequestedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const RequestedDeadlineMissedStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;

    InstanceHandle lastInstanceHandle_;
};

/**
 * @brief Status on how often there has been an incompatibility in the offered QoS.
 *
 * @arg totalCount_
 *  	Total cumulative number of times the concerned DataWriter
 *  	discovered a DataReader for the same Topic with a requested QoS that
 *  	is incompatible with that offered by the DataWriter.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called or
 * 		the status was read.
 *
 * @arg lastPolicyId_
 * 		The PolicyId_t of one of the policies that was found to be incompatible
 * 		the last time an incompatibility was detected.
 *
 * @arg policies_
 * 		A list containing for each policy the total number of times that the
 * 		concerned DataWriter discovered a DataReader for the same Topic with
 * 		a requested QoS that is incompatible with that offered by the DataWriter.
 */
class DLL_STATE OfferedIncompatibleQosStatusImpl
{
public:
    OfferedIncompatibleQosStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , lastPolicyId_(0)
        , policies_()
    { }

    OfferedIncompatibleQosStatusImpl(int32_t totalCount
                                    , int32_t totalCountChange
                                    , DDS::Policy::QosPolicyId lastPolicyId
                                    , const DDS::Policy::QosPolicyCountSeq &policies)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , lastPolicyId_(lastPolicyId)
        , policies_(policies)
    { }

    CLASS_TRAITS(OfferedIncompatibleQosStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

    DDS::Policy::QosPolicyId GetLastPolicyId() 	const { return lastPolicyId_; }
    DDS::Policy::QosPolicyCountSeq GetPolicies() 	const { return policies_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

    void SetLastPolicyId(const DDS::Policy::QosPolicyId &lastPolicyId)	{ lastPolicyId_ = lastPolicyId; }
    void SetPolicies(const DDS::Policy::QosPolicyCountSeq &policies) 		{ policies_ = policies; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const OfferedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                lastPolicyId_ == other.lastPolicyId_);
                // && policies_ == other.policies_);
    }

    bool operator!=(const OfferedIncompatibleQosStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const OfferedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const OfferedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const OfferedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const OfferedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;

    DDS::Policy::QosPolicyId 		lastPolicyId_;
    DDS::Policy::QosPolicyCountSeq 	policies_;
};

/**
 * @brief Status on how often there has been an incompatibility in the requested QoS.
 *
 * @arg totalCount_
 * 		Total cumulative number of times the concerned DataReader
 * 		discovered a DataWriter for the same Topic with an offered QoS that
 * 		was incompatible with that requested by the DataReader.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called or
 * 		the status was read.
 *
 * @arg lastPolicyId_
 * 		The PolicyId_t of one of the policies that was found to be incompatible
 * 		the last time an incompatibility was detected.
 *
 * @arg policies_
 * 		A list containing for each policy the total number of times that the
 * 		concerned DataReader discovered a DataWriter for the same Topic
 * 		with an offered QoS that is incompatible with that requested by the
 * 		DataReader.
 */
class DLL_STATE RequestedIncompatibleQosStatusImpl
{
public:
    RequestedIncompatibleQosStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , lastPolicyId_(0)
        , policies_()
    { }

    RequestedIncompatibleQosStatusImpl(int32_t totalCount
                                    , int32_t totalCountChange
                                    , DDS::Policy::QosPolicyId lastPolicyId
                                    , const DDS::Policy::QosPolicyCountSeq &policies)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , lastPolicyId_(lastPolicyId)
        , policies_(policies)
    { }

    CLASS_TRAITS(RequestedIncompatibleQosStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }

    DDS::Policy::QosPolicyId GetLastPolicyId() 			const { return lastPolicyId_; }
    const DDS::Policy::QosPolicyCountSeq GetPolicies() 	const { return policies_; }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 				{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 	{ totalCountChange_ = totalCountChange; }

    void SetLastPolicyId(const DDS::Policy::QosPolicyId &lastPolicyId)	{ lastPolicyId_ = lastPolicyId; }
    void SetPolicies(const DDS::Policy::QosPolicyCountSeq &policies) 	{ policies_ = policies; }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const RequestedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                lastPolicyId_ == other.lastPolicyId_);
                //policies_ == other.policies_);
    }

    bool operator!=(const RequestedIncompatibleQosStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const RequestedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const RequestedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const RequestedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const RequestedIncompatibleQosStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;

    DDS::Policy::QosPolicyId 		lastPolicyId_;
    DDS::Policy::QosPolicyCountSeq 	policies_;
};

/**
 * @brief Status on number of times publications were matched.
 *
 * @arg totalCount_
 * 		Total cumulative count the concerned DataWriter discovered a
 * 		"match" with a DataReader. That is, it found a DataReader for the
 * 		same Topic with a requested QoS that is compatible with that offered
 * 		by the DataWriter.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called
 * 		or the status was read.
 *
 * @arg currentCount_
 * 		The number of DataReaders currently matched to the concerned DataWriter.
 *
 * @arg currentCountChange_
 * 		The change in current_count since the last time the listener was called
 * 		or the status was read.
 *
 * @arg lastSubscriptionHandle_
 * 		Handle to the last DataReader that matched the DataWriter causing the
 * 		status to change.
 */
class DLL_STATE PublicationMatchedStatusImpl
{
public:
    PublicationMatchedStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , currentCount_(0)
        , currentCountChange_(0)
        , lastSubscriptionHandle_()
    { }

    PublicationMatchedStatusImpl(int32_t totalCount
                                , int32_t totalCountChange
                                , int32_t currentCount
                                , int32_t currentCountChange
                                , const InstanceHandle &lastSubscriptionHandle)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , currentCount_(currentCount)
        , currentCountChange_(currentCountChange)
        , lastSubscriptionHandle_(lastSubscriptionHandle)
    { }

    CLASS_TRAITS(PublicationMatchedStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }
    int32_t GetCurrentCount() 		const { return currentCount_; }
    int32_t GetCurrentCountChange() const { return currentCountChange_; }

    InstanceHandle GetLastSubscriptionHandle() const
    {
        return lastSubscriptionHandle_;
    }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 					{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 		{ totalCountChange_ = totalCountChange; }
    void SetCurrentCount(int32_t currentCount) 				{ currentCount_ = currentCount; }
    void SetCurrentCountChange(int32_t currentCountChange) 	{ currentCountChange_ = currentCountChange; }

    void SetLastSubscriptionHandle(const InstanceHandle &lastSubscriptionHandle)
    {
        lastSubscriptionHandle_ = lastSubscriptionHandle;
    }
public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }
    void IncCurrentCount()          { currentCount_++; }
    void IncCurrentCountChange()    { currentCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const PublicationMatchedStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                currentCount_ == other.currentCount_ &&
                currentCountChange_ == other.currentCountChange_ &&
                lastSubscriptionHandle_ == other.lastSubscriptionHandle_);
    }

    bool operator!=(const PublicationMatchedStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const PublicationMatchedStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const PublicationMatchedStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const PublicationMatchedStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const PublicationMatchedStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;
    int32_t currentCount_;
    int32_t currentCountChange_;

    InstanceHandle lastSubscriptionHandle_;
};

/**
 * @brief Status on number of times subscriptions were matched.
 *
 * @arg totalCount_
 * 		Total cumulative count the concerned DataReader discovered a "match"
 * 		with a DataWriter. That is, it found a DataWriter for the same Topic
 * 		with a requested QoS that is compatible with that offered by the
 * 		DataReader.
 *
 * @arg totalCountChange_
 * 		The change in total_count since the last time the listener was called
 * 		or the status was read.
 *
 * @arg currentCount_
 * 		The number of DataWriters currently matched to the concerned DataReader.
 *
 * @arg currentCountChange_
 * 		The change in current_count since the last time the listener was called
 * 		or the status was read.
 *
 * @arg lastPublicationHandle_
 * 		Handle to the last DataWriter that matched the DataReader causing the
 * 		status to change.
 */
class DLL_STATE SubscriptionMatchedStatusImpl
{
public:
    SubscriptionMatchedStatusImpl()
        : totalCount_(0)
        , totalCountChange_(0)
        , currentCount_(0)
        , currentCountChange_(0)
        , lastPublicationHandle_()
    { }

    SubscriptionMatchedStatusImpl(int32_t totalCount
                                , int32_t totalCountChange
                                , int32_t currentCount
                                , int32_t currentCountChange
                                , const InstanceHandle &lastPublicationHandle)
        : totalCount_(totalCount)
        , totalCountChange_(totalCountChange)
        , currentCount_(currentCount)
        , currentCountChange_(currentCountChange)
        , lastPublicationHandle_(lastPublicationHandle)
    { }

    CLASS_TRAITS(SubscriptionMatchedStatusImpl)

public:
    // ------------------------------------------------
    // Getters
    // ------------------------------------------------
    int32_t GetTotalCount() 		const { return totalCount_; }
    int32_t GetTotalCountChange() 	const { return totalCountChange_; }
    int32_t GetCurrentCount() 		const { return currentCount_; }
    int32_t GetCurrentCountChange() const { return currentCountChange_; }

    InstanceHandle GetLastPublicationHandle() const
    {
        return lastPublicationHandle_;
    }

public:
    // ------------------------------------------------
    // Setters
    // ------------------------------------------------
    void SetTotalCount(int32_t totalCount) 					{ totalCount_ = totalCount; }
    void SetTotalCountChange(int32_t totalCountChange) 		{ totalCountChange_ = totalCountChange; }
    void SetCurrentCount(int32_t currentCount) 				{ currentCount_ = currentCount; }
    void SetCurrentCountChange(int32_t currentCountChange) 	{ currentCountChange_ = currentCountChange; }

    void SetLastPublicationHandle(const InstanceHandle &lastPublicationHandle)
    {
        lastPublicationHandle_ = lastPublicationHandle;
    }

public:
    // ------------------------------------------------
    //  Convenience functions
    // ------------------------------------------------
    void IncTotalCount()            { totalCount_++; }
    void IncTotalCountChange()      { totalCountChange_++; }
    void IncCurrentCount()          { currentCount_++; }
    void IncCurrentCountChange()    { currentCountChange_++; }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const SubscriptionMatchedStatusImpl &other) const
    {
        return (totalCount_ == other.totalCount_ &&
                totalCountChange_ == other.totalCountChange_ &&
                currentCount_ == other.currentCount_ &&
                currentCountChange_ == other.currentCountChange_ &&
                lastPublicationHandle_ == other.lastPublicationHandle_);
    }

    bool operator!=(const SubscriptionMatchedStatusImpl &other) const
    {
        return !(other == *this);
    }

    bool operator<(const SubscriptionMatchedStatusImpl &other) const
    {
        return (totalCount_ < other.totalCount_);
    }

    bool operator<=(const SubscriptionMatchedStatusImpl &other) const
    {
        return (totalCount_ <= other.totalCount_);
    }

    bool operator>(const SubscriptionMatchedStatusImpl &other) const
    {
        return (totalCount_ > other.totalCount_);
    }

    bool operator>=(const SubscriptionMatchedStatusImpl &other) const
    {
        return (totalCount_ >= other.totalCount_);
    }

protected:
    int32_t totalCount_;
    int32_t totalCountChange_;
    int32_t currentCount_;
    int32_t currentCountChange_;

    InstanceHandle lastPublicationHandle_;
};

}}}

#endif

