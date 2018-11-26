/*
 * RtpsWriter.h
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#ifndef RTPS_Structure_RtpsWriter_h_IsIncluded
#define RTPS_Structure_RtpsWriter_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Structure/RtpsEndpoint.h"
#include"RTPS/Cache/DataCache.h"
#include"RTPS/Machine/RtpsMessageSender.h"

#include"RTPS/Export.h"
namespace RTPS
{

class RtpsMessageReceiver;
class RtpsAckNack;
class RtpsNackFrag;

/*-------------------------------------------------------------------------
RTPS Writer specializes RTPS Endpoint and represents the actor that sends
CacheChange messages to the matched RTPS Reader endpoints. The Reference
Implementations StatelessWriter and StatefulWriter specialize RTPS Writer and
differ in the knowledge they maintain about the matched Reader endpoints.
-------------------------------------------------------------------------*/
class DLL_STATE RtpsWriter : public RtpsEndpoint
{
public:
    RtpsWriter();
    RtpsWriter(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
    virtual ~RtpsWriter();

    CLASS_TRAITS(RtpsWriter)

public:
    // ---------------------------------------------------
    // this automatically adds change to Cache
    // ---------------------------------------------------
    virtual CacheChange::Ptr NewChange(ChangeKindId::Type kind, const RtpsData::Ptr &data, const InstanceHandle &handle);

public:
    // ---------------------------------------------------
    // New virtual functions not in RTPS specification
    // Implement in specialized classes
    // ---------------------------------------------------
    virtual WriterKind::Type	GetWriterKind()	const = 0;

    virtual bool ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsAckNack *ackNack) = 0;
    virtual bool ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsNackFrag *nackFrag) = 0;

public:
    bool 			isPushMode()			const	{ return pushMode_; }
    void			SetPushMode(bool p)				{ pushMode_ = p; }

public:
    DataCache& 		GetDataCache() 				          { return writerCache_; }
    Duration		GetHeartBeatPeriod()			const { return heartBeatPeriod_; }
    Duration		GetNackResponseDelay()			const { return nackResponseDelay_; }
    Duration		GetNackSuppressionDuration()	const { return nackSuppressionDuration_; }
    SequenceNumber	GetLastChangeSequenceNumber()	const { return lastChangeSequenceNumber_; }

    void			SetHeartBeatPeriod(const Duration &d)                   { heartBeatPeriod_ = d; }
    void			SetNackResponseDelay(const Duration &d)                 { nackResponseDelay_ = d; }
    void			SetNackSuppressionDuration(const Duration &d)			{ nackSuppressionDuration_ = d; }
    void			SetLastChangeSequenceNumber(const SequenceNumber &s)	{ lastChangeSequenceNumber_ = s; }

    GuardCondition::Ptr GetNewChangeGuard()  { return newChangeGuard_; }

public:
    // ---------------------------------------------------
    // New functions not in RTPS specification
    // ---------------------------------------------------
    void SetMessageSender(const RtpsMessageSender::Ptr &messageSender);

protected:
    RtpsMessageSender::Ptr messageSender_;

protected:
    GuardCondition::Ptr	newChangeGuard_;

protected:
    /*-------------------------------------------------------------
    8.4.7.1.1 Default Timing-Related Values
    The following timing-related values are used as the defaults in
    order to facilitate ‘out-of-the-box’ interoperability between
    implementations.

    nackResponseDelay.sec = 0;
    nackResponseDelay.nanosec = 200 * 1000 * 1000; //200 milliseconds
    nackSuppressionDuration.sec = 0;
    nackSuppressionDuration.nanosec = 0;
    -------------------------------------------------------------*/
    Duration		heartBeatPeriod_;
    Duration		nackResponseDelay_;
    Duration		nackSuppressionDuration_;
    SequenceNumber	lastChangeSequenceNumber_;

protected:
    DataCache		writerCache_;

private:
    bool			pushMode_;

private:
    RtpsWriter(const RtpsWriter&) {}
    RtpsWriter& operator=(const RtpsWriter&) { return *this; }
};

} // namespace RTPS

#endif //RTPS_RtpsWriter_h_IsIncluded
