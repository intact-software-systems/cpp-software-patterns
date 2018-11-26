#ifndef NetworkInformationManager_VivaldiManager_h_IsIncluded
#define NetworkInformationManager_VivaldiManager_h_IsIncluded

#include "NetworkInformationManager/IncludeExtLibs.h"

namespace NetworkInformationManager
{

/*-----------------------------------------------------------------------
		struct VivaldiManagerConfig
-----------------------------------------------------------------------*/
struct VivaldiManagerConfig
{
	static int		vivaldiManagerPort;
	static string 	vivaldiManagerService;
};


/*-----------------------------------------------------------------------
		class VivaldiManager
------------------------------------------------------------------------- */
class VivaldiManager : public Thread
					 , public VivaldiManagerInterface
{
private:
	struct LogNodeCoordsInterval
	{
		LogNodeCoordsInterval() 
			: leverage_time(0)
			  , interval_time(0)
			  , total_time(0)
			  , start_time(0)
			  , log_time(0)
		{}
		~LogNodeCoordsInterval()
		{}

		time_t leverage_time;
		time_t interval_time;
		time_t total_time;
		time_t start_time;
		time_t log_time;
	};

public:
	VivaldiManager(int groupSize = 4, TreeStructure *global_mesh = NULL);
	~VivaldiManager()
	{}
	typedef map<string, HostInformation> 	MapHostInformation;

	// interface function
	virtual void SetVivaldiCoordinates(VivaldiInfo &inf);
	virtual HostInformation GetHostInformation(string componentName);

	virtual TreeStructure* GetNetworkInformation();

public:
	// add new members
	void AddNewMembers(VertexSet &completeMemberSet);
	void AddNewMembers(GroupInformation &completeMemberSet);

	//void SendGroupUpdates(GroupInformation &info);
	bool SendGroupUpdates(const string &componentName, const string &groupName, const InterfaceHandle &handle);

public:
	map<int, VertexSet> GetVivaldiGroups()		{ MutexLocker lock(&mutex_); return vivaldiGroups_; }

protected:
	virtual void run();
	
private:
	void logNodeCoords(LogNodeCoordsInterval &logger);
	void logNodeCoords(int seconds);
	void checkValidity(VivaldiInfo &vip);
	void updateMesh(const VVEdgeData &ed); 
	void createRandomGroups(VertexSet &newMembers, int sz);
	void writeStats(const TreeStructure &g, const double &seconds, ostream &ostr);

protected:
	typedef map<int, VVNodeData> 			MapNodeCoords;
	MapNodeCoords 							mapNodeCoordinates_;
	boost::numeric::ublas::matrix<double> 	distMatrix_; 

private:
	LogNodeCoordsInterval 	logNodeCoordsInterval_;
	int 					groupSize_;

	Mutex 					mutex_;
	WaitCondition 			waitCondition_;

private:
	TreeStructure  			*globalMesh_;  		// shared copy with server thread
	VertexSet				prevMeshNodes_;
	map<int, VertexSet> 	vivaldiGroups_;
	VertexSet				appMembers_;
	GroupInformation 		completeMemberInformation_;

	typedef std::set<string>						SetString;
	typedef std::map<string, SetString>				MapComponentGroups;
	typedef std::map<string, GroupInformation>		MapGroupNameGroupInformation;
	typedef std::map<string, int>					MapComponentVertexId;
	typedef std::map<string, VivaldiClientProxy*>  	MapComponentVivaldiClientProxy;
	
	//MapComponentGroups						mapVivaldiClientGroups_;
	MapGroupNameGroupInformation 			mapGroupVivaldiClients_;
	MapComponentVertexId					mapComponentVertexId_;
	MapComponentVivaldiClientProxy 			componentVivaldiClientProxy_;
};

}// end namespace 

#endif // VivaldiManager_h_IsIncluded

