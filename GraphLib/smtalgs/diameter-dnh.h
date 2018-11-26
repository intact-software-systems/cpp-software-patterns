/***************************************************************************
                          diameter-dnh.cc  -  description
                             -------------------
    begin                : Sat Jul 14 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DIAMETER_DISTANCE_NETWORK_HEURISTIC_KHV
#define GRAPHALGO_DIAMETER_DISTANCE_NETWORK_HEURISTIC_KHV

// My Includes
#include "steiner_class.h"
#include "../treealgs/treestructure.h"

using namespace boost;
using namespace TreeAlgorithms;

class DiameterDNH : SteinerHeuristic
{
private:
	TreeStructure	&T_dnh;			// Stores the DNH tree
	
public:
	DiameterDNH(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in),  T_dnh(out) {} 
	~DiameterDNH() { }  
	
	// Distance Network Heuristic main function
	void Algorithm(vertex_descriptorN zsource);
		
	// Distance Network Heuristic helper functions
	//void ConstructDNGraph(GraphN &DNGraph, vertex_descriptorN zsource, vertex_descriptorN &dngzsource);
	void ConstructDNGraph(GraphN &DNGraph);
	bool FindST(const VertexSet &V, GraphN &DNGraph, vertex_descriptorN zsource, ShortestPathKeeper &spk);
	void ConstructSPGraph(const GraphN &dn_graph, GraphN &sp_graph, const ShortestPathKeeper &spk);
	void ConstructDNHeuristicGraph(GraphN &dnh_graph, GraphN &temp_sp_graph, VertexSet &V);
	void ConstructDNHTree(GraphN &dnh_graph, const ShortestPathKeeper &dnh_mst);
	
	// Tiny test
	//void DNHTest(GraphN &dnh_graph);
};


#endif // GRAPHALGO_DIAMETER_DISTANCE_NETWORK_HEURISTIC_KHV 


