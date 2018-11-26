/*
 * CacheChange.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#include "RTPS/Cache/CacheChange.h"

namespace RTPS
{
/* --------------------------------------------------------
	constructor/destructor
----------------------------------------------------------*/
CacheChange::CacheChange()
	: CacheChangeBase()
{
}

CacheChange::CacheChange(ChangeKindId::Type changeKind, const GUID &guid, const SequenceNumber &seq, const RtpsData::Ptr &data, InstanceHandle handle)
	: CacheChangeBase()
	, changeKind_(changeKind)
	, guid_(guid)
	, sequenceNumber_(seq)
	, data_(data)
	, instanceHandle_(handle)
{
}

CacheChange::~CacheChange()
{
}

/* --------------------------------------------------------
	Check validity of cache-change object
----------------------------------------------------------*/
bool CacheChange::IsValid() const
{
	if(guid_ == GUID::GUID_UNKNOWN())
	{
		ICRITICAL() << "CacheChange with GUID_UNKNOWN";
		return false;
	}

	if(sequenceNumber_ == SequenceNumber::UNKNOWN())
	{
		ICRITICAL() << "SequenceNumber::UNKNOWN";
		return false;
	}

	// Data is allowed to be nil!
	if(data_)
	{
		if(data_->IsValid() == false)
		{
			ICRITICAL() << "data is not valid!";
			return false;
		}
	}

	return true;
}

/* --------------------------------------------------------
	Get functions
----------------------------------------------------------*/
const ChangeKindId::Type& CacheChange::GetChangeKind() const
{
	return changeKind_;
}

const GUID& CacheChange::GetGUID() const
{
	return guid_;
}

const SequenceNumber& CacheChange::GetSequenceNumber() const
{
	return sequenceNumber_;
}

const InstanceHandle& CacheChange::GetInstanceHandle() const
{
	return instanceHandle_;
}

RtpsData::Ptr CacheChange::GetData() const
{
	return data_;
}

/* --------------------------------------------------------
	Set functions
----------------------------------------------------------*/
void CacheChange::SetChangeKind(const ChangeKindId::Type &kind)
{
	changeKind_ = kind;
}

void CacheChange::SetGUID(const GUID &guid)
{
	guid_ = guid;
}

void CacheChange::SetSequenceNumber(const SequenceNumber &sequenceNumber)
{
	sequenceNumber_ = sequenceNumber;
}

void CacheChange::SetInstanceHandle(const InstanceHandle &handle)
{
	instanceHandle_ = handle;
}

void CacheChange::SetData(const RtpsData::Ptr &data)
{
	data_ = data;
}

} // namespace RTPS
