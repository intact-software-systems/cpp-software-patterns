/*
 * Count.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_Count_h_IsIncluded
#define RTPS_Elements_Count_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type Count_t

struct Count_t
{
	long value;
};

typedef Count_t Count;
---------------------------------------------------------------------------------------------*/
class DLL_STATE Count : public NetworkLib::NetObjectBase
{
public:
	Count(const long &val)
		: value_(val)
	{}
	Count()
		: value_(-1)
	{}
	virtual ~Count()
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt32(value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		value_ = reader->ReadInt32();
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int sz = 4;
		return sz;
	}

public:
	bool operator==(const Count &other) const
	{
		return (value_ == other.value_);
	}

	bool operator!=(const Count &other) const
	{
		return !(value_ == other.value_);
	}

public:
	void SetCount(long val)			{ value_ = val; }
	long GetCount()			const 	{ return value_; }

private:
	long value_;
};

} // namespace RTPS

#endif // RTPS_Elements_Count_h_IsIncluded
