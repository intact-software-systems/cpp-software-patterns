/*
 * RtpsSubmessageBase.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsSubmessageBase.h"

namespace RTPS
{
// ----------------------------------------------------------
// constructors/destructor
// ----------------------------------------------------------
RtpsSubmessageBase::RtpsSubmessageBase(const SubmessageHeader &header)
	: header_(header)
{

}

RtpsSubmessageBase::RtpsSubmessageBase()
{

}

RtpsSubmessageBase::~RtpsSubmessageBase()
{

}

// ----------------------------------------------------------
// Convenience function to write SubmessageHeader and
// Submessage in one call
// ----------------------------------------------------------
bool RtpsSubmessageBase::WriteWithHeader(NetworkLib::SerializeWriter *writer)
{
	if(header_.IsValid() == false)
	{
		ICRITICAL() << "Header is invalid!";
		return false;
	}

	header_.SetSubmessageLength(GetSubmessageSize());

	// write header
	header_.Write(writer);

	// write content
	Write(writer);

	return true;
}

// ----------------------------------------------------------
// Convenience functions to check submessage header endianness
// ----------------------------------------------------------
bool RtpsSubmessageBase::IsBigEndian() const
{
	return header_.IsBigEndian();
}

bool RtpsSubmessageBase::IsLittleEndian() const
{
	return header_.IsLittleEndian();
}

} // namespace RTPS
