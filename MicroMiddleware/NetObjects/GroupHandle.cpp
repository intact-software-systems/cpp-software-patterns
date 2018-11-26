#include"MicroMiddleware/NetObjects/GroupHandle.h"

namespace MicroMiddleware
{

std::ostream& operator<<(std::ostream &ostr, const GroupHandle &handle)
{
	//ostr << "(" << handle.GetGroupName() << "," << handle.GetHostName() << "," << handle.GetPortNumber() << ")" ;
	ostr << "(" << handle.GetGroupName() << "," << (InterfaceHandle)handle << ")" ;
	return ostr;
}

}; // namespace MicroMiddleware


