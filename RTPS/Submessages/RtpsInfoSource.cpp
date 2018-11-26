/*
 * RtpsInfoSource.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsInfoSource.h"

namespace RTPS
{
/*--------------------------------------------------
	constructor/destructor
--------------------------------------------------*/
RtpsInfoSource::RtpsInfoSource(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
	, unused_(0)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_SRC);
}

RtpsInfoSource::RtpsInfoSource()
	: RtpsSubmessageBase()
	, unused_(0)
{
	header_.SetSubmessageKind(SubmessageKind::INFO_SRC);
}

RtpsInfoSource::~RtpsInfoSource()
{

}

SubmessageKind::Type RtpsInfoSource::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::INFO_SRC);
	return header_.GetSubmessageKind();
}

int64 RtpsInfoSource::GetSubmessageSize() const
{
	return RtpsInfoSource::SIZE();
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small.
--------------------------------------------------*/
bool RtpsInfoSource::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS

