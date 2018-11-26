/*
 * RtpsInfoDestination.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsInfoDestination.h"

namespace RTPS
{
/*--------------------------------------------------
	constructor/destructor
--------------------------------------------------*/
RtpsInfoDestination::RtpsInfoDestination(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_DST);
}

RtpsInfoDestination::RtpsInfoDestination()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::INFO_DST);
}


RtpsInfoDestination::~RtpsInfoDestination()
{
}

SubmessageKind::Type RtpsInfoDestination::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_DST);
	return header_.GetSubmessageKind();
}

int64 RtpsInfoDestination::GetSubmessageSize() const
{
	return RtpsInfoDestination::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small.
--------------------------------------------------*/
bool RtpsInfoDestination::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS

