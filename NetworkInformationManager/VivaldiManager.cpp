#include "NetworkInformationManager/VivaldiManager.h"
#include <IntactMiddleware/MiddlewareSettings.h>

namespace NetworkInformationManager
{
/*-----------------------------------------------------------------------
		struct VivaldiManagerConfig
-----------------------------------------------------------------------*/
int VivaldiManagerConfig::vivaldiManagerPort = 3987;
string VivaldiManagerConfig::vivaldiManagerService = IntactMiddleware::MiddlewareSettings::GetNetworkInformationManagerService();

/*-----------------------------------------------------------------------
	VivaldiManager algorithm
		- Register manager service in ServiceManager as vivaldi.manager
			- Clients register as vivaldi.client.componentname
		- Clients retrieve vivaldi.manager HostInformation from ServiceManager
		- 

					VivaldiManager constructor
	- TODO: Where to get updated membership information?
		-> Input parameter MembershipManager?

	- TODO: How to map nodeId (vertex_descriptor) to componentName?
		-> Maybe MembershipManager should apply a function to map a unique integer to each member?
		-> Use process id in the beginning?
	- TODO: Communicate with IntactManager to get MembershipSet with vertex mapping?
------------------------------------------------------------------------- */
VivaldiManager::VivaldiManager(int groupSize, TreeStructure *global_mesh) 
	: globalMesh_(global_mesh)
	, groupSize_(groupSize)
{
	if(globalMesh_ == NULL)
		globalMesh_ = new TreeStructure();
}

/*-----------------------------------------------------------------------
						vivaldi server thread
	thread for creating the vivaldi groups as group_mesh_ increases in size
	distribute group to clients 
	
	TODO: 
		update global graph when new vivaldi coordinates arrive
		log node coordinates
		create vivaldi groups

	TODO:
		-- Did groupInformation change?
		 		-> Add new hosts to VivaldiGroups
		 		-> Reallocate groups that are too small due to leaving members
		 1. Create groups of VivaldiClients -> automatically create group-names 
		 		-> map<string, GroupInformation> mapVivaldiMulticastGroups
		 2. Send group-names to VivaldiClients. NB! Remember vertex-id
		 		-> VivaldiClients join groups and get GroupHandles from GroupManager
		 		-> VivaldiClients start multicasting within that group
------------------------------------------------------------------------- */
void VivaldiManager::run()
{
	try
	{
		IWARNING()  << " running!" ;
		mutex_.lock();

		GroupConnectionManager *groupConnection = GroupConnectionManager::GetOrCreate();
		groupConnection->MonitorGroup(IntactMiddleware::MiddlewareSettings::GetNetworkInformationGroup());

		while(true)
		{
			bool ret = waitCondition_.wait(&mutex_,5000);

			// Use GroupConnectionManager and join MiddlewareSettings::GetNetworkInformationGroup()
			// -> create groups of VivaldiClients and subscribe to these groups!
			GroupInformation currGroupInformation = groupConnection->GetGroupConnection(IntactMiddleware::MiddlewareSettings::GetNetworkInformationGroup());
			
			bool equalInfo = currGroupInformation.CompareTo(completeMemberInformation_);
			if(equalInfo == true) continue;

			MapHostInformation addedHosts = currGroupInformation.GetMembersNotIn(completeMemberInformation_);
			MapHostInformation removedHosts = completeMemberInformation_.GetMembersNotIn(currGroupInformation);
			
			completeMemberInformation_ = currGroupInformation;
	
			MapGroupNameGroupInformation mapMonitoredGroups = groupConnection->GetMonitoredGroups();
			
			MapComponentGroups mapComponentGroups;
			
			// 1. Add new components to existing VivaldiGroups if possible
			MapHostInformation mapToBeAdded = addedHosts;
			for(MapGroupNameGroupInformation::iterator it = mapMonitoredGroups.begin(), it_end = mapMonitoredGroups.end(); it != it_end; ++it)
			{
				GroupInformation &groupInfo = it->second;
				ASSERT(groupInfo.IsValid());
				if(groupInfo.GetNumberOfMembers() >= groupSize_) continue;

				for(MapHostInformation::iterator it = mapToBeAdded.begin(), it_end = mapToBeAdded.end(); it != it_end; ++it)
				{
					HostInformation &hostInfo = it->second;
					if(groupInfo.IsMember(hostInfo.GetComponentName())) continue;
					if(mapComponentGroups.count(hostInfo.GetComponentName()) > 0)
					{
						// a client can be member maximum 2 vivaldi client groups
						if(mapComponentGroups[hostInfo.GetComponentName()].size() >= 2) continue;
					}

					mapComponentGroups[hostInfo.GetComponentName()].insert(groupInfo.GetGroupName());
				}
			}

			//TODO: create new Vivaldi Group Name
			static int vivaldiGroupCounter = 0;
			stringstream newGroupName;
			newGroupName << "vivaldi.clientgroup." << vivaldiGroupCounter; //++;

			// 2. Create new VivaldiGroups for remaining new components 
			for(MapHostInformation::iterator it = mapToBeAdded.begin(), it_end = mapToBeAdded.end(); it != it_end; ++it)
			{
				const string &componentName = it->first;
				if(mapComponentGroups.count(componentName) > 0)
				{
					// a client can be member maximum 2 vivaldi client groups
					if(mapComponentGroups[componentName].size() >= 2) continue;
				}

				//TODO: create new Vivaldi Group Name
				//static int vivaldiGroupCounter = 0;
				//stringstream newGroupName;
				//newGroupName << "vivaldi.clientgroup." << vivaldiGroupCounter++;
				groupConnection->MonitorGroup(newGroupName.str());

				mapComponentGroups[componentName].insert(newGroupName.str()); 
			}

			// 3. use proxy and call SetGroupUpdate(groupName); to component
			MapHostInformation &mapHostInfo = completeMemberInformation_.GetGroupMembers();

			for(MapComponentGroups::iterator it = mapComponentGroups.begin(), it_end = mapComponentGroups.end(); it != it_end; ++it)
			{	
				const string &componentName = it->first;
				if(mapHostInfo.count(componentName) <= 0) 
				{
					IDEBUG() << "WARNING! " << componentName << " not found in " << completeMemberInformation_.GetGroupName() ;
					continue;
				}

				HostInformation &hostInfo = mapHostInfo[componentName];
				InterfaceHandle handle(hostInfo.GetHostIP(), hostInfo.GetPortNumber());
				
				SetString &setGroups = it->second;
				for(SetString::iterator it = setGroups.begin(), it_end = setGroups.end(); it != it_end; ++it)
				{
					string groupName = *it;
					bool ret = SendGroupUpdates(componentName, groupName, handle);
					if(ret == false)
						IWARNING()  << "TODO: Remove vivaldi client that cannot be reached! " << handle ;
				}
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << " exception caught " << ex.what() ;
	}

	mutex_.unlock();
}

void VivaldiManager::SetVivaldiCoordinates(VivaldiInfo &inf)
{
	MutexLocker locker(&mutex_);
	
	// -- debug validity of coordinates --
	IWARNING()  << " received vivaldi info TO_SERVER " << inf << " coords.size() " << inf.getNodeData().GetCoords().size() ;
	if(!appMembers_.contains(inf.getId())) checkValidity(inf);
	// -- end debug validity --

	mapNodeCoordinates_[inf.getId()] = inf.getNodeData();
	appMembers_.insert(inf.getId());

	waitCondition_.wakeAll();
}

HostInformation VivaldiManager::GetHostInformation(string componentName)
{
	MutexLocker locker(&mutex_);
	
	MapHostInformation &mapHostInfo = completeMemberInformation_.GetGroupMembers();
	if(mapHostInfo.count(componentName) <= 0)
	{
		IWARNING()  << "WARNING! Can't find component " << componentName ;
		return HostInformation();
	}

	return mapHostInfo[componentName];
}

TreeStructure* VivaldiManager::GetNetworkInformation()
{
	MutexLocker locker(&mutex_);
	return globalMesh_;
}

/*void VivaldiManager::SendGroupUpdates(GroupInformation &info)
{
	for(MapHostInformation::iterator it = info.GetGroupMembers().begin(), it_end = info.GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation &hostInfo = it->second;
		InterfaceHandle handle(hostInfo.GetHostIP(), hostInfo.GetPortNumber());

		bool ret = SendGroupUpdates(hostInfo.GetComponentName(), handle);
		if(ret == false)
			IWARNING()  << "TODO: Remove vivaldi client that cannot be reached! " << handle ;
	}
}*/

bool VivaldiManager::SendGroupUpdates(const string &componentName, const string &groupName, const InterfaceHandle &handle)
{
	try
	{
		VivaldiClientProxy *clientProxy = NULL;
		if(componentVivaldiClientProxy_.count(componentName) <= 0)
		{
			clientProxy = new VivaldiClientProxy(handle);
			clientProxy->WaitConnected();
			componentVivaldiClientProxy_[componentName] = clientProxy;
		}
		else
		{
			clientProxy = componentVivaldiClientProxy_[componentName];
			ASSERT(clientProxy);
		}

		//IWARNING()  << " ERROR! TODO: Only send groups!" ;
		IDEBUG() << " Sending group information to " << componentName << " " << handle ;

		clientProxy->SetGroupUpdate(groupName); //completeMemberInformation_);
	}
	catch(Exception ex)
	{
		IWARNING()  << "Exception caught: " << ex.msg() ;
		return false;
	}
	catch(...)
	{
		IWARNING()  << "Unhandled cxception caught!" ;
		return false;
	}

	return true;
}

void VivaldiManager::logNodeCoords(LogNodeCoordsInterval &logger)
{
	// init if first time logging
	if(logger.leverage_time <= 0)
	{
		logger.leverage_time = 20;
		logger.interval_time = 60; //std::min(PlanetLabArgs::linkAlgoTimeSec(), PlanetLabArgs::linkAlgoIntervalSec());
		logger.total_time = 0;
		logger.start_time = ::time(NULL);
		logger.log_time = logger.interval_time + logger.leverage_time;
	}

	if(::time(NULL) - logger.start_time < logger.log_time) return;

	// log node coords every n minutes
	logger.total_time += logger.interval_time;
	logNodeCoords(logger.total_time);
	
	logger.start_time = ::time(NULL);
	logger.log_time = logger.interval_time;
}

void
VivaldiManager::logNodeCoords(int seconds)
{
	if(appMembers_.empty()) return;
	
	IWARNING()  << " logging node coords " << seconds ;
	
	MutexLocker locker(&mutex_);
	
	using namespace boost::numeric::ublas;
	int maxNodeId_ = *(appMembers_.rbegin());
	distMatrix_ = matrix<double>(maxNodeId_ + 2, maxNodeId_ + 2);
	for(MapNodeCoords::iterator it = mapNodeCoordinates_.begin(), it_end = mapNodeCoordinates_.end(); it != it_end; ++it)
	{
		Coordinates X(it->second.coords);
		vertex_descriptorN x = it->second.id;
			
		for(MapNodeCoords::iterator it_in = mapNodeCoordinates_.begin(), it_in_end = mapNodeCoordinates_.end(); it_in != it_in_end; ++it_in)
		{
			vertex_descriptorN y = it_in->second.id;
			if(x == y)
			{
				distMatrix_(x, y) = 0;
			}
			else
			{
				Coordinates Y(it_in->second.coords);
				distMatrix_(x, y) = X.distance(Y);
					
				VVEdgeData ed;
				ed.weight = distMatrix_(x,y);	
				ed.id = pair<int,int>(x,y);
					
				updateMesh(ed);
			}
		}	
	}
	//ofstream fs(PlanetLabArgs::traceFile(), ios::app);
	//writeStats(*globalMesh_, seconds, fs);
	//fs.close();
	//IWARNING()  << " Global graph is finished, " << globalMesh_->V.size() << " nodes " ;
	//globalMesh_->printGraph(cerr);
}

void
VivaldiManager::checkValidity(VivaldiInfo &inf)
{
	// -- validity of statistics, 0 is valid, 1 is invalid --
	int valid_coords = 1;
	std::vector<double> coords = inf.getNodeData().GetCoords();
	for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
	{
		double w = (double)*vit;
		//cerr << w << ", " ;
		if(w > 0 && w < (numeric_limits<double>::max)()) 
		{
			valid_coords = 0;
			//cerr << " valid coords " << w << " == " << *vit << endl;
		}
	}

	//PLServerBase* sp = (PLServerBase*) SystemSpace::getObject(SystemSpace::SERVER);
	//NodeSocketMap& addressMap = sp->getAddressMap();
	//ofstream of("/hom/knuthelv/results/pl_nodes.hosts", ios::app);
	//of << valid_coords << " " << addressMap[inf.getId()]->getAddress() << endl;
	//of.close();
	// -- end validity of statistics, 0 is valid, 1 is invalid --
}

void
VivaldiManager::writeStats(const TreeStructure &T, const double &seconds, ostream &ostr)
{
	// inside critical section
	ostr << "-V s -t " << seconds << " -g " << groupSize_ << " -d " << VivaldiSpace::getDim() << endl;

	for(map<int, VertexSet>::iterator it = vivaldiGroups_.begin(), it_end = vivaldiGroups_.end(); it != it_end; ++it)
	{
		VertexSet group = it->second;
		vertex_descriptorN v = it->first;
			
		ostr << "-V g -id " << v << " -m " ;
		for(VertexSet::iterator it_in = group.begin(), it_in_end = group.end(); it_in != it_in_end; ++it_in)
		{
			if(*it_in == v) continue;
			ostr << *it_in << " " ;
		}
		ostr << endl;
	}
		
	for(MapNodeCoords::iterator it = mapNodeCoordinates_.begin(), it_end = mapNodeCoordinates_.end(); it != it_end; ++it)
	{
		Coordinates X(it->second.coords);
		vertex_descriptorN x = it->second.id;
		
		ostr << "-V c -id " << x << " -c " ;

		for(vector<double>::iterator vit = it->second.coords.begin(), vit_end = it->second.coords.end(); vit != vit_end; ++vit)
		{
			ostr << *vit << " " ;
		}
		ostr << endl;	
	}	

	for(VertexSet::const_iterator it = T.V.begin(), it_end = T.V.end(); it != it_end; it++)
	{
		vertex_descriptorN u = *it;
		if(!appMembers_.contains(u)) continue;

		for(VertexSet::const_iterator it_in = it, it_in_end = T.V.end(); it_in != it_in_end; it_in++)
		{
			if(it_in == it) continue;
				
			vertex_descriptorN v = *it_in;
			if(!appMembers_.contains(v)) continue;

			pair<edge_descriptorN, bool> ep = edge(u,v,T.g);
			ASSERT(ep.second);
			
			ostr << "-V e -e " << ep.first << " -lt " << T.g[ep.first].weight << endl;
		}
	}
}

/*-----------------------------------------------------------------------
		new members have arrived
------------------------------------------------------------------------- */
void VivaldiManager::AddNewMembers(GroupInformation &completeMemberSet)
{
	MutexLocker lock(&mutex_);
	completeMemberInformation_ = completeMemberSet;
}

void VivaldiManager::AddNewMembers(VertexSet &completeMemberSet)
{
	IWARNING()  << " waiting for mesh update " << strerror(errno) ;

	mutex_.lock();
		
	VertexSet newMembers = completeMemberSet - prevMeshNodes_ - 0;
	if(newMembers.size() >= groupSize_) prevMeshNodes_ = completeMemberSet; 
	
	mutex_.unlock();

	IWARNING()  << " ----------------------------> mesh updated " << newMembers ;

	if(newMembers.size() >= groupSize_) 
		createRandomGroups(newMembers, groupSize_);
}

void VivaldiManager::createRandomGroups(VertexSet &newMembers, int sz)
{
	IWARNING()  << " creating groups " << newMembers << " : " ;

	int max_num = newMembers.size() + 1;
	for(VertexSet::iterator it = newMembers.begin(), it_end = newMembers.end(); it != it_end; ++it)
	{
		VertexSet group(*it);
			
		while(group.size() < sz)
		{
			int v = VivaldiSpace::randomInt(max_num);
			if(newMembers.count(v)) group.insert(v);
		}

		vivaldiGroups_.insert(pair<int,VertexSet>(*it, group));
	}

	waitCondition_.wakeAll();
}

void
VivaldiManager::updateMesh(const VVEdgeData &ed)
{
	// do operations on globalMesh_
	EdgeProp ep;
	ep.id 		= ed.id;
	ep.weight 	= std::max(0.1, ed.weight);
	ep.cost		= 1;
	ep.delay 	= std::max(0.1, ed.weight);
	ep.bw		= 1;
	ep.length	= std::max(0.1, ed.weight);

	globalMesh_->updateEdge(ep.id, ep);
}


/*void VivaldiManager::UpdateInformation(GroupInformation &currGroupInformation)
{
	typedef map<string, HostInformation> MapHostInformation;

	MapHostInformation addedHosts = currGroupInformation.GetMembersNotIn(completeMemberInformation_);
	MapHostInformation removedHosts = completeMemberInformation_.GetMembersNotIn(currGroupInformation);

	// 1. Remove hosts
	for(MapGroupNameGroupInformation::iterator it = mapGroupVivaldiClients_.begin(), it_end = mapGroupVivaldiClients_.end(); it != it_end; ++it)
	{
		const string &groupName = it->first;
		GroupInformation &groupInfo = it->second;
	
		MapHostInformation &mapMembers = groupInfo->GetGroupMembers();
		for(MapHostInformation::iterator it = removedHosts.begin(), it_end = removedHosts.end(); it != it_end; ++it)
		{
			HostInformation &hostInfo = it->second;
			
			if(mapVivaldiClientGroups_.count(hostInfo.GetComponentName()) > 0)
			{
				SetString &componentGroups = mapVivaldiClientGroups_[hostInfo.GetComponentName()];
				componentGroups.

				
			}

		}
		
		for(MapHostInformation::iterator it = mapMembers.begin(), it_end = mapMembers.end(); it != it_end; ++it)
		{
			Host


		}
	}


	// 2. Add hosts


}*/




} // end namespace NetworkInformationManager


