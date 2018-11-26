/*
 * RtpsNackFrag.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsNackFrag.h"

namespace RTPS
{
/*----------------------------------------------------------------------------------------
The Reader sends the NackFrag message to the Writer to request fragments from the Writer.
The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver:

writerGUID = { Receiver.destGuidPrefix, NackFrag.writerId }

The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver:

readerGUID = { Receiver.sourceGuidPrefix, NackFrag.readerId }

The sequence number from which fragments are requested is given by writerSN. The mechanism to explicitly represent
fragment numbers depends on the PSM. Typically, a compact representation (such as a bitmap) is used.
----------------------------------------------------------------------------------------*/
RtpsNackFrag::RtpsNackFrag(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::NACK_FRAG);
}

RtpsNackFrag::RtpsNackFrag()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::NACK_FRAG);
}

RtpsNackFrag::~RtpsNackFrag()
{
}

SubmessageKind::Type RtpsNackFrag::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::NACK_FRAG);
	return header_.GetSubmessageKind();
}

int64 RtpsNackFrag::GetSubmessageSize() const
{
	return RtpsNackFrag::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small.
 writerSN.value is zero or negative.
 fragmentNumberState is invalid (as defined in Section 8.3.5.7).
--------------------------------------------------*/
bool RtpsNackFrag::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS

