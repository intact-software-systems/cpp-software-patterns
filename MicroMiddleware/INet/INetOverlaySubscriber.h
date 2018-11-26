#ifndef MicroMiddleware_INetOverlaySubscriber_h_Included
#define MicroMiddleware_INetOverlaySubscriber_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/INet/INetStub.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class INetBase;
/*--------------------------------------------------------------
	class INetOverlaySubscriber:
		- Does not create a UdpSocket
		- Rather, it registers as a Subscriber in an existing INetConnectionless
	TODO:
		- Add QosPolicy in constructor
		- service-subscription is handled from a "discovery-service" which is service-connection-manager
		  who informs ServiceManager that this subscriber wants a "service" and then receives it from
		  a publisher who is responsible for the sending
		- ServiceConnectionManager sets up an INetConnection with UDP and registers the subscriber

--------------------------------------------------------------*/
class DLL_STATE INetOverlaySubscriber : public INetStub
{
public:
	INetOverlaySubscriber(const std::string &serviceName, const std::string &subscribeToName, const std::string &interfaceName, EndianConverter::Endian hostEndian = EndianConverter::ENDIAN_LITTLE, bool autoStart = true);
	virtual ~INetOverlaySubscriber();

	virtual std::string GetConnectedToName() const;

private:
	virtual void run();
	
private:
	void ReadRpc(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer);

private:
	std::string subscribeToName_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetOverlaySubscriber_h_Included





