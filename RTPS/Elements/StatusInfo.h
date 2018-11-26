/*
 * StatusInfo.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_StatusInfo_h_IsIncluded
#define RTPS_Elements_StatusInfo_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type StatusInfo_t

struct StatusInfo_t
{
	octet[4] value;
};

typedef StatusInfo_t StatusInfo;
---------------------------------------------------------------------------------------------*/
class DLL_STATE StatusInfo : public NetworkLib::NetObjectBase
{
public:
	StatusInfo(const std::vector<octet> &value)
		: value_(value)
	{
        // TODO: Use static asserts!
		if(value_.size() != StatusInfo::SIZE())
			throw CriticalException("StatusInfo vector<octet> has wrong size! Must be made of 4 octets/chars!");

		ASSERT(value_.size() == StatusInfo::SIZE());

	}
	StatusInfo()
		: value_(4,0x00)
	{ }
	virtual ~StatusInfo()
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		// -- debug --
		ASSERT(value_.size() == StatusInfo::SIZE());
		// -- debug --

		for(size_t i = 0; i < StatusInfo::SIZE(); i++)
			writer->WriteInt8(value_[i]);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		for(size_t i = 0; i < StatusInfo::SIZE(); i++)
			value_[i] = reader->ReadInt8();

		// -- debug --
		ASSERT(value_.size() == StatusInfo::SIZE());
		// -- debug --
	}

public:
	inline bool HasDisposeFlag() const
	{
		char flag = (value_[0] & STATUS_INFO_DISPOSE_FLAG);

		return (flag == '1');
	}

	inline bool HasUnregisterFlag() const
	{
		char flag = (value_[0] & STATUS_INFO_UNREGISTER_FLAG);

		return (flag == '1');
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int sz = 4;
		return sz;
	}

public:
	bool operator==(const StatusInfo &other) const
	{
		return (value_ == other.value_);
	}

	bool operator!=(const StatusInfo &other) const
	{
		return !(value_ == other.value_);
	}

public:
	void SetStatusInfo(const std::vector<octet> &value)
	{
		value_ = value;

		// -- debug --
		ASSERT(value_.size() == StatusInfo::SIZE());
		// -- debug --
	}

	std::vector<octet>	GetStatusInfo()	const	{ return value_; }

private:
	std::vector<octet> value_;
};

} // namespace RTPS

#endif // RTPS_Elements_StatusInfo_h_IsIncluded
