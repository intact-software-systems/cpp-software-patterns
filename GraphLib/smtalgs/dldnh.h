/***************************************************************************
                          dldnh.h  -  description
                             -------------------
    begin                : Sun Jun 17 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC_KHV
#define GRAPHALGO_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC_KHV

// My Includes
#include "steiner_class.h"
#include "../treealgs/treestructure.h"

using namespace boost;
using namespace TreeAlgorithms;

class DLDistanceNetworkHeuristic : SteinerHeuristic
{
private:
	TreeStructure	&T_dnh;			// Stores the DNH tree
	
public:
	DLDistanceNetworkHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in),  T_dnh(out) {} 
	~DLDistanceNetworkHeuristic() { }  
	
	// Distance Network Heuristic main function
	void Algorithm(vertex_descriptorN zsource);
		
	// Distance Network Heuristic helper functions
	void ConstructDNGraph(GraphN &DNGraph, vertex_descriptorN zsource, vertex_descriptorN &dngzsource);
	void FindMST(GraphN &DNGraph, vertex_descriptorN zsource, ShortestPathKeeper &spk);
	void ConstructSPGraph(const GraphN &dn_graph, GraphN &sp_graph, const ShortestPathKeeper &spk);
	void ConstructDNHeuristicGraph(GraphN &dnh_graph, GraphN &temp_sp_graph);
	void ConstructDNHTree(GraphN &dnh_graph, const ShortestPathKeeper &dnh_mst);
	
	// Tiny test
	//void DNHTest(GraphN &dnh_graph);
};


#endif // GRAPHALGO_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC_KHV 


