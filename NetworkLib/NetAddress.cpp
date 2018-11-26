#include"NetworkLib/NetAddress.h"

namespace NetworkLib
{

std::ostream& operator<<(std::ostream &ostr, const NetAddress &handle)
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

} // namespace NetworkLib


