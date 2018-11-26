/*
 * RtpsGap.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsGap.h"

namespace RTPS
{

/*-------------------------------------------------------------------------------------------------
The RTPS Writer sends the Gap message to the RTPS Reader to communicate that certain sequence numbers are no
longer relevant. This is typically caused by Writer-side filtering of the sample (content-filtered topics, time-based
filtering). In this scenario, new data-values may replace the old values of the data-objects that were represented by the
sequence numbers that appear as irrelevant in the Gap.

The irrelevant sequence numbers communicated by the Gap message are composed of two groups:
 All sequence numbers in the range gapStart <= sequence_number <= gapList.base -1
 All the sequence numbers that appear explicitly listed in the gapList.
This set will be referred to as the Gap::irrelevant_sequence_number_list.

The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver:

writerGUID = { Receiver.sourceGuidPrefix, Gap.writerId }

The Reader is uniquely identified by its GUID. The Reader GUID is obtained using the state of the Receiver:

readerGUID = { Receiver.destGuidPrefix, Gap.readerId }
-------------------------------------------------------------------------------------------------*/

RtpsGap::RtpsGap(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::GAP);
}

RtpsGap::RtpsGap()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::GAP);
}

RtpsGap::~RtpsGap()
{

}

SubmessageKind::Type RtpsGap::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::GAP);
	return header_.GetSubmessageKind();
}

int64 RtpsGap::GetSubmessageSize() const
{
	return RtpsGap::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small.
 gapStart is zero or negative.
 gapList is invalid (as defined in Section 8.3.5.5).
--------------------------------------------------*/
bool RtpsGap::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS

