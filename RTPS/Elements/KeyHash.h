/*
 * KeyHash.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_KeyHash_h_IsIncluded
#define RTPS_Elements_KeyHash_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type KeyHash_t

struct KeyHash_t
{
	octet[16] value;
};

typedef KeyHash_t KeyHash;
---------------------------------------------------------------------------------------------*/
class DLL_STATE KeyHash : public NetworkLib::NetObjectBase
{
public:
	KeyHash(const std::vector<octet> &value)
		: value_(value)
	{
		if(value_.size() != KeyHash::SIZE())
			throw CriticalException("Wrong KeyHash size given. Should be 16!");
	}
	KeyHash()
		: value_()
	{ }
	virtual ~KeyHash()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		if(value_.empty()) return;

		// -- debug --
		ASSERT(value_.size() == KeyHash::SIZE());
		// -- debug --

		for(size_t i = 0; i < KeyHash::SIZE(); i++)
			writer->WriteInt8(value_[i]);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		for(size_t i = 0; i < KeyHash::SIZE(); i++)
			value_[i] = reader->ReadInt8();

		// -- debug --
		ASSERT(value_.size() == KeyHash::SIZE());
		// -- debug --
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int sz = 16;
		return sz;
	}

public:
	bool operator==(const KeyHash &other) const
	{
		return (value_ == other.value_);
	}

	bool operator!=(const KeyHash &other) const
	{
		return !(value_ == other.value_);
	}

public:
	void SetKeyHash(const std::vector<octet> &value)
	{
		value_ = value;

		// -- debug --
		ASSERT(value_.size() == KeyHash::SIZE());
		// -- debug --
	}

	const std::vector<octet>&	GetKeyHash()	const	{ return value_; }

private:
	std::vector<octet> value_;
};

} // namespace RTPS

#endif // RTPS_Elements_KeyHash_h_IsIncluded
