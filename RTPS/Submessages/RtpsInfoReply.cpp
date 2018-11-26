/*
 * RtpsInfoReply.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsInfoReply.h"

namespace RTPS
{
/*--------------------------------------------------
	constructor/destructor
--------------------------------------------------*/
RtpsInfoReply::RtpsInfoReply(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_REPLY);
}

RtpsInfoReply::RtpsInfoReply(const Locator &locator)
	: RtpsSubmessageBase()
	, unicastLocatorList_(locator)
{
	header_.SetSubmessageKind(SubmessageKind::INFO_REPLY);
}

RtpsInfoReply::RtpsInfoReply()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::INFO_REPLY);
}

RtpsInfoReply::~RtpsInfoReply()
{
}

/*--------------------------------------------------
	virtual re-implementations
--------------------------------------------------*/
SubmessageKind::Type RtpsInfoReply::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_REPLY);
	return header_.GetSubmessageKind();
}

int64 RtpsInfoReply::GetSubmessageSize() const
{
	return (this->unicastLocatorList_.GetSize() + this->multicastLocatorList_.GetSize());
}

/* -------------------------------------------------------------
 In addition to the EndiannessFlag, The InfoReply Submessage introduces the MulticastFlag (“Content” on page 46).
 The PSM maps the MulticastFlag flag into the 2nd least-significant bit (LSB) of the flags.
 The MulticastFlag is represented with the literal ‘M’. M=1 means the InfoReply also includes a multicastLocatorList.
 The value of the MulticastFlag can be obtained from the expression:

 M = SubmessageHeader.flags & 0x02
 -------------------------------------------------------------*/
bool RtpsInfoReply::HasMulticastFlag() const
{
	char flag = (header_.GetSubmessageFlags() & 0x02);

	return (flag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ DATA_INLINE_QOS_FLAG;
set: 	flag = flag | DATA_INLINE_QOS_FLAG;
clear:	flag = flag & ~DATA_INLINE_QOS_FLAG;
----------------------------------------------------*/
void RtpsInfoReply::SetMulticastFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | INFO_REPLY_MULTICAST_FLAG;		// set bit to 1
		ASSERT((flag & INFO_REPLY_MULTICAST_FLAG) == '1');
	}
	else
	{
		flag = flag & ~INFO_REPLY_MULTICAST_FLAG;	// clear bit to 0
		ASSERT((flag & INFO_REPLY_MULTICAST_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
submessageLength in the Submessage header is too small.
--------------------------------------------------*/
bool RtpsInfoReply::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS

