/*
 * RtpsPad.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsPad.h"

namespace RTPS
{
/*--------------------------------------------------
	constructor/destructor
--------------------------------------------------*/
RtpsPad::RtpsPad(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
	, submessageSize_(header.GetSubmessageLength())
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::PAD);
}

RtpsPad::RtpsPad(unsigned int submessageSize)
	: RtpsSubmessageBase()
	, submessageSize_(submessageSize)
{
	header_.SetSubmessageKind(SubmessageKind::PAD);
}

RtpsPad::~RtpsPad()
{
}

/*--------------------------------------------------
		virtual re-implementations
--------------------------------------------------*/
SubmessageKind::Type RtpsPad::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::PAD);
	return header_.GetSubmessageKind();
}

int64 RtpsPad::GetSubmessageSize() const
{
	ASSERT(submessageSize_ != 0);
	return submessageSize_;
}

/*--------------------------------------------------
This Submessage is always valid.
--------------------------------------------------*/
bool RtpsPad::IsValid() const
{
	return true;
}

} // namespace RTPS

