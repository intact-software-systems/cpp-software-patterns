/*
 * ChangeKind.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_ChangeKind_h_IsIncluded
#define RTPS_Elements_ChangeKind_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"

#include"RTPS/Export.h"
namespace RTPS
{

namespace ChangeKindId
{
enum Type
{
	ALIVE = 1,
	NOT_ALIVE_DISPOSED = 2,
	NOT_ALIVE_UNREGISTERED = 3
};
}

/*--------------------------------------------------------------------------------------------
	Mapping of the type ChangeKind_t

struct ChangeKind_t
{
	long value;
};

typedef ChangeKind_t ChangeKind;

Mapping of the reserved values:

#define ChangeKind_ALIVE 1
#define NOT_ALIVE_DISPOSED 2
#define NOT_ALIVE_UNREGISTERED 3
---------------------------------------------------------------------------------------------*/

class DLL_STATE ChangeKind : public NetworkLib::NetObjectBase
{
public:
	ChangeKind(ChangeKindId::Type val)
		: value_(val)
	{}
	ChangeKind()
		: value_(ChangeKindId::ALIVE)
	{}
	virtual ~ChangeKind()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt32((long)value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		value_ = (ChangeKindId::Type) reader->ReadInt32();
	}

public:
	static ChangeKind ALIVE()
	{
		static ChangeKind changeKind(ChangeKindId::ALIVE);
		return changeKind;
	}

	static ChangeKind NOT_ALIVE_DISPOSED()
	{
		static ChangeKind changeKind(ChangeKindId::NOT_ALIVE_DISPOSED);
		return changeKind;
	}

	static ChangeKind NOT_ALIVE_UNREGISTERED()
	{
		static ChangeKind changeKind(ChangeKindId::NOT_ALIVE_UNREGISTERED);
		return changeKind;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 4;
		return sz;
	}

public:
	bool operator==(const ChangeKind &other) const
	{
		return (value_ == other.value_);
	}

	bool operator!=(const ChangeKind &other) const
	{
		return !(value_ == other.value_);
	}

public:
	void SetChangeKind(ChangeKindId::Type c)	{ value_ = c; }

	ChangeKindId::Type GetChangeKind()	const { return value_; }

private:
	ChangeKindId::Type value_;
};

} // namespace RTPS

#endif // RTPS_Elements_ChangeKind_h_IsIncluded
