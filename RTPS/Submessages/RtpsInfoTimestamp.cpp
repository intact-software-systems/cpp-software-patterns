/*
 * RtpsInfoTimestamp.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsInfoTimestamp.h"

namespace RTPS
{
/*--------------------------------------------------
	constructor/destructor
--------------------------------------------------*/
RtpsInfoTimestamp::RtpsInfoTimestamp(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_TS);
}

RtpsInfoTimestamp::RtpsInfoTimestamp()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::INFO_TS);
}

RtpsInfoTimestamp::~RtpsInfoTimestamp()
{

}

SubmessageKind::Type RtpsInfoTimestamp::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_TS);
	return header_.GetSubmessageKind();
}

int64 RtpsInfoTimestamp::GetSubmessageSize() const
{
	return RtpsInfoTimestamp::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small.
--------------------------------------------------*/
bool RtpsInfoTimestamp::IsValid() const
{
	// TODO: Implement
	return true;
}

/* --------------------------------------------------
I=0 means the InfoTimestamp also includes a timestamp.
I=1 means subsequent Submessages should not be considered to have a valid timestamp.

The value of the InvalidateFlag can be obtained from the expression:

I = SubmessageHeader.flags & 0x02
----------------------------------------------------*/
bool RtpsInfoTimestamp::HasInvalidateFlag() const
{
	char flag = (header_.GetSubmessageFlags() & INFO_TIMESTAMP_INVALIDATE_FLAG);

	return (flag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ INFO_TIMESTAMP_INVALIDATE_FLAG;
set: 	flag = flag | INFO_TIMESTAMP_INVALIDATE_FLAG;
clear:	flag = flag & ~INFO_TIMESTAMP_INVALIDATE_FLAG;
----------------------------------------------------*/
void RtpsInfoTimestamp::SetInvalidateFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | INFO_TIMESTAMP_INVALIDATE_FLAG;	// set bit to 1
		ASSERT((flag & INFO_TIMESTAMP_INVALIDATE_FLAG) == '1');
	}
	else
	{
		flag = flag & ~INFO_TIMESTAMP_INVALIDATE_FLAG;	// clear bit to 0
		ASSERT((flag & INFO_TIMESTAMP_INVALIDATE_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

} // namespace RTPS

