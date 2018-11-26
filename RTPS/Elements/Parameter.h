/*
 * Parameter.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_Parameter_h_IsIncluded
#define RTPS_Elements_Parameter_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Elements/ParameterId.h"
#include"RTPS/Export.h"

namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type Parameter_t

struct Parameter_t
{
	ParameterId_t parameterId;
	short length;
	octet value[length];
};

typedef Parameter_t Parameter;
---------------------------------------------------------------------------------------------*/
class DLL_STATE Parameter : public NetworkLib::NetObjectBase
{
public:
	Parameter(const ParameterId &id, const std::vector<octet> &value)
		: parameterId_(id)
		, value_(value)
		{}
	Parameter()
		: parameterId_(-1)
		, value_()
		{}
	virtual ~Parameter()
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		parameterId_.Write(writer);
		writer->WriteInt16((short)value_.size());

		for(size_t i = 0; i < value_.size(); i++)
			writer->WriteInt8(value_[i]);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		parameterId_.Read(reader);
		short length = reader->ReadInt16();

		for(short i = 0; i < length; i++)
			value_[i] = reader->ReadInt8();
	}

public:
	bool operator==(const Parameter &other) const
	{
		return (parameterId_ == other.parameterId_) && (value_ == other.value_);
	}

	bool operator!=(const Parameter &other) const
	{
		return !((parameterId_ == other.parameterId_) && (value_ == other.value_));
	}

public:
	void SetParameterId(const ParameterId &id)		{ parameterId_ = id; }
	void SetValue(const std::vector<octet> &val)	{ value_ = val; }

	ParameterId 		GetParameterId() 	const { return parameterId_; }
	unsigned short		GetLength()			const { return (unsigned short)value_.size(); }
	std::vector<octet>	GetValue()			const { return value_; }
	unsigned short      GetSize()           const { return (ParameterId::SIZE() + 2 + GetLength()); }

public:
	static Parameter SENTINEL()
	{
		static Parameter padding(ParameterId::SENTINEL(), std::vector<octet>());
		return padding;
	}

private:
	ParameterId 		parameterId_;
	std::vector<octet> 	value_;
};

} // namespace RTPS

#endif // RTPS_Elements_Parameter_h_IsIncluded
