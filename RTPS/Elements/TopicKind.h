/*
 * TopicKind.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_TopicKind_h_IsIncluded
#define RTPS_Elements_TopicKind_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
namespace TopicKindId
{
	enum Type
	{
		UNKNOWN = 0,
		NoKey = 1,
		WithKey = 2
	};
}

/*--------------------------------------------------------------------------------------------
	Mapping of the type TopicKind_t

struct TopicKind_t
{
	long value;
};

typedef TopicKind_t TopicKind;

Mapping of the reserved values:

#define TopicKind_NO_KEY 1
#define TopicKind_WITH_KEY 2
---------------------------------------------------------------------------------------------*/
class DLL_STATE TopicKind : public NetworkLib::NetObjectBase
{
public:
	TopicKind(TopicKindId::Type val)
		: value_(val)
	{}
	TopicKind()
		: value_(TopicKindId::NoKey)
	{}
	virtual ~TopicKind()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt32((long)value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		value_ = (TopicKindId::Type)reader->ReadInt32();
	}

public:
	static TopicKind WITH_KEY()
	{
		static TopicKind topicKind(TopicKindId::WithKey);
		return topicKind;
	}

	static TopicKind NO_KEY()
	{
		static TopicKind topicKind(TopicKindId::NoKey);
		return topicKind;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 4;
		return sz;
	}

public:
	bool operator==(const TopicKind &other) const
	{
		return (value_ == other.value_);
	}

	bool operator!=(const TopicKind &other) const
	{
		return !(value_ == other.value_);
	}

public:
	TopicKindId::Type GetTopicKind()	const { return value_; }

	void SetTopicKind(TopicKindId::Type id)		{ value_ = id; }

private:
	TopicKindId::Type value_;
};

} // namespace RTPS

#endif // RTPS_Elements_TopicKind_h_IsIncluded
