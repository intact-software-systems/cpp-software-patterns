#ifndef MicroMiddleware_INetOverlayPublisher_h_Included
#define MicroMiddleware_INetOverlayPublisher_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/GroupInformation.h"
#include"MicroMiddleware/INet/INetProxy.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class INetBase;
class RmiObjectBase;
class RpcIdentifier;

/*--------------------------------------------------------------
	class INetOverlayPublisher:
		- Does not connect to a remote host
		- Rather, it uses an INetConnectionless/INetBase to serialize a published topic

	TODO:
		- Thread can decide what to do with new published topics based on QoS and listening subscribers 
		- Thread can automatically detect if INetConnection is down.
		- QosPolicy -> SetQosPolicy()
		- QosPolicy negotiation between stub and proxy?
		- I think QoSPolicy is more applicable in a publisher/subscriber scenario.
		- Adding and removing subscribers is controlled from a "discovery service" 
			which is service-connection-manager.

listOfSubscribers is manipulated from outside by ServiceConnectionManager

	TODO:
		- Implement an interface template class DataPublisher<class Topic> : protected INetOverlayPublisher
			- Topic class must implement serializable functions Write/Read
			- template class accesses INetOverlayPublisher's functions to write data
			- template class accesses HistoryCache
			- template class provide call-back functionality to user-impl?
		- Implement an interface template class DataSubscriber<class Topic> : protected INetOverlaySubscriber
			- template class accesses INetOverlaySubscriber's function to read data
			- template class accesses HistoryCache

// virtual bool PublishTopic(RpcIdentifier *rpc, RmiObjectBase *obj);  -> calls messageParser_->PublishTopic(rpc, obj, listOfSubscribers);
--------------------------------------------------------------*/

class DLL_STATE INetOverlayPublisher : protected INetProxy
{
public:
	INetOverlayPublisher(const std::string &serviceName, const std::string &interfaceName, EndianConverter::Endian hostEndian = EndianConverter::ENDIAN_LITTLE);
	virtual ~INetOverlayPublisher();

	virtual bool ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj);
	virtual bool EndMarshal(SerializeWriter::Ptr &writer);
	
	virtual std::string GetConnectedToName() const;
	
	// called from outside by ServiceConnectionManager
	virtual bool AddSubscriber(HostInformation &subscriberInfo);
	virtual bool RemoveSubscriber(std::string &subscriberName);
	
private:
	virtual void run();

private:
	// listOfSubscribers -> HostInformations/InterfaceHandles		
	// manipulated from ServiceConnectionManager
	GroupInformation	subscriberGroup_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetOverlayPublisher_h_Included


