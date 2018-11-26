#ifndef MicroMiddleware_GroupHandle_h_Included
#define MicroMiddleware_GroupHandle_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class DLL_STATE GroupHandle : public InterfaceHandle
{
public:
	GroupHandle() 
		: InterfaceHandle() 
	{}
	GroupHandle(std::string groupName, std::string hostName, int portNumber) 
		: InterfaceHandle(hostName, portNumber)
		, groupName_(groupName) 
	{}
	GroupHandle(std::string groupName, const InterfaceHandle &handle) 
		: InterfaceHandle(handle.GetHostName(), handle.GetPortNumber())
		, groupName_(groupName) 
	{}
    virtual ~GroupHandle()
	{}

public:
	// check is valid
	inline bool IsValid()						const	{ if(groupName_.empty()) return false; return InterfaceHandle::IsValid(); }

	// -- get/set functions
	inline std::string GetGroupName()			const 	{ return groupName_; }
	inline void SetGroupName(std::string g)				{ groupName_ = g; }

public:
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteString(groupName_);
		InterfaceHandle::Write(writer);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		groupName_ = reader->ReadString();
		InterfaceHandle::Read(reader);
	}

public:
	bool operator==(const GroupHandle &other) const
	{
		if(groupName_ == other.GetGroupName())
			return true;

		return false;
	}
	
	bool operator<(const GroupHandle &other) const
	{
		return this->GetPortNumber() < other.GetPortNumber() ||
			   (this->GetPortNumber() == other.GetPortNumber() && this->GetHostName().length() < other.GetHostName().length()) ||
			   (this->GetPortNumber() == other.GetPortNumber() && this->GetHostName().length() == other.GetHostName().length() && groupName_.length() < other.GetGroupName().length());
	}

private:
	std::string groupName_;
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const GroupHandle &handle);

}; // namespace MicroMiddleware

#endif // 



