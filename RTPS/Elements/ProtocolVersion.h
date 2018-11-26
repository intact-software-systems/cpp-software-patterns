/*
 * ProtocolVersion.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_ProtocolVersion_h_IsIncluded
#define RTPS_Elements_ProtocolVersion_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

//Mapping of the reserved values:
#define PROTOCOLVERSION_1_0 		{1,0}
#define PROTOCOLVERSION_1_1 		{1,1}
#define PROTOCOLVERSION_2_0 		{2,0}
#define PROTOCOLVERSION_2_1 		{2,1}
#define CURRENT_PROTOCOLVERSION 	PROTOCOLVERSION_2_1
//The Implementations following this version of the document implement protocol version 2.1 (major = 2, minor = 1).

/*--------------------------------------------------------------------------------------------
	Mapping of the type ProtocolVersion_t

typedef struct ProtocolVersion_t
{
	octet major;
	octet minor;
};

typedef ProtocolVersion_t ProtocolVersion;
---------------------------------------------------------------------------------------------*/
class DLL_STATE ProtocolVersion : public NetworkLib::NetObjectBase
{
public:
	ProtocolVersion(octet major, octet minor)
		: major_(major)
		, minor_(minor)
	{}
	ProtocolVersion()
		: major_('2')
		, minor_('1')
	{}
	virtual ~ProtocolVersion()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt8(major_);
		writer->WriteInt8(minor_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		major_ = reader->ReadInt8();
		minor_ = reader->ReadInt8();
	}

public:
	static ProtocolVersion PROTOCOLVERSION()
	{
		static ProtocolVersion pr('2','1');
		return pr;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 2;
		return sz;
	}

public:
	bool operator==(const ProtocolVersion &other) const
	{
		return (major_ == other.major_) && (minor_ == other.minor_);
	}

	bool operator!=(const ProtocolVersion &other) const
	{
		return !( (major_ == other.major_) && (minor_ == other.minor_));
	}

public:
	void 			SetProtocolVersion(octet major, octet minor) 	{ major_ = major; minor_ = minor; }

	std::string 	GetProtocolVersion()		const
    {
        std::stringstream ss;
        ss << major_ << "." << minor_;
        return ss.str();
    }

	octet 			GetMajorVersion()			const { return major_; }
	octet 			GetMinorVersion()			const { return minor_; }

private:
	octet major_;
	octet minor_;
};

} // namespace RTPS

#endif // RTPS_Elements_ProtocolVersion_h_IsIncluded
