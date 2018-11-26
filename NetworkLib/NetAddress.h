#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/Serialize/NetObjectBase.h"

#include"NetworkLib/Export.h"
namespace NetworkLib
{

class DLL_STATE NetAddress : public NetObjectBase
{
public:
	NetAddress() : publicPortNumber_(-1), privatePortNumber_(-1)
		{}
	/*NetAddress(const NetAddress &handle)
		: publicHostName_(handle.GetHostName())
		, publicPortNumber_(handle.GetPortNumber())
		{}*/
	NetAddress(const std::string &hostName, const int &portNumber)
		: publicHostName_(hostName)
		, publicPortNumber_(portNumber)
		, privateHostName_(hostName)
		, privatePortNumber_(portNumber)
		{}
	NetAddress(const int &portNumber, const std::string &hostName)
		: publicHostName_(hostName)
		, publicPortNumber_(portNumber)
		, privateHostName_(hostName)
		, privatePortNumber_(portNumber)
		{}
	virtual ~NetAddress()
	{}

    CLASS_TRAITS(NetAddress)

	inline bool 	IsValid()		const { return publicPortNumber_ > -1; }

	// -- get functions
	inline std::string 	GetHostName()			const { return publicHostName_; }
	inline int			GetPortNumber()			const { return publicPortNumber_; }
	inline std::string	GetPrivateHostName() 	const { return privateHostName_; }
	inline int			GetPrivatePortNumber() 	const { return privatePortNumber_; }

	// -- set functions
	inline void 	SetHostName(const std::string &hostName) 	{ publicHostName_ = hostName; }
	inline void 	SetPortNumber(const int &num)				{ publicPortNumber_ = num; }
	inline void 	SetPrivateHostName(const std::string &name)	{ privateHostName_ = name; }
	inline void 	SetPrivatePortNumber(const int &num)		{ privatePortNumber_ = num; }

	// -- serialize functions
	virtual void Write(SerializeWriter *writer) const
	{
		writer->WriteString(publicHostName_);
		writer->WriteInt32(publicPortNumber_);
		writer->WriteString(privateHostName_);
		writer->WriteInt32(privatePortNumber_);
	}

	virtual void Read(SerializeReader *reader)
	{
		publicHostName_ 	= reader->ReadString();
		publicPortNumber_ 	= reader->ReadInt32();
		privateHostName_  	= reader->ReadString();
		privatePortNumber_  = reader->ReadInt32();
	}

public:
	bool operator==(const NetAddress &other) const
	{
        return publicHostName_ == other.GetHostName() && publicPortNumber_ == other.GetPortNumber();
    }

	bool operator<(const NetAddress &other) const
	{
		return publicPortNumber_ < other.GetPortNumber() ||
			   (publicPortNumber_ == other.GetPortNumber() && publicHostName_.length() < other.GetHostName().length());
	}

	bool operator!=(const NetAddress &other) const
	{
		return !(publicHostName_ == other.GetHostName() && publicPortNumber_ == other.GetPortNumber());
	}

protected:
	std::string 	publicHostName_;
	int				publicPortNumber_;

	std::string		privateHostName_;
	int				privatePortNumber_;
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const NetAddress &handle);

}
