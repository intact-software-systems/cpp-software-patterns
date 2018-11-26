/*
 * RtpsWriter.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#include "RTPS/Structure/RtpsWriter.h"

namespace RTPS
{
/*-------------------------------------------------------------------------
RTPS Writer specializes RTPS Endpoint and represents the actor that sends
CacheChange messages to the matched RTPS Reader endpoints. The Reference
Implementations StatelessWriter and StatefulWriter specialize RTPS Writer and
differ in the knowledge they maintain about the matched Reader endpoints.

TODO: Place the hardcoded numbers somewhere else, should be configurable.
-------------------------------------------------------------------------*/
RtpsWriter::RtpsWriter()
	: RtpsEndpoint()
    , newChangeGuard_(new GuardCondition())
	, heartBeatPeriod_(Duration::FromSeconds(5))
	, nackResponseDelay_(200 * 1000 * 1000)
	, nackSuppressionDuration_(0)
	, lastChangeSequenceNumber_(0)
	, pushMode_(true)
{

}

RtpsWriter::RtpsWriter(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind)
	: RtpsEndpoint(guid, topicKind, reliabilityKind)
    , newChangeGuard_(new GuardCondition())
    , heartBeatPeriod_(Duration::FromSeconds(5))
	, nackResponseDelay_(200 * 1000 * 1000)
	, nackSuppressionDuration_(0)
	, lastChangeSequenceNumber_(0)
	, pushMode_(true)
{

}

RtpsWriter::~RtpsWriter()
{

}

/*-------------------------------------------------------------------------
initializes message sender - called from virtual machine
-------------------------------------------------------------------------*/
void RtpsWriter::SetMessageSender(const RtpsMessageSender::Ptr &messageSender)
{
	messageSender_ = messageSender;
}

/*-------------------------------------------------------------
create new CacheChange or replace old CacheChange with new data
	++this.lastChangeSequenceNumber;
	a_change := new CacheChange(kind, this.guid, this.lastChangeSequenceNumber,data, handle);
	RETURN a_change;

TODO: Change RtpsData to SerializedPayload::Ptr?
-------------------------------------------------------------*/
CacheChange::Ptr RtpsWriter::NewChange(ChangeKindId::Type kind, const RtpsData::Ptr &data, const InstanceHandle &handle)
{
	++lastChangeSequenceNumber_;

	// ------------------------------------------
	// TODO: Initialize RtpsData more?
	// ------------------------------------------
	if(data)
	{
		data->SetWriterSN(lastChangeSequenceNumber_);
		data->SetWriterId(this->GetGUID().GetEntityId());
		data->SetSrcGuidPrefix(this->GetGUID().GetGuidPrefix());
	}

	// ------------------------------------------
	// Create CacheChange
	// ------------------------------------------
    CacheChange::Ptr aChange(new CacheChange(kind, this->GetGUID(), lastChangeSequenceNumber_, data, handle));

	// -- debug --
	ASSERT(aChange->IsValid());
	// -- debug --

	writerCache_.AddChange(aChange);

	// ------------------------------------------
	// Triggers a call-back to the data-writer
	// ------------------------------------------
	this->newChangeGuard_->SetTriggerValue(true);

	return aChange;
}

} // namespace RTPS
