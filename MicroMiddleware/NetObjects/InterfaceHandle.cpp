#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

namespace MicroMiddleware
{

std::ostream& operator<<(std::ostream &ostr, const InterfaceHandle &handle)
{
	if(handle.GetHostName() == handle.GetPrivateHostName())
	{
		ostr << "(" << handle.GetHostName() << "," << handle.GetPortNumber() << ")" ;
	}
	else
	{
		ostr << "((" << handle.GetPrivateHostName() << "," << handle.GetPrivatePortNumber() << ")" ;
		ostr << "(" << handle.GetHostName() << "," << handle.GetPortNumber() << "))" ;
	}
	return ostr;
}


NetworkLib::NetAddress InterfaceHandle::GetNetAddress() const
{
	NetAddress addr(publicHostName_, publicPortNumber_);
	addr.SetPrivateHostName(privateHostName_);
	addr.SetPrivatePortNumber(privatePortNumber_);
	
	return addr;
}

}; // namespace MicroMiddleware

