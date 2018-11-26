/*
 * ReliabilityKind.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_ReliabilityKind_h_IsIncluded
#define RTPS_Elements_ReliabilityKind_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

namespace ReliabilityKindId
{
	enum Type
	{
		UNKNOWN = 0,
		BestEffort = 1,
		Reliable = 3
	};
}

/*--------------------------------------------------------------------------------------------
	Mapping of the type ReliabilityKind_t

struct ReliabilityKind_t
{
	long value;
};

typedef ReliabilityKind_t ReliabilityKind;

Mapping of the reserved values:

#define ReliabilityKind_BEST_EFFORT 1
#define ReliabilityKind_RELIABLE 	3
---------------------------------------------------------------------------------------------*/
class DLL_STATE ReliabilityKind : public NetworkLib::NetObjectBase
{
public:
	ReliabilityKind(ReliabilityKindId::Type val)
		: value_(val)
	{}
	ReliabilityKind()
		: value_(ReliabilityKindId::Reliable)
	{}
	virtual ~ReliabilityKind()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt32((long)value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		value_ = (ReliabilityKindId::Type)reader->ReadInt32();
	}

public:
	static ReliabilityKind RELIABLE()
	{
		static ReliabilityKind rel(ReliabilityKindId::Reliable);
		return rel;
	}

	static ReliabilityKind BEST_EFFORT()
	{
		static ReliabilityKind rel(ReliabilityKindId::BestEffort);
		return rel;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 4;
		return sz;
	}

public:
	void SetReliabilityKind(ReliabilityKindId::Type r)	{ value_ = r;}

	ReliabilityKindId::Type GetReliabilityKind()	const { return value_; }

private:
	ReliabilityKindId::Type value_;
};

} // namespace RTPS

#endif // RTPS_Elements_ReliabilityKind_h_IsIncluded
