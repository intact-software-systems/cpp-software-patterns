/*
 * RtpsHeartbeatFrag.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsHeartbeatFrag.h"

namespace RTPS
{
/*----------------------------------------------------------------------------------------
The HeartbeatFrag message serves the same purpose as a regular Heartbeat message, but instead of indicating the
availability of a range of sequence numbers, it indicates the availability of a range of fragments for the data change with
sequence number WriterSN.

The RTPS Reader will respond by sending a NackFrag message, but only if it is missing any of the available fragments.

The Writer is identified uniquely by its GUID. The Writer GUID is obtained using the state of the Receiver:

writerGUID = { Receiver.sourceGuidPrefix, Heartbeat.writerId }

The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver:

readerGUID = { Receiver.destGuidPrefix, Heartbeat.readerId }

The HeartbeatFrag.readerId can be ENTITYID_UNKNOWN, in which case the HeartbeatFrag applies to all Readers
of that Writer GUID within the Participant.
----------------------------------------------------------------------------------------*/
RtpsHeartbeatFrag::RtpsHeartbeatFrag(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::HEARTBEAT_FRAG);
}

RtpsHeartbeatFrag::RtpsHeartbeatFrag()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::HEARTBEAT_FRAG);
}

RtpsHeartbeatFrag::~RtpsHeartbeatFrag()
{
}

SubmessageKind::Type RtpsHeartbeatFrag::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::HEARTBEAT_FRAG);
	return header_.GetSubmessageKind();
}

int64 RtpsHeartbeatFrag::GetSubmessageSize() const
{
	return RtpsHeartbeatFrag::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small
 writerSN.value is zero or negative
 lastFragmentNum.value is zero or negative
--------------------------------------------------*/
bool RtpsHeartbeatFrag::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS

