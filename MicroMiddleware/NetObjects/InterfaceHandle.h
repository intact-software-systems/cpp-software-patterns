#ifndef MicroMiddleware_InterfaceHandle_h_Included
#define MicroMiddleware_InterfaceHandle_h_Included

#include<string>
#include<ostream>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/RmiObjectBase.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class DLL_STATE InterfaceHandle : public RmiObjectBase
{
public:
	InterfaceHandle() : publicPortNumber_(-1), privatePortNumber_(-1)
		{}
	/*InterfaceHandle(const InterfaceHandle &handle) 
		: publicHostName_(handle.GetHostName())
		, publicPortNumber_(handle.GetPortNumber())
		{}*/
	InterfaceHandle(const std::string &hostName, const int &portNumber) 
		: publicHostName_(hostName)
		, publicPortNumber_(portNumber)
		, privateHostName_(hostName)
		, privatePortNumber_(portNumber)	
		{}
	InterfaceHandle(const int &portNumber, const std::string &hostName) 
		: publicHostName_(hostName)
		, publicPortNumber_(portNumber)
		, privateHostName_(hostName)
		, privatePortNumber_(portNumber)
		{}
	InterfaceHandle(const NetworkLib::NetAddress &addr)
		: publicHostName_(addr.GetHostName())
		, publicPortNumber_(addr.GetPortNumber())
		, privateHostName_(addr.GetPrivateHostName())
		, privatePortNumber_(addr.GetPrivatePortNumber())
		{}
    virtual ~InterfaceHandle()
	{}

	inline bool 	IsValid()		const { return publicPortNumber_ > -1; }
	
	NetworkLib::NetAddress	GetNetAddress() const;

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
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteString(publicHostName_);
		writer->WriteInt32(publicPortNumber_);
		writer->WriteString(privateHostName_);
		writer->WriteInt32(privatePortNumber_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		publicHostName_ 	= reader->ReadString();
		publicPortNumber_ 	= reader->ReadInt32();
		privateHostName_  	= reader->ReadString();
		privatePortNumber_  = reader->ReadInt32();
	}

public:
	bool operator==(const InterfaceHandle &other) const
	{
		return publicHostName_ == other.GetHostName() && publicPortNumber_ == other.GetPortNumber();

	}
	
	bool operator<(const InterfaceHandle &other) const
	{
		return publicPortNumber_ < other.GetPortNumber() ||
			   (publicPortNumber_ == other.GetPortNumber() && publicHostName_.length() < other.GetHostName().length());
	}

	bool operator!=(const InterfaceHandle &other) const
	{
		return !(publicHostName_ == other.GetHostName() && publicPortNumber_ == other.GetPortNumber());

	}

protected:
	std::string 	publicHostName_;
	int				publicPortNumber_;
	
	std::string		privateHostName_;
	int				privatePortNumber_;
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const InterfaceHandle &handle);

}; // namespace MicroMiddleware

#endif // 


