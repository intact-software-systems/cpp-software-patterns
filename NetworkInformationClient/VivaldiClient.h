#ifndef NetworkInformationClient_VivaldiClient_h_IsIncluded
#define NetworkInformationClient_VivaldiClient_h_IsIncluded

#include "NetworkInformationClient/IncludeExtLibs.h"
#include "NetworkInformationClient/ActiveMeasurement.h"

namespace NetworkInformationClient
{
/*-----------------------------------------------------------------------
		struct VivaldiClientConfig
-----------------------------------------------------------------------*/
struct VivaldiClientConfig
{
	static string 	vivaldiManagerService;
	static int		vivaldiClientPort;
};

/*-----------------------------------------------------------------------
						class VivaldiClient
------------------------------------------------------------------------- */
class VivaldiClient : public ComponentBase
					, public Thread
					, public VivaldiGroupInterface		// -> Multicast
					, public VivaldiClientInterface		// -> one-to-one RMI
{
public:
	enum MeasurementType
	{
		ActiveMeasurementType = 0,		// use ActiveMeasurement.h
		PassiveMeasurementType			// TODO: PassiveMeasurement class?
	};
	
	typedef std::pair<VivaldiGroupClient*, VivaldiGroupServer*> PairVivaldiGroup;
	typedef std::set<string>						SetNames;
	typedef std::map<string, RpcMulticastManager*>	MapGroupMulticast;
	typedef std::map<string, PairVivaldiGroup >		MapGroupVivaldiGroup;
	typedef std::map<string, GroupInformation>		MapGroupNameGroupInformation;

public:
	VivaldiClient(HostInformation hostInfo, InterfaceHandle managerHandle);
  	~VivaldiClient();
	
	virtual void run();

	virtual void SetInfoUpdate(VivaldiInfo &info);
	//virtual void SetGroupUpdate(GroupInformation &info); 
	virtual void SetGroupUpdate(string groupName);

private:
	void logNodeCoordsToServer(VivaldiInfo vvip);

private:
	VivaldiManagerClient			*vivaldiManagerClient_;
	InterfaceHandle 				managerHandle_;
	ActiveMeasurement				activeMeasurement_;
	
	MapGroupVivaldiGroup			mapGroupVivaldiMulticast_;
	MapGroupMulticast				mapGroupMulticast_;
	//MapGroupNameGroupInformation	mapGroupInformation_;
	SetNames						groupNames_;

private:
	mutable Mutex 			mutex_;
	WaitCondition			waitCondition_;
	bool					runThread_;

	HostInformation			hostInformation_;
	//GroupInformation		groupInformation_;
	//VertexSet 			neighborHood_;
	VivaldiNode 			vivaldiAlgorithm_;
	VVNodeData				vivaldiData_;
};

}; // end namespace

#endif // VivaldiClient_h_IsIncluded

