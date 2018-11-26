/***************************************************************************
                          simdefs.h  -  description
                             -------------------
    begin                : Fri Apr 28 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_SIM_DEFINITIONS_KHV
#define GRAPHALGO_SIM_DEFINITIONS_KHV

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <queue>
#include <map>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <limits>
#include <time.h>
#include "./network/vertex_set.h"

using namespace std;
//using namespace boost;

/*-----------------------------------------------------------------------
					Node type enums
------------------------------------------------------------------------- */
enum ServerType { 
	SERVER_TYPE_ERROR = 0,

	NO_SERVER_TYPE,	
	CENTRAL_SERVER_STATIC,
	CENTRAL_SERVER_DYNAMIC,
	
	CENTRAL_SERVER_STATIC_PLANET,
	CENTRAL_SERVER_DYNAMIC_PLANET,
	
	END_SERVER_TYPE	
};
const char *printAlgo(ServerType type);
ServerType getServerAlgo(const char* type);
std::ostream& operator<<(std::ostream& ostr, const ServerType &type);

enum ClientType {
	CLIENT_TYPE_ERROR = 0,
	
	NO_CLIENT_TYPE,
	CLIENT_TERMINAL,
	CLIENT_TERMINAL_PLANET,
	
	END_CLIENT_TYPE
};
const char *printAlgo(ClientType type);
ClientType getClientAlgo(const char *type);
std::ostream& operator<<(std::ostream& ostr, const ClientType &type);

enum ProxyType {
	PROXY_TYPE_ERROR = 0,
	
	NO_PROXY_TYPE,
	PROXY_BASIC,
	
	END_PROXY_TYPE
};
const char *printAlgo(ProxyType type);
ProxyType getProxyAlgo(const char *type);
std::ostream& operator<<(std::ostream& ostr, const ProxyType &type);

namespace TreeAlgorithms
{
/*-----------------------------------------------------------------------
		TreeAlgo - Supported Algorithms in the simulator
------------------------------------------------------------------------- */
enum TreeAlgo { 
	TREE_ALGO_ERROR = 0,

	NO_TREE_ALGO,
	
	// unconstrained algorithms	
	SERVER_SHORTEST_PATH_TREE,
	PROXY_SHORTEST_PATH_TREE,
	SHORTEST_PATH_TREE,
	SHORTEST_PATH_TREE_STEINER,
	MINIMUM_SPANNING_TREE,
	PRUNED_MINIMUM_SPANNING_TREE,
	SHORTEST_PATH_HEURISTIC, 
	DISTANCE_NETWORK_HEURISTIC,
	AVERAGE_DISTANCE_HEURISTIC,
	MINIMUM_DIAMETER_SHORTEST_PATH_HEURISTIC,
	MINIMUM_DIAMETER_DISTANCE_NETWORK_HEURISTIC,
	MINIMUM_DIAMETER_ONE_TIME_TREE_CONSTRUCTION,
	
	// degree limited algorithms
	DEGREE_LIMITED_SHORTEST_PATH_TREE,
	DEGREE_LIMITED_SHORTEST_PATH_TREE_STEINER,
	DEGREE_LIMITED_MINIMUM_SPANNING_TREE,
	PRUNED_DEGREE_LIMITED_MINIMUM_SPANNING_TREE,
	COMPACT_TREE,
	PRUNED_COMPACT_TREE,
	DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC,
	DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_BAUER,
	DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC,
	DEGREE_LIMITED_SHORTEST_PATH_DISTANCE_NETWORK_HEURISTIC,
	MINIMUM_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC,
	MINIMUM_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC,
	MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION,
	MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_STEINER,
	
	// diameter bounded algorithms
	BOUNDED_DIAMETER_OTTC_SHORTEST_PATH_HEURISTIC,
	BOUNDED_DIAMETER_RGH_SHORTEST_PATH_HEURISTIC,
	BOUNDED_DIAMETER_RH_SHORTEST_PATH_HEURISTIC,
	BOUNDED_DIAMETER_DISTANCE_NETWORK_HEURISTIC,
	BOUNDED_DIAMETER_RGH_DISTANCE_NETWORK_HEURISTIC,
	ONE_TIME_TREE_CONSTRUCTION_BDMST,
	RANDOMIZED_GREEDY_HEURISTIC_BDMST,	

	// diameter bounded and degree limited
	BOUNDED_COMPACT_TREE,
	BOUNDED_DIAMETER_DEGREE_LIMITED_OTTC_SHORTEST_PATH_HEURISTIC,
	BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_SHORTEST_PATH_HEURISTIC,
	BOUNDED_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC,
	BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_DISTANCE_NETWORK_HEURISTIC,
	DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST,
	DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST_STEINER,
	DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST,
	DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST_STEINER,

	// radius constrained algorithms
	RADIUS_CONSTRAINED_SHORTEST_PATH_HEURISTIC,
	RADIUS_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC,

	// delay constrained and degree limited algorithms
	DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_TREE,
	DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH,
	DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC,

	// delay constrained algorithms
	DELAY_CONSTRAINED_SHORTEST_PATH_TREE,
	DELAY_CONSTRAINED_SHORTEST_PATH,
	DELAY_CONSTRAINED_SHORTEST_PATH_HEURISTIC,
	
	END_TREE_ALGO
};

const char* printAlgo(TreeAlgo treeAlgo);
TreeAlgo getTreeAlgo(const char *algo);
std::ostream& operator<<(std::ostream& ostr, const TreeAlgo& treeAlgo);

/*-----------------------------------------------------------------------
		DynamicTreeAlgo - Supported Algorithms in the simulator
------------------------------------------------------------------------- */
enum DynamicTreeAlgo { 
	DYNAMIC_TREE_ALGO_ERROR = 0,
	NO_DYNAMIC_TREE_ALGO,
	
	INSERT_MAKE_MEMBER,
	
	// degree bounded
	INSERT_RANDOM_EDGE,
	INSERT_MINIMUM_COST_EDGE,
	INSERT_MINIMUM_COST_EDGE_PRIORITY_WCN,
	INSERT_MINIMUM_COST_EDGE_NON_LEAF,
	INSERT_CENTER_NODE,
	INSERT_CENTER_MEMBER_NODE,
	INSERT_CENTER_WCN,
	INSERT_MINIMUM_WCN, 
	INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE,
	INSERT_MINIMUM_RADIUS_DEGREE_LIMITED_EDGE,
	INSERT_TRY_REPLACE_MC_NAIVE,
	INSERT_TRY_REPLACE_MDDL_NAIVE,
		
	// delay constrained and degree bounded
	INSERT_DC_MINIMUM_COST_EDGE,
	INSERT_DC_MINIMUM_COST_EDGE_PRIORITY_WCN,
	INSERT_DC_CENTER_NODE,
	INSERT_DC_CENTER_WCN,
	INSERT_DC_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE,

	// bounded diameter
	INSERT_BD_MINIMUM_COST_EDGE,
	INSERT_BD_MINIMUM_COST_EDGE_PRIORITY_WCN,
	INSERT_BD_CENTER_NODE,
	INSERT_BD_CENTER_WCN,

	// bounded radius heuristics
	INSERT_BR_MINIMUM_COST_EDGE,
	INSERT_BR_MINIMUM_COST_EDGE_PRIORITY_WCN,
	INSERT_BR_CENTER_NODE,
	INSERT_BR_CENTER_WCN,

	// subgraph
	INSERT_SUBGRAPH,
	
	// -- unused --
	INSERT_BEST_NEIGHBORHOOD_EDGE,
	INSERT_MINIMUM_DIAMETER_BEST_NEIGHBORHOOD_EDGE,
	INSERT_MINIMUM_DIAMETER_EDGE,
	// -- unused end --
	
	// remove
	REMOVE_LEAF,
	// degree bounded
	REMOVE_KEEP_AS_NON_MEMBER,
	REMOVE_KEEP_WELL_CONNECTED,
	REMOVE_TRY_REPLACE_MC_NAIVE,
	REMOVE_TRY_REPLACE_MDDL_NAIVE,
	REMOVE_TRY_REPLACE_PRUNE_MC,
	REMOVE_TRY_REPLACE_PRUNE_SEARCH_MC,
	REMOVE_TRY_REPLACE_PRUNE_MDDL,
	REMOVE_TRY_REPLACE_PRUNE_SEARCH_MDDL,
	REMOVE_MINIMUM_COST_EDGE,
	REMOVE_SEARCH_MINIMUM_COST_EDGE, 
	REMOVE_MINIMUM_DIAMETER_EDGE,
	REMOVE_SEARCH_MINIMUM_DIAMETER_EDGE, 
	
	// delay constrained
	REMOVE_DC_MINIMUM_COST_EDGE,
	REMOVE_DC_SEARCH_MINIMUM_COST_EDGE,
	REMOVE_DC_MINIMUM_DIAMETER_EDGE,
	REMOVE_DC_SEARCH_MINIMUM_DIAMETER_EDGE,
	
	// bounded diameter
	REMOVE_BD_MINIMUM_COST_EDGE,
	REMOVE_BD_SEARCH_MINIMUM_COST_EDGE,
	REMOVE_BD_MINIMUM_DIAMETER_EDGE,
	REMOVE_BD_SEARCH_MINIMUM_DIAMETER_EDGE,

	// bounded radius heuristics
	REMOVE_BR_MINIMUM_COST_EDGE,
	REMOVE_BR_SEARCH_MINIMUM_COST_EDGE,
	REMOVE_BR_MINIMUM_DIAMETER_EDGE,
	REMOVE_BR_SEARCH_MINIMUM_DIAMETER_EDGE,
	
	// subgraph	
	REMOVE_SUBGRAPH,

	// -- unused --
	REMOVE_MINIMUM_DIAMETER_BEST_NEIGHBORHOOD_EDGE,	
	REMOVE_BEST_NEIGHBORHOOD_EDGE,
	//INSERT_MULTIPLE_CENTER_STEINER_NODE,
	
	END_DYNAMIC_TREE_ALGO
};

std::ostream& operator<<(std::ostream& ostr, const DynamicTreeAlgo& treeAlgo);
const char* printAlgo(DynamicTreeAlgo treeAlgo);
DynamicTreeAlgo getDynamicTreeAlgo(const char *algo);

/*-----------------------------------------------------------------------
		ReconfigAlgo - Supported Algorithms in the simulator
------------------------------------------------------------------------- */
enum ReconfigAlgo {
	RECONFIG_ALGO_ERROR = 0,
	NO_RECONFIG_ALGO,

	TEST_OPTIMAL_TOTAL,	
	TEST_OPTIMAL_PAIR_WISE,
	K_TOTAL_RECONFIG,

	END_RECONFIG_ALGO
};
std::ostream& operator<<(std::ostream& ostr, const ReconfigAlgo& algo);
const char *printAlgo(ReconfigAlgo algo);
ReconfigAlgo getReconfigAlgo(const char *algo);

/*-----------------------------------------------------------------------
				Distributed Tree algorithms
------------------------------------------------------------------------- */
enum DistributedTreeAlgo {
	DISTRIBUTED_TREE_ALGO_ERROR = 0,

	NO_DISTRIBUTED_TREE_ALGO,
	DISTRIBUTED_MINIMUM_SPANNING_TREE,
	DISTRIBUTED_MINIMUM_DIAMETER_SPANNING_TREE,

	END_DISTRIBUTED_TREE_ALGO
};

const char *printAlgo(DistributedTreeAlgo type);
DistributedTreeAlgo getDistributedTreeAlgo(const char* type);
std::ostream& operator<<(std::ostream& ostr, const DistributedTreeAlgo& type);

/*-----------------------------------------------------------------------
				Distributed Dynamic Tree algorithms
------------------------------------------------------------------------- */
enum DistributedDynamicTreeAlgo {
	DISTRIBUTED_DYNAMIC_TREE_ALGO_ERROR = 0,
	NO_DISTRIBUTED_DYNAMIC_TREE_ALGO,
	
	// insert
	DISTRIBUTED_DYNAMIC_INSERT_MINIMUM_DIAMETER,
	
	// remove
	DISTRIBUTED_DYNAMIC_REMOVE_MINIMUM_DIAMETER,

	END_DISTRIBUTED_DYNAMIC_TREE_ALGO
};

const char *printAlgo(DistributedDynamicTreeAlgo type);
DistributedDynamicTreeAlgo getDistributedDynamicTreeAlgo(const char* type);
std::ostream& operator<<(std::ostream& ostr, const DistributedDynamicTreeAlgo& type);


}; // namespace TreeAlgorithms

namespace GraphAlgorithms
{
/*-----------------------------------------------------------------------
		GraphAlgo - Supported Algorithms in the simulator
------------------------------------------------------------------------- */

enum GraphAlgo 
{
	GRAPH_ALGO_ERROR = 0,
	NO_GRAPH_ALGO,
	
	// complete graph algorithms
	COMPLETE_GRAPH,
	COMPLETE_MEMBER_GRAPH,
	COMPLETE_MEMBER_GRAPH_NEW_STEINER,
	COMPLETE_MEMBER_GRAPH_KEEP_STEINER,

	// edge pruning algorithms
	K_BEST_LINKS,
	ADD_CORE_LINKS,
	ADD_CORE_LINKS_OPTIMIZED,
	ADD_CORE_LINKS_OPTIMIZED_DEGREE_LIMITED,
	DEGREE_LIMITED_BEST_LINKS,

	// kTrees algorithms
	K_MINIMUM_SPANNING_TREE,
	K_DEGREE_LIMITED_MINIMUM_SPANNING_TREE,
	K_SHORTEST_PATH_TREE,
	K_DEGREE_LIMITED_SHORTEST_PATH_TREE,
	K_MINIMUM_DIAMETER_OTTC_TREE,
	K_MINIMUM_DIAMETER_DEGREE_LIMITED_OTTC_TREE,
	K_DEGREE_LIMITED_RGH_TREE,
	
	// kParallelTrees algorithms
	KP_DEGREE_LIMITED_MINIMUM_SPANNING_TREE,
	KP_DEGREE_LIMITED_SHORTEST_PATH_TREE,
	KP_MINIMUM_DIAMETER_DEGREE_LIMITED_OTTC_TREE,
	KP_DEGREE_LIMITED_RGH_TREE,

	// kCombined algorithms
	K_COMBINED_DLMST_DLRGH,
	K_COMBINED_DLMST_MDDLOTTC,
	KP_COMBINED_DLMST_DLRGH,
	KP_COMBINED_DLMST_MDDLOTTC,
	
	// enhanced tree algorithms
	ADD_LONG_LINKS_TO_DLMST,
	ADD_LONG_LINKS_TO_DLSPT,
	ADD_LONG_LINKS_TO_MDDL_OTTC,
	ADD_LONG_LINKS_TO_DL_RGH,
	
	ADD_CORE_LINKS_TO_DLMST,
	ADD_CORE_LINKS_TO_DLSPT,
	ADD_CORE_LINKS_TO_MDDL_OTTC,
	ADD_CORE_LINKS_TO_DL_RGH,

	REDUCE_DIAMETER_DLMST,
	REDUCE_DIAMETER_DLSPT,
	REDUCE_DIAMETER_MDDL_OTTC,
	REDUCE_DIAMETER_DL_RGH,
	
	REDUCE_ECCENTRICITY_DLMST,
	REDUCE_ECCENTRICITY_DLSPT,
	REDUCE_ECCENTRICITY_MDDL_OTTC,
	REDUCE_ECCENTRICITY_DL_RGH,
	
	END_GRAPH_ALGO
};

std::ostream& operator<<(std::ostream& ostr, const GraphAlgo &algo);
const char *printAlgo(GraphAlgo algo);
GraphAlgo getGraphAlgo(const char *algo);
/*-----------------------------------------------------------------------
		DynamicMeshAlgo - Supported Algorithms in the simulator
------------------------------------------------------------------------- */
enum DynamicMeshAlgo { 
	DYNAMIC_MESH_ALGO_ERROR = 0,
	NO_DYNAMIC_MESH_ALGO,
	
	// insert
	K_INSERT_MC,
	K_INSERT_MDDL,
	K_INSERT_MC_MDDL,

	// remove
	K_REMOVE_MC,
	K_REMOVE_MDDL,
	K_REMOVE_TRY_REPLACE_MC_NAIVE,
	K_REMOVE_TRY_REPLACE_MDDL_NAIVE,
	K_REMOVE_TRY_REPLACE_PRUNE_MC,
	K_REMOVE_TRY_REPLACE_PRUNE_MDDL,

	END_DYNAMIC_MESH_ALGO
};

std::ostream& operator<<(std::ostream& ostr, const DynamicMeshAlgo& meshAlgo);
const char* printAlgo(DynamicMeshAlgo treeAlgo);
DynamicMeshAlgo getDynamicMeshAlgo(const char *algo);


/*-----------------------------------------------------------------------
				Core Selection Algorithms
------------------------------------------------------------------------- */
enum CoreSelectionAlgo
{
	CORE_SELECTION_ALGO_ERROR = 0,
	NO_CORE_SELECTION_ALGO,
	
	MEDIAN_DISTANCE,
	AVERAGE_DISTANCE,					// K_MEDIAN
	CENTER_SELECTION,					// K_CENTER
	WALK_SELECTION,						// K_BROADCAST_WALK
	UNICAST_WALK_SELECTION,				// K_UNICAST_WALK
	WALK_SELECTION_GROUP_TREE,			// K_BROADCAST_WALK_GROUP_TREE
	UNICAST_WALK_SELECTION_GROUP_TREE,	// K_UNICAST_WALK_GROUP_TREE
	UNIFORM_SELECTION_LAYOUT,			// NOT WORKING
	TAILED_SELECTION,					// K_TAILED
	BANDWIDTH_SELECTION,		
	RANDOM_SELECTION,
	WORST_SELECTION,

	END_CORE_SELECTION_ALGO
};

std::ostream& operator<<(std::ostream& ostr, const CoreSelectionAlgo &algo);
const char* printAlgo(CoreSelectionAlgo algo);
CoreSelectionAlgo getCoreSelectionAlgo(const char *algo);
}; // namespace GraphAlgorithms

/*-----------------------------------------------------------------------
				GlobalSimArgs - Global arguments
------------------------------------------------------------------------- */

//class GroupInformation;

class GlobalSimArgs {

public:
	GlobalSimArgs() {} 
	~GlobalSimArgs() {}

public: // inline functions
	
	inline static GraphAlgorithms::GraphAlgo 			getSimPruneAlgo() 				{ return simPruneAlgo_;}
	inline static GraphAlgorithms::GraphAlgo 			getSimGraphAlgo() 				{ return simGraphAlgo_;}
	inline static GraphAlgorithms::GraphAlgo			meshAlgo()						{ return meshAlgo_; }
	inline static GraphAlgorithms::CoreSelectionAlgo 	getSimWCNSetAlgo()				{ return simSPSetAlgo_;}
	inline static GraphAlgorithms::CoreSelectionAlgo 	getSimWCNAlgo()					{ return simSPAlgo_;}
	inline static TreeAlgorithms::TreeAlgo 				getSimTreeAlgo() 				{ return simTreeAlgo_;}
	inline static TreeAlgorithms::DynamicTreeAlgo 		insertTreeAlgo() 				{ return simInsertAlgo_; }
	inline static TreeAlgorithms::DynamicTreeAlgo 		insertTreeBackupAlgo() 			{ return simInsertBackupAlgo_; }
	inline static TreeAlgorithms::DynamicTreeAlgo 		removeTreeAlgo() 				{ return simRemoveAlgo_; }
	inline static TreeAlgorithms::DynamicTreeAlgo 		removeTreeBackupAlgo()			{ return simRemoveBackupAlgo_; }
	
	inline static GraphAlgorithms::DynamicMeshAlgo 		insertMeshAlgo() 				{ return simInsertMeshAlgo_; }
	inline static GraphAlgorithms::DynamicMeshAlgo 		insertBackupMeshAlgo()			{ return simInsertBackupMeshAlgo_; }
	inline static GraphAlgorithms::DynamicMeshAlgo 		removeMeshAlgo() 				{ return simRemoveMeshAlgo_; }
	inline static GraphAlgorithms::DynamicMeshAlgo 		removeBackupMeshAlgo()			{ return simRemoveBackupMeshAlgo_; }

	inline static TreeAlgorithms::ReconfigAlgo 			getSimReconfigAlgo() 			{ return simReconfigAlgo_; }
	inline static TreeAlgorithms::DistributedTreeAlgo	distributedTreeAlgo()			{ return distributedTreeAlgo_; }
	inline static TreeAlgorithms::DistributedDynamicTreeAlgo	distributedInsertAlgo()	{ return distributedInsertTreeAlgo_; }
	inline static TreeAlgorithms::DistributedDynamicTreeAlgo	distributedRemoveAlgo()	{ return distributedRemoveTreeAlgo_; }

	inline static double							getSimReconfigThreshold() { return simReconfigThreshold_; }
	inline static int								getSimReconfigInterval() { return simReconfigInterval_; }
	inline static const char* 						getSimStatFile() 		{ return simStatFile_;}
	inline static const char* 						getSimGraphFile()		{ return simGraphFile_;}
	inline static const char* 						getSimActionFile()		{ return simActionFile_;}

	inline static const char* 						getSimAbortLog() 		{ return simAbortLog_;}
	inline static int 								getSteinerSetSize() 	{ return numOfSteiner_; }
	inline static int								getSteinerMemberSize()  { return numOfSteinerInput_; }
	inline static double 							getSteinerMemberRatio() { return steinerMemberRatio_; }
	inline static double							getSteinerSetGraphRatio() { return steinerSetGraphRatio_; }
	inline static VertexSet 						getTestGroupSet() 		{ return testGroupSet_; }
	inline static bool 								hasTestGroup() 			{ return testGroup_; }
	inline static double 							getDelayConstraint() 	{ return delayConstraint_; }
	inline static int								getDegreeConstraint()	{ return degreeConstraint_; }
	inline static int								getDegreeConstraintSP()	{ return degreeConstraintSP_; }
	inline static double							getDiameterBound()		{ return diameterBound_; }
	inline static double							getRelaxDelay()			{ return relaxDelay_; }
	inline static int								getRelaxDegree()		{ return relaxDegree_; }
	inline static double							getRelaxDiameter()		{ return relaxDiameter_; }
	inline static ServerType 						getServerType() 		{ return simServerType_; }
	inline static ClientType 						getClientType() 		{ return simClientType_; }
	inline static ProxyType 						getProxyType() 			{ return simProxyType_; }
	
	inline static iostream& 						statbuffer()			{ return simostream_; } 
	inline static ofstream&							simofstream()			{ return simofstream_; }
	inline static int								getPruneParamK()		{ return pruneParamK_; }
	inline static int								meshParamK()			{ return meshParamK_; }

	inline static int								getNodeId()				{ return nodeId_; }
	
	inline static void 			setSimPruneAlgo(GraphAlgorithms::GraphAlgo algo) 					{ simPruneAlgo_ = algo; }
	inline static void 			setSimGraphAlgo(GraphAlgorithms::GraphAlgo algo) 					{ simGraphAlgo_ = algo; }
	inline static void			meshAlgo(GraphAlgorithms::GraphAlgo algo)							{ meshAlgo_ = algo; }
	inline static void			setSimWCNSetAlgo(GraphAlgorithms::CoreSelectionAlgo algo) 			{ simSPSetAlgo_ = algo; }
	inline static void			setSimWCNAlgo(GraphAlgorithms::CoreSelectionAlgo algo) 				{ simSPAlgo_ = algo; }

	inline static void 			setSimTreeAlgo(TreeAlgorithms::TreeAlgo algo) 						{ simTreeAlgo_ = algo;}
	inline static void			insertTreeAlgo(TreeAlgorithms::DynamicTreeAlgo algo) 				{ simInsertAlgo_ = algo; }
	inline static void			insertTreeBackupAlgo(TreeAlgorithms::DynamicTreeAlgo algo) 			{ simInsertBackupAlgo_ = algo; }
	inline static void			removeTreeAlgo(TreeAlgorithms::DynamicTreeAlgo algo) 				{ simRemoveAlgo_ = algo; }
	inline static void			removeTreeBackupAlgo(TreeAlgorithms::DynamicTreeAlgo algo) 			{ simRemoveBackupAlgo_ = algo; }
	inline static void			setSimReconfigAlgo(TreeAlgorithms::ReconfigAlgo algo) 				{ simReconfigAlgo_ = algo; }

	inline static void			insertMeshAlgo(GraphAlgorithms::DynamicMeshAlgo algo) 			{ simInsertMeshAlgo_ = algo; }
	inline static void			insertBackupMeshAlgo(GraphAlgorithms::DynamicMeshAlgo algo) 	{ simInsertBackupMeshAlgo_ = algo; }
	inline static void			removeMeshAlgo(GraphAlgorithms::DynamicMeshAlgo algo) 			{ simRemoveMeshAlgo_ = algo; }
	inline static void			removeBackupMeshAlgo(GraphAlgorithms::DynamicMeshAlgo algo) 	{ simRemoveBackupMeshAlgo_ = algo; }

	inline static void			distributedTreeAlgo(TreeAlgorithms::DistributedTreeAlgo i)			{ distributedTreeAlgo_ = i; }
	inline static void			distributedInsertAlgo(TreeAlgorithms::DistributedDynamicTreeAlgo i)	{ distributedInsertTreeAlgo_ = i; }
	inline static void			distributedRemoveAlgo(TreeAlgorithms::DistributedDynamicTreeAlgo i)	{ distributedRemoveTreeAlgo_ = i; }

	
	inline static void			setSimReconfigThreshold(double thres) 			{ simReconfigThreshold_ = thres; }
	inline static void			setSimReconfigInterval(int val) 				{ simReconfigInterval_ = val; }
	inline static void 	        setSimStatFile(const char* stat) 				{ simStatFile_ = stat;}
	inline static void	        setSimGraphFile(const char* file)				{ simGraphFile_  = file;}
	inline static void	        setSimActionFile(const char* file)				{ simActionFile_  = file;}
	inline static void 	        setSimAbortLog(const char* stat) 				{ simAbortLog_ = stat;}
	inline static void 			setSteinerSetSize(int num) 						{ numOfSteiner_ = num; }
	inline static void			setSteinerMemberSize(int num)  					{ numOfSteinerInput_ = num; }
	inline static void			setSteinerSetGraphRatio(double in)				{ steinerSetGraphRatio_ = in; }
	inline static void 			setSteinerMemberRatio(double num) 				{ steinerMemberRatio_ = num; }
	inline static void 			setTestGroupSet(VertexSet s) 					{ testGroupSet_ = s; }
	inline static void 			setHasTestGroup(bool b) 						{ testGroup_ = b; }
	inline static void 			setDelayConstraint(double d) 					{ delayConstraint_ = d; }
	inline static void			setDegreeConstraint(int d)						{ degreeConstraint_ = d; }
	inline static void			setDegreeConstraintSP(int d)					{ degreeConstraintSP_ = d; }
	inline static void			setRelaxDelay(double i)							{ relaxDelay_ = i; }
	inline static void			setRelaxDegree(int i)							{ relaxDegree_ = i; }
	inline static void			setRelaxDiameter(double i)						{ relaxDiameter_ = i; }
	inline static void			setDiameterBound(double d)						{ diameterBound_ = d; }
	inline static void			setPruneParamK(int k)							{ pruneParamK_ = k; }
	inline static void			meshParamK(int k)								{ meshParamK_ = k; }

	inline static void 			setServerType(ServerType s) 					{ simServerType_ = s; }
	inline static void 			setClientType(ClientType s) 					{ simClientType_ = s; }
	inline static void 			setProxyType(ProxyType s) 						{ simProxyType_ = s; }
	
	inline static void			setSimOstream(streambuf* ostr)					{ simostream_.rdbuf(ostr); }
	inline static void			setSimOstream(const char* simfile)				{ simofstream_.open(simfile, ios::out | ios::app); }
	inline static void			setSimOstream(ostream& ostr)					{ simostream_.rdbuf(ostr.rdbuf()); }
	
	//inline static void			setGroup(GroupInformation &g)					{ currGroup_ = &g; }
	
	inline static void			setNodeId(int i)								{ nodeId_ = i; }
public:
	// -- debug --
	static double 	maxAvMedDiffdist_;
	static double 	maxAvMedDiffcost_;
	static double 	maxEdgeDelay_;
	static double 	maxEdgeCost_;
	static double 	maxGroupSize_;
	static VertexSet invalidVertices_;
	// -- debug --

	// statistic streams
	static iostream  simostream_;
	static stringstream simStatBuffer_;
	static ofstream	 simofstream_;
	
	//static GroupInformation *currGroup_;
	
private:
	static ServerType 	simServerType_;
	static ClientType 	simClientType_;
	static ProxyType  	simProxyType_;

	static GraphAlgorithms::GraphAlgo 			simGraphAlgo_;
	static GraphAlgorithms::GraphAlgo			simPruneAlgo_;
	static GraphAlgorithms::GraphAlgo			meshAlgo_;
	static GraphAlgorithms::CoreSelectionAlgo	simSPSetAlgo_;
	static GraphAlgorithms::CoreSelectionAlgo	simSPAlgo_;

	static TreeAlgorithms::TreeAlgo 			simTreeAlgo_;
	static TreeAlgorithms::DynamicTreeAlgo		simInsertAlgo_;
	static TreeAlgorithms::DynamicTreeAlgo  	simRemoveAlgo_;
	static TreeAlgorithms::DynamicTreeAlgo		simInsertBackupAlgo_;
	static TreeAlgorithms::DynamicTreeAlgo  	simRemoveBackupAlgo_;
	
	static GraphAlgorithms::DynamicMeshAlgo		simInsertMeshAlgo_;
	static GraphAlgorithms::DynamicMeshAlgo  	simRemoveMeshAlgo_;
	static GraphAlgorithms::DynamicMeshAlgo		simInsertBackupMeshAlgo_;
	static GraphAlgorithms::DynamicMeshAlgo  	simRemoveBackupMeshAlgo_;

	static TreeAlgorithms::ReconfigAlgo  		simReconfigAlgo_;
	static TreeAlgorithms::DistributedTreeAlgo  distributedTreeAlgo_;
	static TreeAlgorithms::DistributedDynamicTreeAlgo  distributedInsertTreeAlgo_;
	static TreeAlgorithms::DistributedDynamicTreeAlgo  distributedRemoveTreeAlgo_;

	static double 		simReconfigThreshold_;
	static int			simReconfigInterval_;	
	static const char* 	simStatFile_;
	static const char*  simGraphFile_;
	static const char*  simActionFile_;
	static const char* 	simAbortLog_;
	static int 			numOfSteiner_;
	static int  		numOfSteinerInput_;
	static double 		steinerMemberRatio_;
	static double		steinerSetGraphRatio_;
	static double 		delayConstraint_;
	static int			degreeConstraint_;
	static int			degreeConstraintSP_;
	static double		relaxDelay_;
	static int			relaxDegree_;
	static double		relaxDiameter_;
	static int			pruneParamK_;
	static int			meshParamK_;
	static double		diameterBound_;

	static int 			dynamicsThreshold_;

	static VertexSet 	testGroupSet_;
	static bool 		testGroup_;
	static int			nodeId_;
};

/*-----------------------------------------------------------------------
			global defines: Assertions, logging etc.
------------------------------------------------------------------------- */
//#ifdef WIN32
//typedef unsigned long clock_t;
//#endif

#ifndef WRITE_FUNCTION
#define WRITE_FUNCTION GlobalSimArgs::getNodeId() << ": [" << __FUNCTION__ << "," << __LINE__ << "]"
#endif

#ifndef WRITE_PRETTY_FUNCTION
    #ifdef USE_GCC
        #define WRITE_PRETTY_FUNCTION GlobalSimArgs::getNodeId() << ": [" << __PRETTY_FUNCTION__ << "," << __LINE__ << "]"
    #elif WIN32
        #define WRITE_PRETTY_FUNCTION GlobalSimArgs::getNodeId() << ": [" << __FUNCSIG__ << "," << __LINE__ << "]"
    #endif
#endif

#define MAXIMUM_WEIGHT	(std::numeric_limits<double>::max)()
#define NO_SOURCE_NODE	100000

#ifdef MACX
// no push-pop support
#elif USE_GCC
#define SYSTEM_SUPPORTS_PUSH_POP 1
#endif

#define statout  GlobalSimArgs::statbuffer()
#define stattofile GlobalSimArgs::simofstream()

void logSimArgs(ostream&);
void writeSimArgs(ostream&);
bool isSteinerAlgo(const TreeAlgorithms::TreeAlgo &algo);

#ifndef ASSERT
#ifdef USE_GCC
    #define ASSERT(test) \
        ( (test) ? (void)0\
            : (ofstream(GlobalSimArgs::getSimAbortLog(), ios::out | ios::app) << " ----------------------------------------------- " << endl << "Assertion Failed: " << #test << endl << "Location: " << __FILE__ << "," << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl, \
            cerr << "Assertion Failed: " << #test << endl << "Location: " << __FILE__ << "," << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl, abort()))
#elif WIN32
    #define ASSERT(test) \
        ( (test) ? (void)0\
            : (ofstream(GlobalSimArgs::getSimAbortLog(), ios::out | ios::app) << " ----------------------------------------------- " << endl << "Assertion Failed: " << #test << endl << "Location: " << __FILE__ << "," << __LINE__ << ":" << __FUNCSIG__ << endl, \
            cerr << "Assertion Failed: " << #test << endl << "Location: " << __FILE__ << "," << __LINE__ << ":" << __FUNCSIG__ << endl, abort()))
    #endif
#endif

//#undef NDEBUG	
#ifndef NDEBUG
#	define ASSERTING(test) ASSERT(test)
#else
#	define ASSERTING(test) ((void)0)
#endif 

/*-----------------------------------------------------------------------
			unused global defines: Assertions, logging etc.
------------------------------------------------------------------------- */
/*	
#define ASSERTMSG(TST,MSG)\
( (TST) ? (void)0\
: (cerr << __FILE__ "(" << __LINE__ << "): Assertion failed " #TST << MSG << endl,abort()))
#define PRECONDITION(TST,MSG) ASSERTMSG(TST,"Precondition " << MSG)
#ifndef NDEBUG
#  define POSTCONDITION(TST,MSG) ASSERTMSG(TST,"Postcondition " << MSG)
#  define INVARIANT(TST,MSG) ASSERTMSG(TST,"Invariant " << MSG)
#else
#  define POSTCONDITION(TST,MSG) ((void)0)
#  define INVARIANT(TST,MSG) ((void)0)
#endif
										   
#define ASSERT(test) \
	( (test) ? (void)0\
		: (ofstream(GlobalSimArgs::getSimAbortLog(), ios::out | ios::app) << "Assertion Failed: " << __FILE__ << "," << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl \
		<< "         " << SimTime::Now() << ": " << #test << endl \
		<< "Simulation Arguments: " << GlobalSimArgs::getSimTreeAlgo() << " " << GlobalSimArgs::getSimGraphAlgo() << " " << GlobalSimArgs::getSimPruneAlgo() << endl \
		<< " " << GlobalSimArgs::getSimInsertAlgo() << " " << GlobalSimArgs::getSimRemoveAlgo() << " " << GlobalSimArgs::getSimReconfigAlgo() << endl, \
		std::cerr << "Assertion Failed: " << __FILE__ << "," << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl \
		<< "         " << SimTime::Now() << ": " << #test << endl \
		<< "Simulation Arguments: " << GlobalSimArgs::getSimTreeAlgo() << " " << GlobalSimArgs::getSimGraphAlgo() << " " << GlobalSimArgs::getSimPruneAlgo() << endl \
		<< " " << GlobalSimArgs::getSimInsertAlgo() << " " << GlobalSimArgs::getSimRemoveAlgo() << " " << GlobalSimArgs::getSimReconfigAlgo() << endl, abort()))

#define ASSERT2(test) \
	( (test) ? (void)0\
		: (ofstream(GlobalSimArgs::getSimAbortLog(), ios::out | ios::app) << "Assertion Failed: " << __FILE__ << "," << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl \
		<< "         " << SimTime::Now() << ": " << #test << endl, logSimArgs((ofstream(GlobalSimArgs::getSimAbortLog(), ios::out | ios::app)), \
		std::cerr << "Assertion Failed: " << __FILE__ << "," << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl \
		<< "         " << SimTime::Now() << ": " << #test << endl, logSimArgs(cerr), abort()))

#define ASSERTING(TST)\
( (TST) ? (void)0\
: (cerr << __FILE__ "(" << __LINE__ << "): Assertion failed " #TST << endl,abort()))
*/ 

#endif // GRAPHALGO_SIM_DEFINITIONS_KHV

