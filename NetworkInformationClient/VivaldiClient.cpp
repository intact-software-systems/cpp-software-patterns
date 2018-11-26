#include "NetworkInformationClient/VivaldiClient.h"
#include <IntactMiddleware/MiddlewareSettings.h>

namespace NetworkInformationClient
{
/*-----------------------------------------------------------------------
		struct VivaldiClientConfig
-----------------------------------------------------------------------*/
string VivaldiClientConfig::vivaldiManagerService = IntactMiddleware::MiddlewareSettings::GetNetworkInformationManagerService();
int VivaldiClientConfig::vivaldiClientPort = 6789;
//int VivaldiClientConfig::vivaldiClientHost = CURRENT_HOST_NAME;

/*-----------------------------------------------------------------------
					VivaldiClient constructor
		TODO
			- should I have a ClientDynamicsManager here?
		Algorithm:
			- MembershipManager has my vertex id
			- IntactManager calls SetGroupUpdate with GroupInformation
			- Init VivaldiGroupClient
			- Start ActiveMeasurement
			- Send out rtt measurements to hosts in GroupInformation
			- Receive rtt measurements from neighbor
				- TODO: if neighbor not in my group, include neighbor in GroupInformation?
				- TODO: reply with updated VivaldiInformation to neighbor
------------------------------------------------------------------------- */
VivaldiClient::VivaldiClient(HostInformation hostInfo, InterfaceHandle managerHandle) 
	: ComponentBase(hostInfo.GetComponentName())
	, Thread(hostInfo.GetComponentName())
	, vivaldiManagerClient_(NULL)
	, managerHandle_(managerHandle)
	, activeMeasurement_("NetworkInformationClient.ActiveMeasurement", ActiveMeasurement::PingICMP, 1000)
	, runThread_(true)
	, hostInformation_(hostInfo)
{ 
    ASSERT(hostInformation_.GetComponentId() > -1);
	vivaldiAlgorithm_ = VivaldiNode(VivaldiSpace::getDim(), VivaldiSpace::maxNodeErr); 
	vivaldiData_ = VVNodeData(hostInformation_.GetComponentId(), VivaldiSpace::getDim());
}

VivaldiClient::~VivaldiClient()
{

}

/*-----------------------------------------------------------------------
		vivaldi thread - generates vivaldi information to neighbors

	TODO:
		1. Join MembershipManager through MembershipManagerConnection
			-> MembershipManager updates HostInformation and automatically generates a unique vertex-id
			-> Use vertex-id when joining group MiddlewareSettings::GetNetworkInformationGroup()
				-> Subscribe to VivaldiClient group
			-> VivaldiManager now has vertex-id of all VivaldiClients
				-> Sends group-name and GroupInformation to VivaldiClients 
------------------------------------------------------------------------- */
void VivaldiClient::run()
{
	// for sending Vivaldi information to neighbors
	IWARNING()  << " host " << hostInformation_ ;
	
	time_t start_time = ::time(NULL); 
	time_t interval_time = 10; 
	time_t total_time = 0;

	mutex_.lock();

	GroupConnectionManager *groupConnection = GroupConnectionManager::GetOrCreate();

	while(runThread_) 
	{
		bool ret = waitCondition_.wait(&mutex_, 1000);
		if(groupNames_.size() <= 0) continue;

		for(SetNames::const_iterator it = groupNames_.begin(), it_end = groupNames_.end(); it != it_end; ++it)
		{
			//string groupName = *it;
			//GroupInformation prevGroupInformation;
			//if(mapGroupInformation_.count(groupName) >= 1)
			//	prevGroupInformation = mapGroupInformation_[groupName];

			GroupInformation currGroupInformation = groupConnection->GetGroupConnection(*it); //groupName);
			if(currGroupInformation.IsValid() == false) continue;
			//if(currGroupInformation.CompareTo(prevGroupInformation) == true) continue;
			//mapGroupInformation_[groupName] = currGroupInformation;
			activeMeasurement_.SetGroupInformation(currGroupInformation);
		}

		VivaldiInfo vvip(vivaldiData_, hostInformation_);
		
		//for(MapGroupMulticast::iterator it = mapGroupMulticast_.begin(), it_end = mapGroupMulticast_.end(); it != it_end; ++it)
		for(MapGroupVivaldiGroup::iterator it = mapGroupVivaldiMulticast_.begin(), it_end = mapGroupVivaldiMulticast_.end(); it != it_end; ++it)
		{
			const string &groupName = it->first;
			//RpcMulticastManager *multicastManager = it->second;
			const PairVivaldiGroup &pairVivaldi = it->second;
			
			IWARNING()  << " multicasting " << groupName << " vivaldi info " << vvip ;

			VivaldiGroupClient *vivaldiGroupClient = pairVivaldi.first; //dynamic_cast<VivaldiGroupClient*>(multicastManager->GetMulticastClient());
			ASSERT(vivaldiGroupClient);
			
			vivaldiGroupClient->SetInfoUpdate(vvip);
		}

		// periodic logging to server
		if(::time(NULL) - start_time > interval_time + total_time)
		{
			total_time = total_time + interval_time; 
			logNodeCoordsToServer(vvip);
		}
	}
	
	mutex_.unlock();
}

void VivaldiClient::logNodeCoordsToServer(VivaldiInfo vvip)
{
	try
	{
		IWARNING()  << " logging node coords to server "  ;
		if(vivaldiManagerClient_ == NULL)
		{
			vivaldiManagerClient_ = new VivaldiManagerClient(managerHandle_);
			
			IDEBUG() << " Sending coordinates to server. Waiting for connection!! " ;
			vivaldiManagerClient_->WaitConnected();
		}

		ASSERT(vivaldiManagerClient_);
		vivaldiManagerClient_->SetVivaldiCoordinates(vvip);
	}
	catch(Exception ex)
	{
		IDEBUG() << "Could not contact " << managerHandle_ << " " << ex.msg() ;
	}
	catch(...)
	{
		IDEBUG() << "Could not contact " << managerHandle_ ;
		//if(vivaldiManagerClient_ != NULL)
		//{
		//	delete vivaldiManagerClient_;
		//	vivaldiManagerClient_ = NULL;
		//}
	}
}

/*-----------------------------------------------------------------------
					RMI received
------------------------------------------------------------------------- */
// TODO: Add function for VivaldiSpace::NEIGHBOR_UNREACHABLE
// Other?

// VivaldiInfo.getType() == VivaldiSpace::NEIGHBOR
// TODO: Is it necessary to reply to this RMI call?
// If yes -> Then I do not have a proxy to help me!
//        -> With INetConnection a connection is already available
//		  -> And, a stub is attached to it!
void VivaldiClient::SetInfoUpdate(VivaldiInfo &inf)
{
	MutexLocker lock(&mutex_);

	double rtt = activeMeasurement_.GetRoundTripTime(inf.getHostInformation().GetComponentName());
	if(rtt > 0.0f) 
	{
		inf.setRtt(rtt);
		vivaldiAlgorithm_.update(inf.getNodeData());

		if(vivaldiAlgorithm_.updateData(vivaldiData_)) 
		{
			IWARNING()  << " updated information " ;
			//waitCondition_.wakeAll();
		}
	}
}

// TODO: Add to VivaldiClientInterface
void VivaldiClient::SetGroupUpdate(string groupName)
{
	MutexLocker lock(&mutex_);

	if(mapGroupMulticast_.count(groupName) > 0)
	{
		IDEBUG() << "WARNING! " << GetComponentName() << " is already registered to group " << groupName ;
		return;
	}

	RpcMulticastManager *multicastManager = new RpcMulticastManager();
	multicastManager->StartMulticast();
	
	VivaldiGroupServer *vivaldiGroupServer = new VivaldiGroupServer(GetComponentName() + "." + groupName + ".stub", this);
	VivaldiGroupClient *vivaldiGroupClient = new VivaldiGroupClient(multicastManager->GetMulticastClient(), groupName);
	
	bool ret = multicastManager->RegisterInterface(VivaldiGroupInterface::GetInterfaceId(), vivaldiGroupServer);
	ASSERT(ret);
	ret = multicastManager->JoinGroup(groupName);
	ASSERT(ret);

	mapGroupMulticast_[groupName] = multicastManager;
	mapGroupVivaldiMulticast_[groupName] = PairVivaldiGroup(vivaldiGroupClient, vivaldiGroupServer);

	groupNames_.insert(groupName);

	IDEBUG() << GetComponentName() << " is registered in group " << groupName ;
}

/*void VivaldiClient::SetGroupUpdate(GroupInformation &groupInfo)
{
	MutexLocker lock(&mutex_);
	
	if(neighborHood_.empty() == false)
		IWARNING()  << "WARNING! Already received Vivaldi group " << neighborHood_ ;

	typedef map<string, HostInformation> 	MapHostInformation;
	for(MapHostInformation::iterator it = groupInfo.GetGroupMembers().begin(), it_end = groupInfo.GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation &hostInfo = it->second;
		if(hostInfo.GetComponentName() == hostInformation_.GetComponentName())
		{
			IDEBUG() << " skipping my host info : " << hostInfo ;
			continue;
		}

		int neighborNodeId = hostInfo.GetComponentId();
		ASSERT(neighborNodeId >= 0);

		IWARNING()  << hostInfo.GetComponentName() << " == vertex# " << hostInfo.GetComponentId() ;
		neighborHood_.insert(neighborNodeId);
	}

	groupInformation_ = groupInfo;

	IDEBUG() << " Group Information : " << groupInformation_.GetGroupName() ;
	groupInformation_.Print();

	if(vivaldiGroupServer_ == NULL)
	{
		ASSERT(vivaldiGroupClient_ == NULL);
		ASSERT(vivaldiGroupServer_ == NULL);

		multicastManager_->StartMulticast();
	
		vivaldiGroupServer_ = new VivaldiGroupServer(GetComponentName() + ".group.stub", this);
		vivaldiGroupClient_ = new VivaldiGroupClient(multicastManager_->GetMulticastClient(), groupInformation_.GetGroupName());
	
		bool ret = multicastManager_->RegisterInterface(VivaldiGroupInterface::GetInterfaceId(), vivaldiGroupServer_);
		ASSERT(ret);
		ret = multicastManager_->RegisterInGroup(groupInformation_.GetGroupName());
		ASSERT(ret);

		IDEBUG() << "Registered in group " << groupInformation_.GetGroupName() ;
	}

	activeMeasurement_.SetGroupInformation(groupInformation_);
	//TODO: vivaldiData_ = VVNodeData(nodeId, VivaldiSpace::getDim());
	
	// start sending to neighbors
	//waitCondition_.wakeAll();
}*/

}; // end namespace NetworkInformationClient


