/*
 * DataReaderListener.h
 *
 *  Created on: 25. september 2012
 *      Author: KVik
 */

#ifndef DCPS_Subscriber_DataReaderListener_h_IsIncluded
#define DCPS_Subscriber_DataReaderListener_h_IsIncluded

#include"DCPS/Infrastructure/Listener.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Sub/SubFwd.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief
 *
 * Since a DataReader is a kind of Entity, it has the ability to have a listener associated with it. In this case,
 * the associated listener should be of concrete type DataReaderListener.
 *
 * The use of this listener and its relationship to changes in the communication status of the DataReader is
 * described in Section 7.1.4, Listeners, Conditions, and Wait-sets, on page 120.
 */
template <typename DATA>
class DLL_STATE DataReaderListener : public DCPS::Infrastructure::Listener
{
public:
	virtual ~DataReaderListener() { }

	CLASS_TRAITS(DataReaderListener)

	virtual void OnRequestedDeadlineMissed(typename DCPS::Subscription::DataReader<DATA> &the_reader, const Status::RequestedDeadlineMissedStatus& status) = 0;
	virtual void OnRequestedIncompatibleQos(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::RequestedIncompatibleQosStatus& status) = 0;
	virtual void OnSampleRejected(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::SampleRejectedStatus& status) = 0;
	virtual void OnLivelinessChanged(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::LivelinessChangedStatus& status) = 0;
	virtual void OnDataAvailable(typename DCPS::Subscription::DataReader<DATA>& the_reader) = 0;
	virtual void OnSubscriptionMatched(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::SubscriptionMatchedStatus& status) = 0;
	virtual void OnSampleLost(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::SampleLostStatus& status) = 0;
};

template <typename DATA>
class DLL_STATE NoOpDataReaderListener : public DataReaderListener<DATA>
{
public:
	virtual ~NoOpDataReaderListener() { }

	CLASS_TRAITS(NoOpDataReaderListener)

	virtual void OnRequestedDeadlineMissed(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::RequestedDeadlineMissedStatus& status) { }
	virtual void OnRequestedIncompatibleQos(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::RequestedIncompatibleQosStatus& status) { }
	virtual void OnSampleRejected(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::SampleRejectedStatus& status) { }
	virtual void OnLivelinessChanged(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::LivelinessChangedStatus& status) { }
	virtual void OnDataAvailable(typename DCPS::Subscription::DataReader<DATA>& the_reader) { }
	virtual void OnSubscriptionMatched(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::SubscriptionMatchedStatus& status) { }
	virtual void OnSampleLost(typename DCPS::Subscription::DataReader<DATA>& the_reader, const Status::SampleLostStatus& status) { }
};

}} // namespace DCPS::Subscription

#endif // DCPS_Subscriber_DataReaderListener_h_IsIncluded

