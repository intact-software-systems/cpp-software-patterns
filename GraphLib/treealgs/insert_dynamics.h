/***************************************************************************
                          insert_dynamics.h  -  description
                             -------------------
    begin                : Thu Aug 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_INSERT_DYNAMICS_KHV
#define GRAPHALGO_INSERT_DYNAMICS_KHV

#include "../boostprop.h"
#include "treealgs.h"
#include "tree_dynamics.h"
#include "../network/group_info.h"
#include "../graphalgs/steiner_set.h"

using namespace std;
using namespace boost; 

/*-----------------------------------------------------------------------
					class InsertDynamics()
------------------------------------------------------------------------- */
class InsertDynamics : TreeDynamics
{
public:
	InsertDynamics(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, vertex_descriptorN actionVert)
		   	: TreeDynamics(g, groupInfo, fifoSteiner, actionVert) {}
	~InsertDynamics() {}

	// main functions
	void insertVertex(const DynamicTreeAlgo &algo);
 	clock_t insertVertex_in(const DynamicTreeAlgo &algo);

 	// insert
	void ivCNode();
	void ivCNodeGM();
	void ivCNodeSP();
	void ivMCNodeSP();
	void ivMCE();
	void ivMCE(vertex_descriptorN, VertexState, VertexFunction);
	void ivMCEPrioritySP();
	void ivMCENonLeaf();
	void ivRandomEdge();
	void ivMSP();
	void ivTRMCN();
	void ivTRMDDLN();
	void ivBNP();
	void ivMDBNP();
	void ivMDP();
	void ivMDDLP();
	void ivMRDLP();

	// delay constriained
	void ivDCCNode();
	void ivDCCNodeSP();
	void ivDCMCE();
	void ivDCMCE(vertex_descriptorN, VertexState, VertexFunction);
	void ivDCMCEPrioritySP();
	void ivDCMDDLP();

	// bounded diameter -> bounded_diameter_dynamics.cc
	void ivBDMCE();
	void ivBDMCEPriorityWCN();
	void ivBDCNode();
	void ivBDCNodeWCN();
	
	int relax_ivBRMCE(const DistanceVector &distance, DistanceVector &degree_bound, DistanceVector &radius_bound, VertexSet &degreeBroken, VertexSet &radiusBroken);
	int relax_ivBDMCE(const TreeDiameter &TD, DistanceVector &degree_bound, DistanceVector &diameter_bound, VertexSet &degreeBroken, VertexSet &diameterBroken);

	// radius heuristics
	void ivBRMCE();
	void ivBRMCEPriorityWCN();
	void ivBRCNode();
	void ivBRCNodeWCN();

	// subgraph
	void ivSubGraph(TreeStructure &subT, VertexSet& steinerSet);
};
 	
 
#endif 



