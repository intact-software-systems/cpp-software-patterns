#ifndef MicroMiddleware_RmiObjectBase_h_Included
#define MicroMiddleware_RmiObjectBase_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{
class DLL_STATE RmiObjectBase
{
protected:
	typedef std::list<NetworkLib::NetAddress>	ListNetAddress;

public:
	RmiObjectBase();
	virtual ~RmiObjectBase();

	virtual void Write(NetworkLib::SerializeWriter*) = 0;
	virtual void Read(NetworkLib::SerializeReader *) = 0;
	
	void SetInNetAddress(const NetworkLib::NetAddress &addr);
	void SetOutNetAddress(const NetworkLib::NetAddress &addr);
	void SetOutNetAddresses(const ListNetAddress &listHandles);

	const NetworkLib::NetAddress&	GetInNetAddress() const;
	const NetworkLib::NetAddress&	GetOutNetAddress() const;
	const ListNetAddress&			GetOutNetAddresses() const;

	bool HasOutAddresses() const;

protected:
	NetworkLib::NetAddress 	inputNetAddress_;
	NetworkLib::NetAddress	outputNetAddress_;
	ListNetAddress			listOutputAddresses_;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_RmiObjectBase_h_Included

