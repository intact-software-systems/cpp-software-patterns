/*
 * RtpsReader.h
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#ifndef RTPS_Structure_RtpsReader_h_IsIncluded
#define RTPS_Structure_RtpsReader_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Structure/RtpsEndpoint.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Cache/DataCache.h"
#include"RTPS/Machine/RtpsMessageSender.h"
#include"RTPS/Export.h"
namespace RTPS
{

class RtpsMessageReceiver;
class RtpsData;
class RtpsHeartbeat;
class RtpsGap;

/*-------------------------------------------------------------------------
RTPS Reader specializes RTPS Endpoint and represents the actor that receives CacheChange messages from one or
more RTPS Writer endpoints. The Reference Implementations StatelessReader and StatefulReader specialize RTPS
Reader and differ in the knowledge they maintain about the matched Writer endpoints.
-------------------------------------------------------------------------*/
class DLL_STATE RtpsReader : public RtpsEndpoint
{
public:
	RtpsReader();
	RtpsReader(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
	virtual ~RtpsReader();

public:
	virtual ReaderKind::Type	GetReaderKind()	const = 0;

	virtual bool ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsData *data) = 0;
	virtual bool ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsGap *gap) = 0;
	virtual bool ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsHeartbeat *heartbeat) = 0;

public:
	const DataCache& GetDataCache() 					const { return readerCache_; }

	Duration		GetHeartBeatResponseDelay()			const { return heartBeatResponseDelay_; }
	Duration		GetHeartBeatSuppressionDuration()	const { return heartBeatSuppressionDuration_; }
	bool			ExpectsInlineQos()					const { return expectsInlineQos_; }

	void			SetHeartBeatResponseDelay(const Duration &d)			{ heartBeatResponseDelay_ = d; }
	void			SetHeartBeatSuppressionDuration(const Duration &d)		{ heartBeatSuppressionDuration_ = d; }
	void			SetExpectsInlineQos(bool d)								{ expectsInlineQos_ = d; }

public:
	// ---------------------------------------------------
	// New functions not in RTPS specification
	// ---------------------------------------------------
	void SetMessageSender(const RtpsMessageSender::Ptr &messageSender);

protected:
	/* -------------------------------------------------
	 The following timing-related values are used as the
	 defaults in order to facilitate ‘out-of-the-box’
	 interoperability between implementations.

		heartbeatResponseDelay.sec = 0;
		heartbeatResponseDelay.nanosec = 500 * 1000 * 1000; // 500 milliseconds
		heartbeatSuppressionDuration.sec = 0;
		heartbeatSuppressionDuration.nanosec = 0;
	 --------------------------------------------------*/
	bool		expectsInlineQos_;				// in-line QoS sent along with data?
	Duration	heartBeatResponseDelay_;		// delay sending of a positive/negative ack
	Duration	heartBeatSuppressionDuration_;	// ignore HEARTBEATs that arrive too soon after previous HEARTBEAT
	DataCache	readerCache_;					// History of CacheChanges

protected:
	RtpsMessageSender::Ptr messageSender_;
};

} // namespace RTPS

#endif // RTPS_RtpsReader_h_IsIncluded
