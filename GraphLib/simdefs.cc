/***************************************************************************
                          simdefs.h  -  description
                             -------------------
    begin                : Fri Apr 28 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "simdefs.h"
//#include "./network/group_info.h"
//#include "./treealgs/treealgs.h"
//#include "./graphalgs/graphalgs.h"
#include "./simtime.h"
#include <fstream>
#include <string.h>

using namespace std;
//using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*---------------------------------------------------------------------------------
						initialize static members
---------------------------------------------------------------------------------*/
ServerType 	GlobalSimArgs::simServerType_ = NO_SERVER_TYPE;
ClientType 	GlobalSimArgs::simClientType_ = NO_CLIENT_TYPE;
ProxyType 	GlobalSimArgs::simProxyType_ = NO_PROXY_TYPE;

GraphAlgo 	GlobalSimArgs::simGraphAlgo_ = NO_GRAPH_ALGO;
GraphAlgo 	GlobalSimArgs::simPruneAlgo_ = NO_GRAPH_ALGO;
GraphAlgo	GlobalSimArgs::meshAlgo_ = NO_GRAPH_ALGO;
CoreSelectionAlgo GlobalSimArgs::simSPSetAlgo_ = TAILED_SELECTION;
CoreSelectionAlgo GlobalSimArgs::simSPAlgo_ = TAILED_SELECTION;

TreeAlgo 			GlobalSimArgs::simTreeAlgo_ = NO_TREE_ALGO;
DynamicTreeAlgo 	GlobalSimArgs::simInsertAlgo_ = NO_DYNAMIC_TREE_ALGO;
DynamicTreeAlgo 	GlobalSimArgs::simRemoveAlgo_ = NO_DYNAMIC_TREE_ALGO;
DynamicTreeAlgo		GlobalSimArgs::simInsertBackupAlgo_ = INSERT_MINIMUM_COST_EDGE;
DynamicTreeAlgo 	GlobalSimArgs::simRemoveBackupAlgo_ = REMOVE_TRY_REPLACE_MC_NAIVE;

DynamicMeshAlgo 	GlobalSimArgs::simInsertMeshAlgo_ = NO_DYNAMIC_MESH_ALGO;
DynamicMeshAlgo 	GlobalSimArgs::simRemoveMeshAlgo_ = NO_DYNAMIC_MESH_ALGO;
DynamicMeshAlgo		GlobalSimArgs::simInsertBackupMeshAlgo_ = NO_DYNAMIC_MESH_ALGO;
DynamicMeshAlgo 	GlobalSimArgs::simRemoveBackupMeshAlgo_ = NO_DYNAMIC_MESH_ALGO;

ReconfigAlgo 		GlobalSimArgs::simReconfigAlgo_  = NO_RECONFIG_ALGO;
DistributedTreeAlgo GlobalSimArgs::distributedTreeAlgo_ = NO_DISTRIBUTED_TREE_ALGO;
DistributedDynamicTreeAlgo GlobalSimArgs::distributedInsertTreeAlgo_ = NO_DISTRIBUTED_DYNAMIC_TREE_ALGO;
DistributedDynamicTreeAlgo GlobalSimArgs::distributedRemoveTreeAlgo_ = NO_DISTRIBUTED_DYNAMIC_TREE_ALGO;

int			GlobalSimArgs::simReconfigInterval_   = (numeric_limits<int>::max)();
double		GlobalSimArgs::simReconfigThreshold_ = (numeric_limits<double>::max)();
const char* GlobalSimArgs::simStatFile_ = NULL;
const char* GlobalSimArgs::simGraphFile_ = NULL;
const char* GlobalSimArgs::simActionFile_ = NULL;
VertexSet	GlobalSimArgs::invalidVertices_ = VertexSet();

int			GlobalSimArgs::nodeId_ = -1;
int 		GlobalSimArgs::pruneParamK_ = 2;
int 		GlobalSimArgs::meshParamK_ = 2;
int 		GlobalSimArgs::numOfSteiner_ = 0;
int			GlobalSimArgs::numOfSteinerInput_ = 0;
double		GlobalSimArgs::steinerSetGraphRatio_ = 0;
double 		GlobalSimArgs::steinerMemberRatio_ = 0;
double 		GlobalSimArgs::delayConstraint_ = (numeric_limits<double>::max)();
int			GlobalSimArgs::degreeConstraint_ = (numeric_limits<int>::max)();
int			GlobalSimArgs::degreeConstraintSP_ = (numeric_limits<int>::max)();
double		GlobalSimArgs::diameterBound_ = (numeric_limits<double>::max)();
double		GlobalSimArgs::relaxDelay_ = 0.1;
int			GlobalSimArgs::relaxDegree_	= 1;
double 		GlobalSimArgs::relaxDiameter_ = 0.1;
const char* GlobalSimArgs::simAbortLog_ = "abort_log.txt";
VertexSet 	GlobalSimArgs::testGroupSet_ = VertexSet(); 
bool 		GlobalSimArgs::testGroup_ = false;
iostream	GlobalSimArgs::simostream_(cout.rdbuf()); 
stringstream GlobalSimArgs::simStatBuffer_(stringstream::in | stringstream::out | stringstream::app); 
ofstream	GlobalSimArgs::simofstream_("");

//GroupInformation *GlobalSimArgs::currGroup_ = NULL;

// debug and stats
double GlobalSimArgs::maxAvMedDiffdist_ = (numeric_limits<double>::max)();
double GlobalSimArgs::maxAvMedDiffcost_ = (numeric_limits<double>::max)();
double GlobalSimArgs::maxEdgeDelay_ = (numeric_limits<double>::max)();
double GlobalSimArgs::maxEdgeCost_ = (numeric_limits<double>::max)();
double GlobalSimArgs::maxGroupSize_ = (numeric_limits<double>::max)();

/*---------------------------------------------------------------------------------
					writing simulation args to ostream 
---------------------------------------------------------------------------------*/
void logSimArgs(ostream& ostr)
{
	ostr << "# Time: " << SimTime::Now() << " Simulation Arguments: " << endl
		 << "# Server Type                 " << GlobalSimArgs::getServerType() << endl
		 << "# Client Type                 " << GlobalSimArgs::getClientType() << endl
		 << "# Proxy Type                  " << GlobalSimArgs::getProxyType() << endl
		 << "# Tree Algorithm              " << GlobalSimArgs::getSimTreeAlgo() << endl
		 << "# Distributed Tree Algorithm  " << GlobalSimArgs::distributedTreeAlgo() << endl
		 << "# Distr Dynamic Insert Algo.  " << GlobalSimArgs::distributedInsertAlgo() << endl
		 << "# Distr Dynamic Remove Algo.  " << GlobalSimArgs::distributedRemoveAlgo() << endl
		 << "# Mesh Algorithm              " << GlobalSimArgs::meshAlgo() << endl
		 << "# Graph Algorithm             " << GlobalSimArgs::getSimGraphAlgo() << endl
		 << "# Prune Algorithm             " << GlobalSimArgs::getSimPruneAlgo() << endl
		 << "# Prune Param k               " << GlobalSimArgs::getPruneParamK() << endl
		 << "# Mesh Param k                " << GlobalSimArgs::meshParamK() << endl
		 << "# WCN Algorithm               " << GlobalSimArgs::getSimWCNAlgo() << endl 
		 << "# WCN Set Algorithm           " << GlobalSimArgs::getSimWCNSetAlgo() << endl 
		 << "# Insert Tree Algorithm       " << GlobalSimArgs::insertTreeAlgo() << endl
		 << "# Remove Tree Algorithm       " << GlobalSimArgs::removeTreeAlgo() << endl
		 << "# Insert Mesh Algorithm       " << GlobalSimArgs::insertMeshAlgo() << endl
		 << "# Remove Mesh Algorithm       " << GlobalSimArgs::removeMeshAlgo() << endl
		 << "# Reconfig Algorithm          " << GlobalSimArgs::getSimReconfigAlgo() << endl
		 << "# Reconfig Interval           " << GlobalSimArgs::getSimReconfigInterval() << endl
		 << "# Reconfig Threshold          " << GlobalSimArgs::getSimReconfigThreshold() << endl
		 << "# Delay Constraint            " << GlobalSimArgs::getDelayConstraint() << endl
		 << "# Degree Constraint GM        " << GlobalSimArgs::getDegreeConstraint() << endl
		 << "# Degree Constraint SP        " << GlobalSimArgs::getDegreeConstraintSP() << endl
		 << "# Diameter Bound              " << GlobalSimArgs::getDiameterBound() << endl
		 << "# Core Size                   " << GlobalSimArgs::getSteinerMemberSize() << endl
		 << "# Steiner Ratio Input Graph   " << GlobalSimArgs::getSteinerMemberRatio() << endl
		 << "# Steiner Set Size            " << GlobalSimArgs::getSteinerSetSize() << endl
		 << "# Statistics File             " << GlobalSimArgs::getSimStatFile() << endl
		 << "# Graph File                  " << GlobalSimArgs::getSimGraphFile() << endl
		 << "# Action File                 " << GlobalSimArgs::getSimActionFile() << endl
		 << "# Relax Delay                 " << GlobalSimArgs::getRelaxDelay() << endl
	 	 << "# Relax Degree                " << GlobalSimArgs::getRelaxDegree() << endl
		 << "# Relax Diameter              " << GlobalSimArgs::getRelaxDiameter() << endl;
	 
	if(GlobalSimArgs::hasTestGroup())
		ostr << "# Test Group set              " << GlobalSimArgs::getTestGroupSet() << endl;

	//if(GlobalSimArgs::currGroup_ != NULL)
	//{
	//	ostr << "Last group Information: " << endl;
		//GlobalSimArgs::currGroup_->dumpAll(ostr);
	//}
	//ostr.flush();
}
/*---------------------------------------------------------------------------------
				writeGlobalArgs(): write arguments to stat file
---------------------------------------------------------------------------------*/
void writeSimArgs(ostream& ostr)
{
	if(!GlobalSimArgs::getSimStatFile()) 	
	{
		GlobalSimArgs::setSimOstream(cout);
	}
	else // TODO
	{
		GlobalSimArgs::setSimOstream(GlobalSimArgs::simStatBuffer_.rdbuf());
		GlobalSimArgs::setSimOstream(GlobalSimArgs::getSimStatFile());
	}

	ostr << "# Simulation Start " << endl;
	ostr << "-Arguments " ;
	ostr << " -ServerType " << printAlgo(GlobalSimArgs::getServerType());
	ostr << " -ClientType " << printAlgo(GlobalSimArgs::getClientType());
	ostr << " -ProxyType " << printAlgo(GlobalSimArgs::getProxyType());
	ostr << " -TreeAlgo " << printAlgo(GlobalSimArgs::getSimTreeAlgo()) ;
	ostr << " -DistributedInsertAlgo " << printAlgo(GlobalSimArgs::distributedInsertAlgo()) ;
	ostr << " -DistributedRemoveAlgo " << printAlgo(GlobalSimArgs::distributedRemoveAlgo()) ;
	ostr << " -DistributedTreeAlgo " << printAlgo(GlobalSimArgs::distributedTreeAlgo()) ;
	ostr << " -MeshAlgo " << printAlgo(GlobalSimArgs::meshAlgo()) ;
	ostr << " -GraphAlgo " << printAlgo(GlobalSimArgs::getSimGraphAlgo()) ;
	ostr << " -PruneAlgo " << printAlgo(GlobalSimArgs::getSimPruneAlgo()) ;
	ostr << " -PruneParamk " << GlobalSimArgs::getPruneParamK() ;
	ostr << " -MeshParamk " << GlobalSimArgs::meshParamK() ;
	ostr << " -InsertAlgo " << printAlgo(GlobalSimArgs::insertTreeAlgo()) ;
	ostr << " -RemoveAlgo " << printAlgo(GlobalSimArgs::removeTreeAlgo()) ;
	ostr << " -InsertMeshAlgo " << printAlgo(GlobalSimArgs::insertMeshAlgo()) ;
	ostr << " -RemoveMeshAlgo " << printAlgo(GlobalSimArgs::removeMeshAlgo()) ;
	ostr << " -ReconfigAlgo " << printAlgo(GlobalSimArgs::getSimReconfigAlgo()) ;
	ostr << " -ReconfigInterval " << GlobalSimArgs::getSimReconfigInterval() ;
	ostr << " -ReconfigThreshold " << GlobalSimArgs::getSimReconfigThreshold() ;
	ostr << " -DelayConstr " << GlobalSimArgs::getDelayConstraint() ;
	ostr << " -DegreeConstrGM " << GlobalSimArgs::getDegreeConstraint() ;
	ostr << " -DegreeConstrSP " << GlobalSimArgs::getDegreeConstraintSP() ;
	ostr << " -DiameterBound  " << GlobalSimArgs::getDiameterBound() ;
	ostr << " -WCNAlgo " << printAlgo(GlobalSimArgs::getSimWCNAlgo()) ;
	ostr << " -WCNSetAlgo " << printAlgo(GlobalSimArgs::getSimWCNSetAlgo()) ;
	ostr << " -CoreSize " << GlobalSimArgs::getSteinerMemberSize() ;
	ostr << " -SteinerRatio " << GlobalSimArgs::getSteinerMemberRatio() ;
	ostr << " -SteinerPoolSize " << GlobalSimArgs::getSteinerSetSize() ;
	ostr << " -RelaxDelay " << GlobalSimArgs::getRelaxDelay() ;
	ostr << " -RelaxDegree " << GlobalSimArgs::getRelaxDegree() ;
	ostr << " -RelaxDiameter " << GlobalSimArgs::getRelaxDiameter() ;

	ostr << endl;
	ostr.flush();
}

bool isSteinerAlgo(const TreeAlgo &algo)
{
	if( algo == MINIMUM_SPANNING_TREE ||  
		algo == DEGREE_LIMITED_MINIMUM_SPANNING_TREE ||
		algo == SHORTEST_PATH_TREE ||	  
		algo == DELAY_CONSTRAINED_SHORTEST_PATH_TREE ||
		algo == DEGREE_LIMITED_SHORTEST_PATH_TREE ||
		algo == DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_TREE ||
		algo == COMPACT_TREE ||
		algo == BOUNDED_COMPACT_TREE ||
		algo == SERVER_SHORTEST_PATH_TREE ||
		algo == PROXY_SHORTEST_PATH_TREE ||
		algo == ONE_TIME_TREE_CONSTRUCTION_BDMST ||
		algo == DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST ||
		algo == MINIMUM_DIAMETER_ONE_TIME_TREE_CONSTRUCTION ||
		algo == MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION ||
		algo == RANDOMIZED_GREEDY_HEURISTIC_BDMST ||
		algo == DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST ||
		algo == NO_TREE_ALGO)
	{
		return false;
	}
	
	return true;
}

/*-----------------------------------------------------------------------
					Server type print
------------------------------------------------------------------------- */
const char *stringServerType[] = { "error", "no_algo", "css", "csd", "css-pl", "csd-pl"};
const char *printAlgo(ServerType type)
{
	return stringServerType[type];
}

ServerType getServerAlgo(const char *algo)
{
	for( int i = 0; i < END_SERVER_TYPE; i++)
		if(strcmp(algo, stringServerType[i]) == 0) return (ServerType) i;

	return (ServerType) 0;
}

std::ostream& operator<<(std::ostream& ostr, const ServerType& serverType)
{
	switch(serverType)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(SERVER_TYPE_ERROR)
	CASE(NO_SERVER_TYPE)
	CASE(CENTRAL_SERVER_STATIC)
	CASE(CENTRAL_SERVER_DYNAMIC)
	CASE(CENTRAL_SERVER_STATIC_PLANET)
	CASE(CENTRAL_SERVER_DYNAMIC_PLANET)
#undef CASE
	default:
		cerr << "Missing operator<< case for ServerType " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}

/*-----------------------------------------------------------------------
						Client type
------------------------------------------------------------------------- */
const char *stringClientType[] = { "error", "no_algo", "ct", "ct-pl" };
const char *printAlgo(ClientType type)
{
	return stringClientType[type];
}

ClientType getClientAlgo(const char *algo)
{
	for( int i = 0; i < END_CLIENT_TYPE; i++)
		if(strcmp(algo, stringClientType[i]) == 0) return (ClientType) i;

	return (ClientType) 0;
}

std::ostream& operator<<(std::ostream& ostr, const ClientType& clientType)
{
	switch(clientType)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(CLIENT_TYPE_ERROR)
	CASE(NO_CLIENT_TYPE)
	CASE(CLIENT_TERMINAL)
	CASE(CLIENT_TERMINAL_PLANET)
#undef CASE
	default:
		cerr << "Missing operator<< case for ClientType " << endl;
		ASSERTING(0);
		break;
	}

	return ostr;
}

/*-----------------------------------------------------------------------
						Proxy type
------------------------------------------------------------------------- */
const char *stringProxyType[] = { "error", "no_algo", "pb" };
const char *printAlgo(ProxyType type)
{
	return stringProxyType[type];
}

ProxyType getProxyAlgo(const char *algo)
{
	for( int i = 0; i < END_PROXY_TYPE; i++)
		if(strcmp(algo, stringProxyType[i]) == 0) return (ProxyType) i;

	return (ProxyType) 0;
}

std::ostream& operator<<(std::ostream& ostr, const ProxyType& proxyType)
{
	switch(proxyType)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(PROXY_TYPE_ERROR)
	CASE(NO_PROXY_TYPE)
	CASE(PROXY_BASIC)
#undef CASE
	default:
		cerr << "Missing operator<< case for ProxyType " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}


namespace TreeAlgorithms
{
/*-----------------------------------------------------------------------
						TreeAlgo print
------------------------------------------------------------------------- */
const char *stringAlgo[] = { "error", "no_algo", // = 1
			// unconstrained algorithms
			"sspt", "pspt", "spt", "steiner-spt", "mst", "pmst", "sph", "dnh", "adh", "md-sph", "md-dnh", "md-ottc", // = 10
			// degree bounded algorithms   
			"dl-spt", "steiner-dl-spt", "dl-mst", "pdlmst", "ct", "pct", "dl-sph", "dl-sph-bauer", "dl-dnh", "dl-sp-dnh", "mddl-sph", "mddl-dnh", "mddl-ottc", "steiner-mddl-ottc", // = 22
			// bounded diameter
			"bdo-sph", "bdrgh-sph", "bdr-sph", "bdo-dnh", "bdr-dnh", "ottc", "rgh", // = 28
			// bounded diameter degree limited
			"bct", "bddlo-sph", "bddlr-sph", "bddlo-dnh", "bddlr-dnh", "dl-ottc", "steiner-dl-ottc", "dl-rgh", "steiner-dl-rgh", // = 35
			// radius constrained algorithms
			"br-sph", "brdl-sph", // = 37
			// delay constrained degree bounded algorithms
			"dcdl-spt", "dcdl-sp", "dcdl-sph", // =40
			// delay constrained algorithms
			"dc-spt", "dc-sp", "dc-sph", // = 43
			};

const char* printAlgo(TreeAlgo treeAlgo)
{
	return stringAlgo[treeAlgo];
}

TreeAlgo getTreeAlgo(const char *algo)
{
	for( int i = 0; i < END_TREE_ALGO; i++)
		if(strcmp(algo, stringAlgo[i]) == 0) return (TreeAlgo) i;

	return (TreeAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const TreeAlgo& treeAlgo)
{
	switch(treeAlgo)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(TREE_ALGO_ERROR)
	CASE(NO_TREE_ALGO)

	// unconstrained algorithms	
	CASE(SERVER_SHORTEST_PATH_TREE)
	CASE(PROXY_SHORTEST_PATH_TREE)
	CASE(SHORTEST_PATH_TREE)
	CASE(SHORTEST_PATH_TREE_STEINER)
	CASE(MINIMUM_SPANNING_TREE)
	CASE(PRUNED_MINIMUM_SPANNING_TREE)
	CASE(SHORTEST_PATH_HEURISTIC) 
	CASE(DISTANCE_NETWORK_HEURISTIC) 
	CASE(AVERAGE_DISTANCE_HEURISTIC)
	CASE(MINIMUM_DIAMETER_SHORTEST_PATH_HEURISTIC)
	CASE(MINIMUM_DIAMETER_DISTANCE_NETWORK_HEURISTIC)
	CASE(MINIMUM_DIAMETER_ONE_TIME_TREE_CONSTRUCTION)
	
	// degree constrained algorithms
	CASE(DEGREE_LIMITED_SHORTEST_PATH_TREE)
	CASE(DEGREE_LIMITED_SHORTEST_PATH_TREE_STEINER)
	CASE(DEGREE_LIMITED_MINIMUM_SPANNING_TREE)
	CASE(PRUNED_DEGREE_LIMITED_MINIMUM_SPANNING_TREE)
	CASE(COMPACT_TREE)
	CASE(PRUNED_COMPACT_TREE)
	CASE(DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC)
	CASE(DEGREE_LIMITED_SHORTEST_PATH_DISTANCE_NETWORK_HEURISTIC)
	CASE(DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)
	CASE(DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_BAUER)
	CASE(MINIMUM_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)
	CASE(MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION)
	CASE(MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_STEINER)
	CASE(MINIMUM_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC)

	// diameter bounded algorithms
	CASE(BOUNDED_DIAMETER_OTTC_SHORTEST_PATH_HEURISTIC)
	CASE(BOUNDED_DIAMETER_RGH_SHORTEST_PATH_HEURISTIC)
	CASE(BOUNDED_DIAMETER_RH_SHORTEST_PATH_HEURISTIC)
	CASE(ONE_TIME_TREE_CONSTRUCTION_BDMST)
	CASE(RANDOMIZED_GREEDY_HEURISTIC_BDMST)
	CASE(BOUNDED_DIAMETER_DISTANCE_NETWORK_HEURISTIC)
	CASE(BOUNDED_DIAMETER_RGH_DISTANCE_NETWORK_HEURISTIC)
	
	// diameter bounded degree limited algorithms
	CASE(BOUNDED_COMPACT_TREE)
	CASE(DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST)
	CASE(DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST_STEINER)
	CASE(DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST)
	CASE(DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST_STEINER)
	CASE(BOUNDED_DIAMETER_DEGREE_LIMITED_OTTC_SHORTEST_PATH_HEURISTIC)
	CASE(BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_SHORTEST_PATH_HEURISTIC)
	CASE(BOUNDED_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC)
	CASE(BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_DISTANCE_NETWORK_HEURISTIC)

	// radius constrained algorithms
	CASE(RADIUS_CONSTRAINED_SHORTEST_PATH_HEURISTIC)
	CASE(RADIUS_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)

	// delay constrained and degree bounded algorithms
	CASE(DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_TREE)
	CASE(DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH)
	CASE(DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)

	// delay constrained algorithms
	CASE(DELAY_CONSTRAINED_SHORTEST_PATH_TREE)
	CASE(DELAY_CONSTRAINED_SHORTEST_PATH)
	CASE(DELAY_CONSTRAINED_SHORTEST_PATH_HEURISTIC)
	
	CASE(END_TREE_ALGO)
#undef CASE
	default:
		cerr << "Missing operator<< case for TreeAlgo " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}

/*-----------------------------------------------------------------------
				DynamicTreeAlgo print
------------------------------------------------------------------------- */
const char *stringDynamicAlgo[] = { "error", "no_algo", 
				// insert
				"imm", 
				// degree bounded
				"ir",	// 3
				"i-mc", "i-mcwell", "i-mcnl", 
				"i-cn", "i-cmn", "i-cwell",  
				"i-mwell", 
		 		"i-mddl", "i-mrdl",
				"itr-mc",
				"itr-mddl",
				// delay constrained
				"idc-mc", "idc-mcwell", 
				"idc-cn", "idc-cwell", 
				"idc-mddl", // = 18
				// bounded diameter
				"ibd-mc", "ibd-mcwell", 
				"ibd-cn", "ibd-cwell", 	
				// bounded radius heuristics
				"ibr-mc", "ibr-mcwell", 
				"ibr-cn", "ibr-cwell", 	// = 26
				// subgraph
				"i-sub", //  = 27
				// unused
				"ibne", "imdbne", "imd", 
				// remove
				"rlf",  // = 31
				// degree bounded
				"rk", "rkwell", 
				"rtr-mc", "rtr-mddl", 
				"rtr-p-mc", "rtr-ps-mc", 
				"rtr-p-mddl", "rtr-ps-mddl",
				"r-mc", "rs-mc",  
				"r-mddl", "rs-mddl",  // = 43
				// delay constrained
				"rdc-mc", "rsdc-mc",  
				"rdc-mddl", "rsdc-mddl", // = 47
				// bounded diameter
				"rbd-mc", "rsbd-mc",  
				"rbd-mddl", "rsbd-mddl", 
				// bounded radius heuristics
				"rbr-mc", "rsbr-mc",  
				"rbr-mddl", "rsbr-mddl", // = 55
				// subgraph
				"r-sub", // = 56
				// unused
				"rmdbne", "rbne"
				//"imcsn",
				};

const char* printAlgo(DynamicTreeAlgo treeAlgo)
{
	return stringDynamicAlgo[treeAlgo];
}
DynamicTreeAlgo getDynamicTreeAlgo(const char *algo)
{
	for( int i = 0; i < END_DYNAMIC_TREE_ALGO; i++)
		if(strcmp(algo, stringDynamicAlgo[i]) == 0) return (DynamicTreeAlgo) i;

	return (DynamicTreeAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const DynamicTreeAlgo& treeAlgo)
{
	switch(treeAlgo)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(DYNAMIC_TREE_ALGO_ERROR)
	CASE(NO_DYNAMIC_TREE_ALGO)
	
	// -- insert --
	CASE(INSERT_MAKE_MEMBER)
	// degree bounded
	CASE(INSERT_RANDOM_EDGE)
	CASE(INSERT_MINIMUM_COST_EDGE)
	CASE(INSERT_MINIMUM_COST_EDGE_PRIORITY_WCN)
	CASE(INSERT_MINIMUM_COST_EDGE_NON_LEAF)
	CASE(INSERT_CENTER_NODE)
	CASE(INSERT_CENTER_MEMBER_NODE)
	CASE(INSERT_CENTER_WCN)
	CASE(INSERT_MINIMUM_WCN) 
	CASE(INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE)
	CASE(INSERT_MINIMUM_RADIUS_DEGREE_LIMITED_EDGE)
	CASE(INSERT_TRY_REPLACE_MC_NAIVE)
	CASE(INSERT_TRY_REPLACE_MDDL_NAIVE)
	// delay constrained and degree bounded
	CASE(INSERT_DC_MINIMUM_COST_EDGE)
	CASE(INSERT_DC_MINIMUM_COST_EDGE_PRIORITY_WCN)
	CASE(INSERT_DC_CENTER_NODE)
	CASE(INSERT_DC_CENTER_WCN)
	CASE(INSERT_DC_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE)

	// bounded diameter
	CASE(INSERT_BD_MINIMUM_COST_EDGE)
	CASE(INSERT_BD_MINIMUM_COST_EDGE_PRIORITY_WCN)
	CASE(INSERT_BD_CENTER_NODE)
	CASE(INSERT_BD_CENTER_WCN)

	// bounded radius heuristics
	CASE(INSERT_BR_MINIMUM_COST_EDGE)
	CASE(INSERT_BR_MINIMUM_COST_EDGE_PRIORITY_WCN)
	CASE(INSERT_BR_CENTER_NODE)
	CASE(INSERT_BR_CENTER_WCN)

	// subgraph
	CASE(INSERT_SUBGRAPH)
	
	// unused --
	CASE(INSERT_BEST_NEIGHBORHOOD_EDGE)
	CASE(INSERT_MINIMUM_DIAMETER_BEST_NEIGHBORHOOD_EDGE)
	CASE(INSERT_MINIMUM_DIAMETER_EDGE)
	//CASE(INSERT_MULTIPLE_CENTER_WCN)
	// unused end --
	// -- insert end --
	 
	// -- remove --
	CASE(REMOVE_LEAF)
	// degree bounded
	CASE(REMOVE_KEEP_AS_NON_MEMBER)
	CASE(REMOVE_KEEP_WELL_CONNECTED)
	CASE(REMOVE_TRY_REPLACE_MC_NAIVE)
	CASE(REMOVE_TRY_REPLACE_MDDL_NAIVE)
	CASE(REMOVE_TRY_REPLACE_PRUNE_MC)
	CASE(REMOVE_TRY_REPLACE_PRUNE_SEARCH_MC)
	CASE(REMOVE_TRY_REPLACE_PRUNE_MDDL)
	CASE(REMOVE_TRY_REPLACE_PRUNE_SEARCH_MDDL)
	CASE(REMOVE_MINIMUM_COST_EDGE)
	CASE(REMOVE_SEARCH_MINIMUM_COST_EDGE) 
	CASE(REMOVE_MINIMUM_DIAMETER_EDGE)
	CASE(REMOVE_SEARCH_MINIMUM_DIAMETER_EDGE) 
	// delay constrained and degree bounded
	CASE(REMOVE_DC_MINIMUM_COST_EDGE)
	CASE(REMOVE_DC_SEARCH_MINIMUM_COST_EDGE)
	CASE(REMOVE_DC_MINIMUM_DIAMETER_EDGE)
	CASE(REMOVE_DC_SEARCH_MINIMUM_DIAMETER_EDGE)

	// bounded diameter
	CASE(REMOVE_BD_MINIMUM_COST_EDGE)
	CASE(REMOVE_BD_SEARCH_MINIMUM_COST_EDGE)
	CASE(REMOVE_BD_MINIMUM_DIAMETER_EDGE)
	CASE(REMOVE_BD_SEARCH_MINIMUM_DIAMETER_EDGE)

	// bounded radius heuristics
	CASE(REMOVE_BR_MINIMUM_COST_EDGE)
	CASE(REMOVE_BR_SEARCH_MINIMUM_COST_EDGE)
	CASE(REMOVE_BR_MINIMUM_DIAMETER_EDGE)
	CASE(REMOVE_BR_SEARCH_MINIMUM_DIAMETER_EDGE)
	
	// subgraph
	CASE(REMOVE_SUBGRAPH)
	
	// unused --
	CASE(REMOVE_BEST_NEIGHBORHOOD_EDGE)
	CASE(REMOVE_MINIMUM_DIAMETER_BEST_NEIGHBORHOOD_EDGE)
	// unused end --
	// -- remove end --
		
	CASE(END_DYNAMIC_TREE_ALGO)
#undef CASE
		default:
		cerr << "Error! Missing operator for DynamicTreeAlgo " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}
/*-----------------------------------------------------------------------
				ReconfigAlgo print
------------------------------------------------------------------------- */
const char *stringReconfigAlgo[] = { "error", "no_algo", "tot", "topw", "ktr" };

const char *printAlgo(ReconfigAlgo algo)
{
	return stringReconfigAlgo[algo];
}

ReconfigAlgo getReconfigAlgo(const char *algo)
{
	for( int i = 0; i < END_RECONFIG_ALGO; i++)
		if(strcmp(algo, stringReconfigAlgo[i]) == 0) return (ReconfigAlgo) i;

	return (ReconfigAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const ReconfigAlgo& algo)
{
	switch(algo)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(RECONFIG_ALGO_ERROR)
	CASE(NO_RECONFIG_ALGO)
	
	CASE(TEST_OPTIMAL_TOTAL)
	CASE(TEST_OPTIMAL_PAIR_WISE)
	CASE(K_TOTAL_RECONFIG)
	
	CASE(END_RECONFIG_ALGO)
#undef CASE
		default:
		cerr << "ERROR! Missing operator for ReconfigAlgo " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}

/*---------------------------------------------------------------------------------
								DistributedTreeAlgo
---------------------------------------------------------------------------------*/
const char* stringDistributedTreeAlgo[] = { "error", "no_algo", "mst", "mdst" };

const char* printAlgo(DistributedTreeAlgo l)
{
	return stringDistributedTreeAlgo[l];
}
DistributedTreeAlgo getDistributedTreeAlgo(const char *algo)
{
	for( int i = 0; i < END_DISTRIBUTED_TREE_ALGO; i++)
		if(strcmp(algo, stringDistributedTreeAlgo[i]) == 0) return (DistributedTreeAlgo) i;

	return (DistributedTreeAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const DistributedTreeAlgo& type)
{
	switch(type)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(DISTRIBUTED_TREE_ALGO_ERROR)
	
	CASE(NO_DISTRIBUTED_TREE_ALGO)
	CASE(DISTRIBUTED_MINIMUM_SPANNING_TREE)
	CASE(DISTRIBUTED_MINIMUM_DIAMETER_SPANNING_TREE)
	
	CASE(END_DISTRIBUTED_TREE_ALGO)
#undef CASE
	default:
		cerr << "Missing operator<< case for DistributedTreeAlgo " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}

/*---------------------------------------------------------------------------------
								DistributedDynamicTreeAlgo
---------------------------------------------------------------------------------*/
const char* stringDistributedDynamicTreeAlgo[] = { "error", "no_algo", "i-mddl", "r-mddl" };

const char* printAlgo(DistributedDynamicTreeAlgo l)
{
	return stringDistributedDynamicTreeAlgo[l];
}
DistributedDynamicTreeAlgo getDistributedDynamicTreeAlgo(const char *algo)
{
	for( int i = 0; i < END_DISTRIBUTED_DYNAMIC_TREE_ALGO; i++)
		if(strcmp(algo, stringDistributedDynamicTreeAlgo[i]) == 0) return (DistributedDynamicTreeAlgo) i;

	return (DistributedDynamicTreeAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const DistributedDynamicTreeAlgo& type)
{
	switch(type)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(DISTRIBUTED_DYNAMIC_TREE_ALGO_ERROR)
	CASE(NO_DISTRIBUTED_DYNAMIC_TREE_ALGO)

	// insert
	CASE(DISTRIBUTED_DYNAMIC_INSERT_MINIMUM_DIAMETER)
	
	// remove
	CASE(DISTRIBUTED_DYNAMIC_REMOVE_MINIMUM_DIAMETER)
	
	CASE(END_DISTRIBUTED_DYNAMIC_TREE_ALGO)
#undef CASE
	default:
		cerr << "Missing operator<< case for DistributedDynamicTreeAlgo " << endl;
		ASSERTING(0);
		break;
	}

	return ostr;
}

} // namespace TreeAlgorithms


namespace GraphAlgorithms
{
/*-----------------------------------------------------------------------
				DynamicTreeAlgo print
------------------------------------------------------------------------- */
const char *stringDynamicMeshAlgo[] = { 
				"error", "no_algo",

				// insert
				"k-i-mc",
				"k-i-mddl",
				"k-i-mc-mddl",

				// remove
				"k-r-mc",
				"k-r-mddl",
				"k-rtr-mc",
				"k-rtr-mddl",
				"k-rtr-p-mc",
				"k-rtr-p-mddl",

				"no_algo"
				};

const char* printAlgo(DynamicMeshAlgo meshAlgo)
{
	return stringDynamicMeshAlgo[meshAlgo];
}

DynamicMeshAlgo getDynamicMeshAlgo(const char *algo)
{
	for( int i = 0; i < END_DYNAMIC_MESH_ALGO; i++)
		if(strcmp(algo, stringDynamicMeshAlgo[i]) == 0) return (DynamicMeshAlgo) i;

	return (DynamicMeshAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const DynamicMeshAlgo& meshAlgo)
{
	switch(meshAlgo)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(DYNAMIC_MESH_ALGO_ERROR)
	CASE(NO_DYNAMIC_MESH_ALGO)
	
	// insert
	CASE(K_INSERT_MC)
	CASE(K_INSERT_MDDL)
	CASE(K_INSERT_MC_MDDL)

	// remove
	CASE(K_REMOVE_MC)
	CASE(K_REMOVE_MDDL)
	CASE(K_REMOVE_TRY_REPLACE_MC_NAIVE)
	CASE(K_REMOVE_TRY_REPLACE_MDDL_NAIVE)
	CASE(K_REMOVE_TRY_REPLACE_PRUNE_MC)
	CASE(K_REMOVE_TRY_REPLACE_PRUNE_MDDL)

	CASE(END_DYNAMIC_MESH_ALGO)
#undef CASE
		default:
		cerr << "Error! Missing operator for DynamicMeshAlgo " << endl;
		ASSERTING(0);
		break;
	}

	return ostr;
}



/*-----------------------------------------------------------------------
		GraphAlgo - Supported Algorithms in the simulator
------------------------------------------------------------------------- */
const char *stringGraphAlgo[] = { "error", "no_algo", 
				// complete graphs
				"c", "cm", "cmnwcn", "cmkwcn",
				// prune algorithms
				"kbl", "acl", "aclo", "dl-aclo", "dl-bl",
				// mesh algorithms
				// kIT
				"k-mst", "k-dl-mst", "k-spt", "k-dl-spt", "k-md-ottc", "k-mddl-ottc", "k-dl-rgh",
				// kPT
				"kp-dl-mst", "kp-dl-spt", "kp-mddl-ottc", "k-dl-rgh",
				// k combined trees
				"kc-mst-bddlmst", "kc-mst-mddl", "kpc-mst-bddlmst", "kpc-mst-mddl",
				// enhanced tree algorithms
				"al-dl-mst", "al-dl-spt", "al-mddl-ottc", "al-dl-rgh",
				"cl-dl-mst", "cl-dl-spt", "cl-mddl-ottc", "cl-dl-rgh",
				"rd-dl-mst", "rd-dl-spt", "rd-mddl-ottc", "rd-dl-rgh",
				"re-dl-mst", "re-dl-spt", "re-mddl-ottc", "re-dl-rgh"
	};

const char *printAlgo(GraphAlgo algo)
{
	return stringGraphAlgo[algo];
}	

GraphAlgo getGraphAlgo(const char *algo)
{
	for( int i = 0; i < END_GRAPH_ALGO; i++)
		if(strcmp(algo, stringGraphAlgo[i]) == 0) return (GraphAlgo) i;

	return (GraphAlgo) 0;
}

std::ostream& operator<<(std::ostream& ostr, const GraphAlgo &graphAlgo)
{
	switch(graphAlgo)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(GRAPH_ALGO_ERROR)
	CASE(NO_GRAPH_ALGO)

	// complete graph
	CASE(COMPLETE_GRAPH)
	CASE(COMPLETE_MEMBER_GRAPH)
	CASE(COMPLETE_MEMBER_GRAPH_NEW_STEINER)
	CASE(COMPLETE_MEMBER_GRAPH_KEEP_STEINER)

	// prune
	CASE(K_BEST_LINKS)
	CASE(ADD_CORE_LINKS)
	CASE(ADD_CORE_LINKS_OPTIMIZED)
	CASE(ADD_CORE_LINKS_OPTIMIZED_DEGREE_LIMITED)
	CASE(DEGREE_LIMITED_BEST_LINKS)

	// mesh
	// k iterative trees
	CASE(K_MINIMUM_SPANNING_TREE)
	CASE(K_DEGREE_LIMITED_MINIMUM_SPANNING_TREE)
	CASE(K_SHORTEST_PATH_TREE)
	CASE(K_DEGREE_LIMITED_SHORTEST_PATH_TREE)
	CASE(K_MINIMUM_DIAMETER_OTTC_TREE)
	CASE(K_MINIMUM_DIAMETER_DEGREE_LIMITED_OTTC_TREE)
	CASE(K_DEGREE_LIMITED_RGH_TREE)

	// k parallel trees
	CASE(KP_DEGREE_LIMITED_MINIMUM_SPANNING_TREE)
	CASE(KP_DEGREE_LIMITED_SHORTEST_PATH_TREE)
	CASE(KP_MINIMUM_DIAMETER_DEGREE_LIMITED_OTTC_TREE)
	CASE(KP_DEGREE_LIMITED_RGH_TREE)

	// k combined trees
	CASE(K_COMBINED_DLMST_DLRGH)
	CASE(K_COMBINED_DLMST_MDDLOTTC)
	CASE(KP_COMBINED_DLMST_DLRGH)
	CASE(KP_COMBINED_DLMST_MDDLOTTC)
	
	// enhanced tree algorithms
	CASE(ADD_LONG_LINKS_TO_DLMST)
	CASE(ADD_LONG_LINKS_TO_DLSPT)
	CASE(ADD_LONG_LINKS_TO_MDDL_OTTC)
	CASE(ADD_LONG_LINKS_TO_DL_RGH)
	
	CASE(ADD_CORE_LINKS_TO_DLMST)
	CASE(ADD_CORE_LINKS_TO_DLSPT)
	CASE(ADD_CORE_LINKS_TO_MDDL_OTTC)
	CASE(ADD_CORE_LINKS_TO_DL_RGH)
	
	CASE(REDUCE_DIAMETER_DLMST)
	CASE(REDUCE_DIAMETER_DLSPT)
	CASE(REDUCE_DIAMETER_MDDL_OTTC)
	CASE(REDUCE_DIAMETER_DL_RGH)
	
	CASE(REDUCE_ECCENTRICITY_DLMST)
	CASE(REDUCE_ECCENTRICITY_DLSPT)
	CASE(REDUCE_ECCENTRICITY_MDDL_OTTC)
	CASE(REDUCE_ECCENTRICITY_DL_RGH)

#undef CASE
	default:
		cerr << "Missing operator<< case for GraphAlgo " << endl;
		ASSERTING(0);
		break;	
	}
	return ostr;
}

/*-----------------------------------------------------------------------
			Supported Algorithms in the simulator
------------------------------------------------------------------------- */
const char *stringCoreSelectionAlgo[] = { "error", "no_algo", 
									"median", "k-median", "k-center", 
									"k-broadcast-walk", "k-unicast-walk", 
									"k-broadcast-walk-tree", "k-unicast-walk-tree", 
									"k-uniform", 
									"k-tailed", 
									"k-bandwidth", 
									"k-random", "k-worst", 
									"error" };

const char *printAlgo(CoreSelectionAlgo algo)
{
	return stringCoreSelectionAlgo[algo];
}

CoreSelectionAlgo getCoreSelectionAlgo(const char *algo)
{
	for( int i = 0; i < END_CORE_SELECTION_ALGO; i++)
		if(strcmp(algo, stringCoreSelectionAlgo[i]) == 0) return (CoreSelectionAlgo) i;

	return (CoreSelectionAlgo) 0;
}
std::ostream& operator<<(std::ostream& ostr, const CoreSelectionAlgo &algo)
{
	switch(algo)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(CORE_SELECTION_ALGO_ERROR)
	CASE(NO_CORE_SELECTION_ALGO)
	CASE(MEDIAN_DISTANCE)
	CASE(AVERAGE_DISTANCE)
	CASE(CENTER_SELECTION)
	CASE(WALK_SELECTION)
	CASE(UNICAST_WALK_SELECTION)		
	CASE(WALK_SELECTION_GROUP_TREE)
	CASE(UNICAST_WALK_SELECTION_GROUP_TREE)		
	CASE(UNIFORM_SELECTION_LAYOUT)
	CASE(TAILED_SELECTION)
	CASE(BANDWIDTH_SELECTION)
	CASE(RANDOM_SELECTION)
	CASE(WORST_SELECTION)
#undef CASE
	default:
		cerr << "Missing operator<< case for CoreSelectionAlgo " << endl;
		ASSERTING(0);
		break;
	}
	return ostr;
}

} // namespace GraphAlgorithms



