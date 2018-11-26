/*
 * ParameterId.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_ParameterId_h_IsIncluded
#define RTPS_Elements_ParameterId_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/CommonDefines.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type ParameterId_t

struct ParameterId_t
{
	short value;
};

//typedef ParameterId_t ParameterId;
---------------------------------------------------------------------------------------------*/
class DLL_STATE ParameterId : public NetworkLib::NetObjectBase
{
public:
	ParameterId(const short &val)
		: value_(val)
	{}
	ParameterId()
		: value_(PID_PAD)
	{}
	virtual ~ParameterId()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt16(value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		value_ = reader->ReadInt16();
	}

public:
	bool operator==(const ParameterId &other) const
	{
		return (value_ == other.value_);
	}

	bool operator!=(const ParameterId &other) const
	{
		return (value_ != other.value_);
	}

public:
	void SetParameterId(short val)	{ value_ = val; }
	short GetParameterId()		const { return value_; }

public:
	static unsigned int SIZE()
	{
		static unsigned int sz = 2;
		return sz;
	}

public:
	static ParameterId SENTINEL()
	{
		static ParameterId pid(PID_SENTINEL);
		return pid;
	}

	static ParameterId USER_DATA()
	{
		static ParameterId pid(PID_USER_DATA);
		return pid;
	}

	static ParameterId TOPIC_NAME()
	{
		static ParameterId pid(PID_TOPIC_NAME);
		return pid;
	}

	static ParameterId TYPENAME()
	{
		static ParameterId pid(PID_TYPE_NAME);
		return pid;
	}

	static ParameterId GROUP_DATA()
	{
		static ParameterId pid(PID_GROUP_DATA);
		return pid;
	}

	static ParameterId TOPIC_DATA()
	{
		static ParameterId pid(PID_TOPIC_DATA);
		return pid;
	}

	static ParameterId DURABILITY()
	{
		static ParameterId pid(PID_DURABILITY);
		return pid;
	}

	static ParameterId DURABILITY_SERVICE()
	{
		static ParameterId pid(PID_DURABILITY_SERVICE);
		return pid;
	}

	static ParameterId DEADLINE()
	{
		static ParameterId pid(PID_DEADLINE);
		return pid;
	}

	static ParameterId LATENCY_BUDGET()
	{
		static ParameterId pid(PID_LATENCY_BUDGET);
		return pid;
	}

	static ParameterId LIVELINESS()
	{
		static ParameterId pid(PID_LIVELINESS);
		return pid;
	}

	static ParameterId RELIABILITY()
	{
		static ParameterId pid(PID_RELIABILITY);
		return pid;
	}

	static ParameterId LIFESPAN()
	{
		static ParameterId pid(PID_LIFESPAN);
		return pid;
	}

	static ParameterId DESTINATION_ORDER()
	{
		static ParameterId pid(PID_DESTINATION_ORDER);
		return pid;
	}

	static ParameterId HISTORY()
	{
		static ParameterId pid(PID_HISTORY);
		return pid;
	}

	static ParameterId RESOURCE_LIMITS()
	{
		static ParameterId pid(PID_RESOURCE_LIMITS);
		return pid;
	}

	static ParameterId OWNERSHIP()
	{
		static ParameterId pid(PID_OWNERSHIP);
		return pid;
	}

	static ParameterId OWNERSHIP_STRENGTH()
	{
		static ParameterId pid(PID_OWNERSHIP_STRENGTH);
		return pid;
	}

	static ParameterId PRESENTATION()
	{
		static ParameterId pid(PID_PRESENTATION);
		return pid;
	}

	static ParameterId PARTITION()
	{
		static ParameterId pid(PID_PARTITION);
		return pid;
	}

	static ParameterId TIME_BASED_FILTER()
	{
		static ParameterId pid(PID_TIME_BASED_FILTER);
		return pid;
	}

	static ParameterId TRANSPORT_PRIORITY()
	{
		static ParameterId pid(PID_TRANSPORT_PRIORITY);
		return pid;
	}

	static ParameterId PROTOCOL_VERSION()
	{
		static ParameterId pid(PID_PROTOCOL_VERSION);
		return pid;
	}

	static ParameterId VENDORID()
	{
		static ParameterId pid(PID_VENDORID);
		return pid;
	}

	static ParameterId UNICAST_LOCATOR()
	{
		static ParameterId pid(PID_UNICAST_LOCATOR);
		return pid;
	}

	static ParameterId MULTICAST_LOCATOR()
	{
		static ParameterId pid(PID_MULTICAST_LOCATOR);
		return pid;
	}

	static ParameterId MULTICAST_IPADDRESS()
	{
		static ParameterId pid(PID_MULTICAST_IPADDRESS);
		return pid;
	}

	static ParameterId DEFAULT_UNICAST_LOCATOR()
	{
		static ParameterId pid(PID_DEFAULT_UNICAST_LOCATOR);
		return pid;
	}

	static ParameterId DEFAULT_MULTICAST_LOCATOR()
	{
		static ParameterId pid(PID_DEFAULT_MULTICAST_LOCATOR);
		return pid;
	}

	static ParameterId METATRAFFIC_UNICAST_LOCATOR()
	{
		static ParameterId pid(PID_METATRAFFIC_UNICAST_LOCATOR);
		return pid;
	}

	static ParameterId METATRAFFIC_MULTICAST_LOCATOR()
	{
		static ParameterId pid(PID_METATRAFFIC_MULTICAST_LOCATOR);
		return pid;
	}

	static ParameterId DEFAULT_UNICAST_IPADDRESS()
	{
		static ParameterId pid(PID_DEFAULT_UNICAST_IPADDRESS);
		return pid;
	}

	static ParameterId DEFAULT_UNICAST_PORT()
	{
		static ParameterId pid(PID_DEFAULT_UNICAST_PORT);
		return pid;
	}

	static ParameterId METATRAFFIC_UNICAST_IPADDRESS()
	{
		static ParameterId pid(PID_METATRAFFIC_UNICAST_IPADDRESS);
		return pid;
	}

	static ParameterId METATRAFFIC_UNICAST_PORT()
	{
		static ParameterId pid(PID_METATRAFFIC_UNICAST_PORT);
		return pid;
	}

	static ParameterId METATRAFFIC_MULTICAST_IPADDRESS()
	{
		static ParameterId pid(PID_METATRAFFIC_MULTICAST_IPADDRESS);
		return pid;
	}

	static ParameterId METATRAFFIC_MULTICAST_PORT()
	{
		static ParameterId pid(PID_METATRAFFIC_MULTICAST_PORT);
		return pid;
	}

	static ParameterId EXPECTS_INLINE_QOS()
	{
		static ParameterId pid(PID_EXPECTS_INLINE_QOS);
		return pid;
	}

	static ParameterId PARTICIPANT_MANUAL_LIVELINESS_COUNT()
	{
		static ParameterId pid(PID_PARTICIPANT_MANUAL_LIVELINESS_COUNT);
		return pid;
	}

	static ParameterId PARTICIPANT_BUILTIN_ENDPOINTS()
	{
		static ParameterId pid(PID_PARTICIPANT_BUILTIN_ENDPOINTS);
		return pid;
	}

	static ParameterId PARTICIPANT_LEASE_DURATION()
	{
		static ParameterId pid(PID_PARTICIPANT_LEASE_DURATION);
		return pid;
	}

	static ParameterId CONTENT_FILTER_PROPERTY()
	{
		static ParameterId pid(PID_CONTENT_FILTER_PROPERTY);
		return pid;
	}

	static ParameterId PARTICIPANT_GUID()
	{
		static ParameterId pid(PID_PARTICIPANT_GUID);
		return pid;
	}

	static ParameterId PARTICIPANT_ENTITYID()
	{
		static ParameterId pid(PID_PARTICIPANT_ENTITYID);
		return pid;
	}

	static ParameterId GROUP_GUID()
	{
		static ParameterId pid(PID_GROUP_GUID);
		return pid;
	}

	static ParameterId GROUP_ENTITYID()
	{
		static ParameterId pid(PID_GROUP_ENTITYID);
		return pid;
	}

	static ParameterId BUILTIN_ENDPOINT_SET()
	{
		static ParameterId pid(PID_BUILTIN_ENDPOINT_SET);
		return pid;
	}

	static ParameterId PROPERTY_LIST()
	{
		static ParameterId pid(PID_PROPERTY_LIST);
		return pid;
	}

	static ParameterId TYPE_MAX_SIZE_SERIALIZED()
	{
		static ParameterId pid(PID_TYPE_MAX_SIZE_SERIALIZED);
		return pid;
	}

	static ParameterId ENTITY_NAME()
	{
		static ParameterId pid(PID_ENTITY_NAME);
		return pid;
	}

	static ParameterId CONTENT_FILTER_INFO()
	{
		static ParameterId pid(PID_CONTENT_FILTER_INFO);
		return pid;
	}

	static ParameterId COHERENT_SET()
	{
		static ParameterId pid(PID_COHERENT_SET);
		return pid;
	}

	static ParameterId DIRECTED_WRITE()
	{
		static ParameterId pid(PID_DIRECTED_WRITE);
		return pid;
	}

	static ParameterId ORIGINAL_WRITER_INFO()
	{
		static ParameterId pid(PID_ORIGINAL_WRITER_INFO);
		return pid;
	}

	static ParameterId KEY_HASH()
	{
		static ParameterId pid(PID_KEY_HASH);
		return pid;
	}

	static ParameterId STATUS_INFO()
	{
		static ParameterId pid(PID_STATUS_INFO);
		return pid;
	}

private:
	short value_;
};

} // namespace RTPS

#endif // RTPS_Elements_ParameterId_h_IsIncluded
