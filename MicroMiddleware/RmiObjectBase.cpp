#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/InterfaceHandle.h"

namespace MicroMiddleware
{

RmiObjectBase::RmiObjectBase() 
{ 

}

RmiObjectBase::~RmiObjectBase() 
{

}

void RmiObjectBase::SetInNetAddress(const NetworkLib::NetAddress &netAddress)
{
	if(inputNetAddress_.IsValid())
	{
		IDEBUG() << "WARNING! handle exists " << inputNetAddress_;
		return;
	}
	
	inputNetAddress_ = netAddress;
}


void RmiObjectBase::SetOutNetAddress(const NetworkLib::NetAddress &netAddress)
{
	if(!outputNetAddress_.IsValid())
	{
		IDEBUG() << "WARNING! Output handle exists: " << outputNetAddress_;
		return;
	}

	outputNetAddress_ = netAddress;
}

void RmiObjectBase::SetOutNetAddresses(const ListNetAddress &listAddresses)
{
	if(!listOutputAddresses_.empty())
	{
		IDEBUG() << "WARNING! Output handles exists! " ;
		return;
	}

	listOutputAddresses_ = listAddresses;
}

const NetworkLib::NetAddress& RmiObjectBase::GetInNetAddress() const
{
	return inputNetAddress_;
}

const NetworkLib::NetAddress& RmiObjectBase::GetOutNetAddress() const
{
	return outputNetAddress_;
}

const RmiObjectBase::ListNetAddress& RmiObjectBase::GetOutNetAddresses() const
{
	return listOutputAddresses_;
}

bool RmiObjectBase::HasOutAddresses() const
{
	return !listOutputAddresses_.empty();
}

} // namespace MicroMiddleware

