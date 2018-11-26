/*
 * FragmentNumber.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_FragmentNumber_h_IsIncluded
#define RTPS_Elements_FragmentNumber_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*--------------------------------------------------------------------------------------------
	Mapping of the type FragmentNumber_t

struct FragmentNumber_t
{
	int64 value;
};

typedef FragmentNumber_t FragmentNumber;
---------------------------------------------------------------------------------------------*/
class DLL_STATE FragmentNumber : public NetworkLib::NetObjectBase
{
public:
	FragmentNumber(const int64 &val)
		: value_(val)
	{}
	FragmentNumber()
		: value_(-1)
	{}
	virtual ~FragmentNumber()
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt64(value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		value_ = reader->ReadInt64();
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int sz = 4;
		return sz;
	}

public:
	void SetFragmentNumber(int64 val)	{ value_ = val; }
	int64 GetFragmentNumber()	const { return value_; }

private:
	int64 value_;
};

} // namespace RTPS

#endif // RTPS_Elements_FragmentNumber_h_IsIncluded
