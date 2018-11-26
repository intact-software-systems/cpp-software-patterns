/*
 * RtpsAckNack.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsAckNack.h"

namespace RTPS
{
/* ---------------------------------------------------------
	constructor/destructor
---------------------------------------------------------*/
RtpsAckNack::RtpsAckNack(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::ACKNACK);
}

RtpsAckNack::RtpsAckNack()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::ACKNACK);
}

RtpsAckNack::~RtpsAckNack()
{

}

/* ---------------------------------------------------------
	pure virtual functions reimplemented
---------------------------------------------------------*/
SubmessageKind::Type RtpsAckNack::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::ACKNACK);
	return header_.GetSubmessageKind();
}

int64 RtpsAckNack::GetSubmessageSize() const
{
	return RtpsAckNack::SIZE();
}

/* ---------------------------------------------------------
This Submessage is invalid when any of the following is true:
• submessageLength in the Submessage header is too small.
• readerSNState is invalid (as defined in Section 8.3.5.5).
 ---------------------------------------------------------*/
bool RtpsAckNack::IsValid() const
{
	// TODO: Implement
	return true;
}

/* --------------------------------------------------
 The FinalFlag is represented with the literal ‘F’. F=1 means the reader does not require a
 response from the writer. F=0 means the writer must respond to the AckNack message.

The value of the FinalFlag can be obtained from the expression:
F = SubmessageHeader.flags & 0x02
----------------------------------------------------*/
bool RtpsAckNack::HasFinalFlag() const
{
	char flag = (header_.GetSubmessageFlags() & ACKNACK_FINAL_FLAG);

	return (flag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ ACKNACK_FINAL_FLAG;
set: 	flag = flag | ACKNACK_FINAL_FLAG;
clear:	flag = flag & ~ACKNACK_FINAL_FLAG;
----------------------------------------------------*/
void RtpsAckNack::SetFinalFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | ACKNACK_FINAL_FLAG;		// set FINAL bit to 1
		ASSERT((flag & ACKNACK_FINAL_FLAG) == '1');
	}
	else
	{
		flag = flag & ~ACKNACK_FINAL_FLAG;	// clear FINAL bit to 0
		ASSERT((flag & ACKNACK_FINAL_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

} // namespace RTPS
