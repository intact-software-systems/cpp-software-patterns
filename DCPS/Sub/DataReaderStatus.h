#ifndef DCPS_Subscription_DataReaderStatus_h_IsIncluded
#define DCPS_Subscription_DataReaderStatus_h_IsIncluded

#include"DCPS/Status/IncludeLibs.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief The DataReaderStatus class is a container for all status objects relevant for a DataReader.
 */
class DataReaderStatus
{
public:

    // ---------------------------------------------------
    // Constructor
    // ---------------------------------------------------

    /**
     * @brief DataReaderStatus initial constructor.
     */
    DataReaderStatus();

    /**
     * Destructor
     */
    ~DataReaderStatus();

public:
    // ---------------------------------------------------
    // Getters
    // ---------------------------------------------------

    template <typename STATUS>
    const STATUS& status() const;

    template <typename STATUS>
    STATUS& status();


    // --------------------------------------------------------------
	//  Status Management
	// --------------------------------------------------------------
	const Status::LivelinessChangedStatus&          GetLivelinessChangedStatus() 		const { return livelinessStatus_; }
	const Status::SampleRejectedStatus& 			GetSampleRejectedStatus() 			const { return sampleRejectedStatus_;}
	const Status::SampleLostStatus&					GetSampleLostStatus() 				const { return sampleLostStatus_;}
	const Status::RequestedDeadlineMissedStatus&	GetRequestedDeadlineMissedStatus() 	const { return deadlineMissedStatus_; }
	const Status::RequestedIncompatibleQosStatus&	GetRequestedIncompatibleQosStatus() const { return incompatibleQosStatus_; }
	const Status::SubscriptionMatchedStatus&		GetSubscriptionMatchedStatus() 		const { return subscriptionMatchedStatus_;}

private:

    // ---------------------------------------------------
    // Attributes
    // ---------------------------------------------------

	Status::LivelinessChangedStatus			livelinessStatus_;
	Status::SampleRejectedStatus 			sampleRejectedStatus_;
	Status::SampleLostStatus				sampleLostStatus_;
	Status::RequestedDeadlineMissedStatus	deadlineMissedStatus_;
	Status::RequestedIncompatibleQosStatus	incompatibleQosStatus_;
	Status::SubscriptionMatchedStatus		subscriptionMatchedStatus_;
};

}}

#endif // DCPS_Subscription_DataReaderStatus_h_IsIncluded
