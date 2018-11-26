/*
 * RtpsHeartbeat.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsHeartbeat.h"

namespace RTPS
{
/*----------------------------------------------------------------------------------------
The Heartbeat message serves two purposes:
- It informs the Reader of the sequence numbers that are available in the writer’s HistoryCache so that the Reader may
request (using an AckNack) any that it has missed.
- It requests the Reader to send an acknowledgement for the CacheChange changes that have been entered into the
reader's HistoryCache such that the Writer knows the state of the reader.
All Heartbeat messages serve the first purpose. That is, the Reader will always find out the state of the writer's
HistoryCache and may request what it has missed. Normally, the RTPS Reader would only send an AckNack message if
it is missing a CacheChange.

The Writer uses the FinalFlag to request the Reader to send an acknowledgment for the sequence numbers it has
received. If the Heartbeat has the FinalFlag set, then the Reader is not required to send an AckNack message back.
However, if the FinalFlag is not set, then the Reader must send an AckNack message indicating which CacheChange
changes it has received, even if the AckNack indicates it has received all CacheChange changes in the writer's
HistoryCache.

The Writer sets the LivelinessFlag to indicate that the DDS DataWriter associated with the RTPS Writer of the message
has manually asserted its liveliness using the appropriate DDS operation (see the DDS Specification). The RTPS Reader
should therefore renew the manual liveliness lease of the corresponding remote DDS DataWriter.

The Writer is identified uniquely by its GUID. The Writer GUID is obtained using the state of the Receiver:

writerGUID = { Receiver.sourceGuidPrefix, Heartbeat.writerId }

The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver:

readerGUID = { Receiver.destGuidPrefix, Heartbeat.readerId }

The Heartbeat.readerId can be ENTITYID_UNKNOWN, in which case the Heartbeat applies to all Readers of that
writerGUID within the Participant.
----------------------------------------------------------------------------------------*/

RtpsHeartbeat::RtpsHeartbeat(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::HEARTBEAT);
}

RtpsHeartbeat::RtpsHeartbeat()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::HEARTBEAT);
}

RtpsHeartbeat::~RtpsHeartbeat()
{
}

SubmessageKind::Type RtpsHeartbeat::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::HEARTBEAT);
	return header_.GetSubmessageKind();
}

int64 RtpsHeartbeat::GetSubmessageSize() const
{
	return RtpsHeartbeat::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
- submessageLength in the Submessage header is too small
- firstSN.value is zero or negative
- lastSN.value is zero or negative
- lastSN.value < firstSN.value
--------------------------------------------------*/
bool RtpsHeartbeat::IsValid() const
{
	// TODO: Implement
	return true;
}

/* --------------------------------------------------
F=1 means the Writer does not require a response from the Reader.
F=0 means the Reader must respond to the HeartBeat message.

The value of the FinalFlag can be obtained from the expression:

F = SubmessageHeader.flags & 0x02
----------------------------------------------------*/
bool RtpsHeartbeat::HasFinalFlag() const
{
	char flag = (header_.GetSubmessageFlags() & HEARTBEAT_FINAL_FLAG);

	return (flag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ HEARTBEAT_FINAL_FLAG;
set: 	flag = flag | HEARTBEAT_FINAL_FLAG;
clear:	flag = flag & ~HEARTBEAT_FINAL_FLAG;
----------------------------------------------------*/
void RtpsHeartbeat::SetFinalFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | HEARTBEAT_FINAL_FLAG;		// set FINAL bit to 1
		ASSERT((flag & HEARTBEAT_FINAL_FLAG) == '1');
	}
	else
	{
		flag = flag & ~HEARTBEAT_FINAL_FLAG;	// clear FINAL bit to 0
		ASSERT((flag & HEARTBEAT_FINAL_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

/* --------------------------------------------------
The LivelinessFlag is represented with the literal "L". L=1 means the DDS DataReader associated with the RTPS Reader
should refresh the "manual" liveliness of the DDS DataWriter associated with the RTPS Writer of the message.
The value of the LivelinessFlag can be obtained from the expression:

L = SubmessageHeader.flags & 0x04
----------------------------------------------------*/
bool RtpsHeartbeat::HasLivelinessFlag() const
{
	char flag = (header_.GetSubmessageFlags() & HEARTBEAT_LIVELINESS_FLAG);

	return (flag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ HEARTBEAT_LIVELINESS_FLAG;
set: 	flag = flag | HEARTBEAT_LIVELINESS_FLAG;
clear:	flag = flag & ~HEARTBEAT_LIVELINESS_FLAG;
----------------------------------------------------*/
void RtpsHeartbeat::SetLivelinessFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | HEARTBEAT_LIVELINESS_FLAG;	// set bit to 1
		ASSERT((flag & HEARTBEAT_LIVELINESS_FLAG) == '1');
	}
	else
	{
		flag = flag & ~HEARTBEAT_LIVELINESS_FLAG;	// clear bit to 0
		ASSERT((flag & HEARTBEAT_LIVELINESS_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

} // namespace RTPS

